#ifndef __PYRAM_H__
#define __PYRAM_H__

#include <ignBufferImage.h>
#include <sxs/sxsImageScaleSets.h>
#include <sxs/sxsImageScaleClimbingUser.h>
#include <sxs/sxsN_Verbose_User.h>
#include <sxs/sxsE_MumfordShah.h>


class MS_ScaleClimbingUser: public virtual sxs::N_Verbose_User, public virtual sxs::E_MumfordShah_User {
};

template<class T>
sxs::ImageScaleSets*            ComputePyramid(ign::image::BufferImage<T>* ttim, size_t verbose=0);

template<class T>
void                            CutPyramid(sxs::ImageScaleSets* sxs,  ign::image::BufferImage<T>* img, float cut);

template<class T>
ign::image::BufferImage<int>*   SurSegmente(ign::image::BufferImage<T>* image, float p=10000);

int                             SavePyramid(sxs::ImageScaleSets *sxs, std::string const &issfilename, std::string const &imagefilename, size_t verbose = 0);

sxs::ImageScaleSets*            LoadPyramid(std::string const &filename);


#include "pyram_code.h"


#endif // __SEGMENT_H__
