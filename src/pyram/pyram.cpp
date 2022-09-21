#include <iostream>
#include <fstream>
#include <vector>
#include <fstream>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>

#include <ignImageDriverManager.h>
#include <ignMPDImageDriver.h>
#include <ignImageEntropy.h>
#include <ignImageModBord.h>
#include <ignImageNumRegion.h>

#include <sxs/sxsImageScaleSets.h>
#include <sxs/sxsImageScaleClimbing.h>

#include <exception.h>
#include <pyram.h>

using namespace sxs;

/**
 * Cette macro envoie le message dans la console et le flux de log.
 * Elle s'utilise presque comme un flux habituel (void \URL[Tutorial]{ExempleSORTIEMESSAGE.html}
 */
#define PYRAM_SORTIEMESSAGE(a) \
{ \
  std::ostringstream oss_sortiemessage; \
  oss_sortiemessage << a; \
  std::cout<<oss_sortiemessage.str(); \
  std::clog<<oss_sortiemessage.str() ; \
  std::cout.flush(); std::clog.flush(); \
}

#define PYRAM_SORTIEERREUR(a) PYRAM_SORTIEMESSAGE(a)

/**
* Affichage d'un message d'alerte.
*
* Cette macro fonctionne comme 
* SORTIEMESSAGE 
*, mais n'envoie le message que si les warnings sont actifs.
* Pour activer les warnings, voir 
* Systeme::EnableWarnings
*/
#define PYRAM_SORTIEWARNING(a) { PYRAM_SORTIEMESSAGE ("WARNING : "<< a) ; }

enum Actions {
	RIEN, PYRAMIDE, CUT, PYRAMIDE_AND_CUT
};


int
SavePyramid(sxs::ImageScaleSets *sxs, std::string const &issfilename, std::string const &imagefilename, size_t verbose) {

	std::cout<<"test "<<std::endl;
	if (verbose > 0) {
		SORTIEMESSAGE("Sauvegarde de la pyramide "<< issfilename <<std::endl);
	}
	std::cout<<"test b "<<std::endl;
	
#ifdef LGL_WITH_BOOST
	lgl::Filename issfile(issfilename);
	lgl::Filename OriFile(imagefilename);
	std::cout<<"test c "<<std::endl;

#else
	lgl::Filename issfile(wxString(issfilename.c_str(), *wxConvCurrent));
	lgl::Filename OriFile(wxString(imagefilename.c_str(), *wxConvCurrent));
#endif
	if (!sxs->save(issfile, OriFile)) {
		SORTIEMESSAGE("Probleme lors de la sauvegarde de la pyramide."<<std::endl);
		return -1;
	}

	return 0;
}

sxs::ImageScaleSets*
LoadPyramid(std::string const &filename) {
	SORTIEMESSAGE("Lecture de la pyramide "<< filename <<std::endl);
	//lgl::Filename file(wxString(filename.c_str(), *wxConvCurrent));
	lgl::Filename file(filename);

	sxs::ImageScaleSets* sxs = new sxs::ImageScaleSets;
	lgl::BOOL test = sxs->load(file);
	if (!test) {
		SORTIEMESSAGE("Probleme lors de la lecture de la pyramide."<<std::endl);
		delete sxs;
		return NULL;
	}
	return sxs;
}


int
cuts_pyram(sxs::ImageScaleSets* sxs, std::vector<float> const &cut_value,
           std::vector<std::string> const &nom_out, int mode_bord,
        bool mode_seve, size_t verbose) {

    if (cut_value.size() != nom_out.size()) {
        SORTIEMESSAGE("Nombre de fichiers et de valeurs de coupe differents !!!" << std::endl);
        return -1;
    }
    if (mode_seve && verbose > 0)
        SORTIEMESSAGE("Mode Compatible SEVE active." << std::endl);

    for (unsigned int i = 0; i < cut_value.size(); ++i) {

        ign::image::BufferImage<int> img;
        CutPyramid(sxs, &img, cut_value[i]);
        ign::image::renumerotation(img);

        // code specifique pour etre compatible avec un logiciel de classif qui s'appelle Seve.
        // fait le contraire du sous ech 2 fait avant
        // et renumerote les labels pour avoir des numeros continus (1 puis 2 puis 3...)
        // sort des images codees en unsigned short.
        if (mode_seve) {

            ign::image::BufferImage<unsigned short> img2;
            img2.initialize(img.numCols() * 2, img.numLines() * 2, img.numBands());

            for (size_t l = 0; l < img2.numLines(); ++l)
                for (size_t c = 0; c < img2.numCols(); ++c)
                    img2(c, l) = img(c / 2, l / 2);

            if (mode_bord > 0) {
                ign::image::BufferImage<unsigned short>* cropped = ign::image::deleteBorder(img2, mode_bord * 2);
                ign::image::renumerotation(*cropped);
                cropped->save(nom_out[i].c_str());
            } else {
                img2.save(nom_out[i].c_str());
            }

        } else {

            // sort des images codees en int.
            if (mode_bord > 0) {
                ign::image::BufferImage<int>* cropped = ign::image::deleteBorder(img, mode_bord);
                ign::image::renumerotation(*cropped);
                cropped->save(nom_out[i].c_str());

            } else {
                img.save(nom_out[i].c_str());
            }

        }
        if (verbose > 0) {
            SORTIEMESSAGE(" ; Fichier = " << nom_out[i] << std::endl);
        }
    }

    return 0;
}

sxs::ImageScaleSets*
compute_pyram(std::string const &filename, unsigned int ssech, int mode_bord, float alpha, int verbose) {

    if (verbose >= 0) {
        std::cout << "Calcul de la pyramide (image:" << filename << ",ssech:"
				<< ssech << ",alpha:" << alpha << ",verbose:" << verbose << ")."
                << std::endl;
    }
    try {

        if (!boost::filesystem::exists(filename)) {
            PYRAM_SORTIEERREUR("Le fichier '" << filename << "' n'existe pas." << std::endl);
            return NULL;
        }

        ign::image::BufferImage<unsigned char>* base = new ign::image::BufferImage<unsigned char>(filename);

        ign::image::underSampling(*base, ssech);

        ign::image::BufferImage<unsigned char>* ttim;
        if (mode_bord > 0) {
            ttim = ign::image::addBorderMirror(base, mode_bord);
        } else {
            ttim = base;
        }

        return ComputePyramid(ttim);

    } catch (std::exception& e) {

// petit trick au cas ou on essaie d'ouvrir un ECW sous windows et qu'on oublie de mettre une dll au bon endroit.
#if defined (__WIN32__)
        bool ddlExists = boost::filesystem::exists( "gdalplugins\\gdal_ECW_JP2ECW.dll");
        if (boost::algorithm::ends_with(filename, ".ecw") && !ddlExists) {
            PYRAM_SORTIEWARNING("Format ECW, Plugin GDAL manquant, DLL manquante : gdalplugins/gdal_ECW_JP2ECW.dll" << std::endl);
        }
#endif

        PYRAM_SORTIEERREUR(e.what() << std::endl);
        return NULL;
    }
}


int seg_and_cut(int argc, char **argv) {

	unsigned int ssech = 1;
	float alpha = 1.0;
	int verbose = 0;
	bool mode_seve = false;
	int mode_bord = 0; //Ajout gestion des bords 30/08/2007
	std::string adresse_texture;
	bool calcul_texture = false;

	if (argc <= 1) {
		std::cout << "Usage : " << argv[0] << " param.txt" << std::endl;
		std::cout << " avec param.txt contenant :" << std::endl;
		std::cout << "ACTION pyramide ou cut ou pyramide-and-cut" << std::endl;
		std::cout << std::endl;
		std::cout << "Si ACTION vaut pyramide :" << std::endl;
		std::cout << "IMAGEFILE nom-du-fichier-image" << std::endl;
		std::cout << "[SOUSECH] facteur de sous-echantillonnage (defaut : " << ssech << ")" << std::endl;
		std::cout << "[ALPHA] alpha-du-deriche (defaut : " << alpha << ")" << std::endl;
		std::cout << "[VERBOSE] niveau-de-verbosite (defaut : " << verbose << ")" << std::endl;
		std::cout << "[ISSFILE nom-du-fichier-iss (defaut : nom-du-fichier-image.iss)" << std::endl;
		std::cout << "[MODEBORD 1 si prise en compte des bords (defaut : pas de bord)" << std::endl;
		std::cout << std::endl;
		std::cout << "Si ACTION vaut cut :" << std::endl;
		std::cout << "ISSFILE nom-du-fichier-iss" << std::endl;
		std::cout << "[MODESEVE] (images de labels compatibles avec le logicel Seve du SBV, defaut : " << mode_seve << ")" << std::endl;
		std::cout << "CUTS valeur nom-du-fichier" << std::endl;
		std::cout << "[CUTS valeur nom-du-fichier]" << std::endl;
		std::cout << "[CUTS valeur nom-du-fichier]" << std::endl;
		std::cout << "..." << std::endl;
		std::cout << std::endl;
		std::cout << "Si ACTION vaut pyramide-and-cut :" << std::endl;
		std::cout << "IMAGEFILE nom-du-fichier-image" << std::endl;
		std::cout << "[SOUSECH] facteur de sous-echantillonnage (defaut : " << ssech << ")" << std::endl;
		std::cout << "[ALPHA] alpha-du-deriche (defaut : " << alpha << ")" << std::endl;
		std::cout << "[VERBOSE] niveau-de-verbosite (defaut : " << verbose << ")" << std::endl;
		std::cout << "[ISSFILE] nom-du-fichier-iss (defaut : pas de sauvegarde)" << std::endl;
		std::cout << "[MODESEVE] (images de labels compatibles avec le logicel Seve du SBV, defaut : " << mode_seve << ")" << std::endl;
		std::cout << "[MODEBORD 1 si prise en compte des bords (defaut : pas de bord)" << std::endl;
		std::cout << "CUTS valeur nom-du-fichier" << std::endl;
		std::cout << "[CUTS valeur nom-du-fichier]" << std::endl;
		std::cout << "[CUTS valeur nom-du-fichier]" << std::endl;
		std::cout << "..." << std::endl;
		return 0;
	}

	Actions act = RIEN;
	std::string issfilename, imagefilename;
	std::vector<float> cut_value;
	std::vector<std::string> nom_out;

	// lecture du fichier de parametres
	{
		std::ifstream fic(argv[1]);
		if (!fic.good()) {
			std::cout << "Le fichier '" << argv[1] << "' n'existe pas." << std::endl;
			return 1;
		}

		while (fic.good()) {
			std::string mot_cle;
			fic >> mot_cle;
			if (mot_cle == "ACTION") {
				std::string lecture;
				fic >> lecture;
				if (lecture == "cut")
					act = CUT;
				else if (lecture == "pyramide")
					act = PYRAMIDE;
				else if (lecture == "pyramide-and-cut")
					act = PYRAMIDE_AND_CUT;
				else
					std::cout << "ACTION " << lecture << "non comprise : IGNORE" << std::endl;
			} else if (mot_cle == "IMAGEFILE") {
				fic >> imagefilename;
			} else if (mot_cle == "SOUSECH") {
				fic >> ssech;
			} else if (mot_cle == "ALPHA") {
				fic >> alpha;
			} else if (mot_cle == "VERBOSE") {
				fic >> verbose;
			} else if (mot_cle == "ISSFILE") {
				fic >> issfilename;
			} else if (mot_cle == "MODESEVE") {
				fic >> mode_seve;
			}
			//Ajout possibilite de calcul de la texture
			else if (mot_cle == "TEXTURE")
			{
				fic >> adresse_texture;
				if(adresse_texture!=std::string("")){calcul_texture=true;}
			}
			//Ajout gestion des bords 30/08/2007
			else if (mot_cle == "MODEBORD") {
				fic >> mode_bord;
			}
			//Fin ajout gestion des bords 30/08/2007
			else if (mot_cle == "CUTS") {
				std::string nom;
				float val;
				fic >> val >> nom;
				cut_value.push_back(val);
				nom_out.push_back(nom);
			} else if (mot_cle.length() != 0)
				std::cout << "Mot cle (" << mot_cle << ") non compris : IGNORE" << std::endl;
		}
	}

	if (verbose > 0) {
		std::cout << "IMAGE FILE : " << imagefilename << std::endl;
		std::cout << "ACTION : " << act << std::endl;
		std::cout << "SOUSECH : " << ssech << std::endl;
		std::cout << "ALPHA : " << alpha << std::endl;
		std::cout << "VERBOSE : " << verbose << std::endl;
		std::cout << "MODESEVE : " << mode_seve << std::endl;
		std::cout << "MODEBORD : " << mode_bord << std::endl;
		if (calcul_texture) {
			std::cout << "TEXTURE : " << adresse_texture << std::endl;
		}
	}

	//Ajout calcul texture pour SEVE
	if(calcul_texture){
        if (verbose) std::cout << "Calcul de la texture (structure entropique de l'image)" << std::endl;
        
		ign::image::BufferImage<unsigned char> imgin(imagefilename.c_str());
        if (verbose) std::cout << "Calcul de la texture - Image chargée" << std::endl;
		ign::image::entropicStructureImage(imgin);
        if (verbose) std::cout << "Calcul de la texture - Calcul fait" << std::endl;
		imgin.save(adresse_texture.c_str());
        if (verbose) std::cout << "Calcul de la texture OK" << std::endl;
	}

	// astuce pour que la sortie standard flush vraiment à chaque caractère,
	// comme ça, on voit bien le pourcentage changer, c'est important quand c'est long !

	if (verbose > 1) {
		std::cout << std::unitbuf;
	}

	int return_value = 0;
	sxs::ImageScaleSets* sxs = NULL;

	if ((act == PYRAMIDE) && (issfilename.length() == 0))
		issfilename = imagefilename + std::string(".iss");

	if (act == PYRAMIDE || act == PYRAMIDE_AND_CUT) {

        sxs = compute_pyram(imagefilename, ssech, mode_bord, alpha, verbose); 
		
		if (sxs == NULL) {
			std::cout << "Probleme de fabrication de la pyramide." << std::endl;
			return_value = -10;
		} else {
			if (issfilename.length() != 0) {
                imagefilename = "ori";
				std::cout << "call SavePyramid" << std::endl;

                return_value = SavePyramid(sxs, issfilename, imagefilename);
			}
		}
	}
	if (return_value != 0)
		return return_value;

	if (act == CUT) {
        sxs = LoadPyramid(issfilename);
	}

	if (sxs == NULL) {
		std::cout << "Probleme de fabrication de la pyramide." << std::endl;
		return_value = -10;
	} else {
		if (verbose > 0) {
			std::cout << "Fin de fabrication de la pyramide." << std::endl;
		}
	}

	if (return_value != 0)
		return return_value;

	if (act == CUT || act == PYRAMIDE_AND_CUT) {
        return_value = cuts_pyram(sxs, cut_value, nom_out, mode_bord, mode_seve, verbose);
	}

	return return_value;
}


int main(int argc, char* argv[]) {

    // image::io::driver::ImageDriverManager::Instance()->unregisterDriver(image::io::driver::MPDImageDriver::DriverName());

	// setNotifyLevel(FATAL);

    std::stringstream errorStream;
    bool caucht = 0;
	try {
		int return_value = seg_and_cut(argc, argv);
		return return_value;
    }
    PYRAM_CATCH_ALL_EXCEPTIONS(errorStream, caucht, 0);

    if (caucht) {

        std::cout << "PYRAM ERROR !\n" << errorStream.str() << std::endl;
        return 1;
    }
}
