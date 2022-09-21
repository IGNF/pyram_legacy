#include <ignBufferImage.h>
#include "ignImageSampling.h"

#include <ignImageDeriche.h>
#include <ignImageAggChannels.h>
#include <ignImageWatershed.h>

// lGl headers
#include <lgl/lglString.h>
#include <lgl/lglNoyau.h>
#include <lgl/lglImageIO.h>
#include <sxs/macroSortieMessage.h>
#include "sxs/sxsImageScaleSets.h"



template <class T>
ign::image::BufferImage<int>*
SurSegmente(ign::image::BufferImage<T>* ttim, float p) {

    ign::image::BufferImage<float> modDer;
    {
        ign::image::BufferImage<float> gX, gY;
        ign::image::Deriche(*ttim, gX, gY, 1.0);
        ign::image::ModuleDeriche(gX, gY, modDer);
        modDer.close();
    }

    ign::image::BufferImage<float>* aggregatedModDer = new ign::image::BufferImage<float>();
    aggregatedModDer = ign::image::aggregateCannelsMax(modDer);

    ign::image::BufferImage<int>* bassins = new ign::image::BufferImage<int>();
    ign::image::BufferImage<unsigned char> imdir;
    int NbBass = 0; // Nombre de bassins versants

    ign::image::Watershed(*aggregatedModDer, imdir, *bassins, NbBass, p);

    delete aggregatedModDer;

    return bassins;
}

template<class T>
void CutPyramid(sxs::ImageScaleSets* sxs,  ign::image::BufferImage<T>* img, float cut) {

    lgl::ImageRam *Section = 0;
    sxs->getSection(cut, Section);
    lgl::TImageRamToBufferImage(Section->type(), *Section, *img);

    delete Section;

}

template<class T>
sxs::ImageScaleSets* ComputePyramid(ign::image::BufferImage<T>* ttim, size_t verbose) {

    BufferImage<int>* bassins = SurSegmente(ttim);


    // Transforms it into an lgl::ImageRamT
    // NB : in sxs the input objects must be allocated dynamically (but are freed by the objects which catch them)
    lgl::ImageRamT<T>* image = new lgl::ImageRamT<T>;
    lgl::ImageRamT<int>* segmentationBase = new lgl::ImageRamT<int>;

    lgl::BufferImageToImageRamT(*ttim, *image);
    lgl::BufferImageToImageRamT(*bassins, *segmentationBase);

    /// Allocation of the ImageScaleSets
    sxs::ImageScaleSets* sxs = new sxs::ImageScaleSets;
    /// Allocation of the scale-climbing user
    MS_ScaleClimbingUser* user = new MS_ScaleClimbingUser;
    user->setImage(*image);
    user->verboseLevel() = verbose;
    // The algorithm
    sxs::ImageScaleClimbing algo;
    algo.setUser(user);
    algo.setScaleSets(*sxs);
    algo.setImage(image);
    algo.setBaseSegmentation(segmentationBase);
    // Its parameters
    sxs::ImageScaleClimbingParameters* param = new sxs::ImageScaleClimbingParameters;
    // The neighborhood for the base segmentation and the graph construction
    lgl::Neighborhood N(lgl::Neighborhood::d2_8n);
    param->setNeighborhood(N);
    algo.setParameters(param);
    // Passes the algorithm to the user for run-time information recuperation (
    user->setAlgorithm(algo);
    ///=========================================================================
    // Computation
    if (!algo.run()) {
        SORTIEMESSAGE("Probleme lors du calcul de la pyramide\n");
        delete sxs;
        return NULL;
    }

    ///=========================================================================
    return sxs;
}
