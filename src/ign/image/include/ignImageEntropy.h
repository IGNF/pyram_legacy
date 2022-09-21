/*
 Salade --- Roger Trias i Sanz  2002-2005
 Systeme Automatique de Localisation Agraire et de Detection d'Equi-cultures

   Structuration entropique comme attribut de texture
   De "Analyse d'images aeriennes stereo pour la restitution des milieux urbains"
   Caroline Baillard, these de doctorat
*/
#ifndef __ENTROPY_H__
#define __ENTROPY_H__

#include <string>
#include <iostream>
#include <cassert>
#include <cmath>

#include <ignBufferImage.h>
#include <ignImageDeriche.h>
// #include <ign/filesystem/PathFinder.h>
//#include <ign/filesystem/utils.h>


#define log2(a) log10((double)a)/log10((double)2)

using namespace ign::image;
using namespace std;

namespace ign {
	namespace image {

		static inline double round(double val)	{
			return floor(val + 0.5);
		}

		double atan2_thr(double y, double x);

		/// \brief calcul de la structure entropique de l'image.
		void entropicStructureImage(BufferImage<unsigned char> & imgin);


			//  Pour chaque ds-ieme pixel dans l'image (sous-echantillonage par ns*ns) on calcule
			// la structuration entropique pour un voisinage pondere gaussiennement avec sigmasq
			// (sigma*sigma) donne, avec 'nbins' bins d'histogramme
			//
			// Entrees: l'image à 1 canal dont on veut calculer la structuration entropique;
			// on en calcule le gradient par Deriche avec 'alpha'
			//
			// Si 'verbose', indique la progression du calcul
			//
			// Le resultat est une image à 2 canaux, de taille celle des entrees divisee par ns.
			// Le canal 0 contient le niveau de "non-artificialite", le canal 1 contient
			// la fraction de pixels du voisinage qui sont sufissament forts (module du gradient > minmod).
			// Des valeurs faibles du canal 1 indiquent que la zone n'a pas d'orientations privilegiees,
			// et donc il s'agit d'une zone "vide".
			// Pour des valeurs forts du canal 1, alors des valeurs faibles du canal 0 indiquent des zones
			// artificielles et des valeurs forts indiquent des zones naturelles.
			//
			// Le gradient prend typiquement des valeurs de 0.0 à 255.0 (moyenne=8.0 dans un exemple typique).
			// Donc, une bonne valeur de 'minmod' serait 4.0
			ign::image::BufferImage<float>* entropicStructureOneChannel(const ign::image::BufferImage<unsigned char> & image, int ds, int nbins,
						float minmod=4.0, float sigmasq=100, float alpha=0.5, bool verbose=false);


			//generalisation de la fonction
			template < class T >
			ign::image::BufferImage<float>* entropicStructureOneChannel(const ign::image::BufferImage< T > & image,
				int ds, 
				int nbins,
				bool verbose=false){
				float minmod=4.0;
				float sigmasq=100;
				float alpha=1;

				if(verbose)std::cout<<"debut de l'algorthyme de calcul de l'entropie pour les images en generales"<<std::endl;
				ImageSize src_size = image.size(1);
			ImageSize out_size((int) (ceil(1.0 * src_size.first / ds)), (int) (ceil(1.0 * src_size.second / ds)));
			BufferImage<float>* result = new BufferImage<float>(out_size.first, out_size.second, 2, 0.0);

			double *histogram = new double[nbins];

			// calcul du module et argument du gradient
			if (verbose)
				std::cout << "preparing... " << std::flush;
			BufferImage<float> gradientX, gradientY, gradientM, gradientA;
			gradientX.initialize(src_size.first, src_size.second, 1);
			gradientY.initialize(src_size.first, src_size.second, 1);
			gradientM.initialize(src_size.first, src_size.second, 1);
			gradientA.initialize(src_size.first, src_size.second, 1);

			Deriche(image, gradientX, gradientY, alpha);
			double ming = 1000.0, maxg = 0.0, aveg = 0.0;
			for (size_t y = 0; y < src_size.second; ++y)
				for (size_t x = 0; x < src_size.first; ++x) {
					float mod = (float) hypot(gradientY(x, y, 0), gradientX(x, y, 0));
					float arg = (float) atan2_thr(gradientY(x, y, 0), gradientX(x, y, 0));

					if (arg < 0)
						arg += (float) M_PI * 2.0f;
					if (arg >= (float) M_PI * 2.0f)
						arg = 0.0;
					gradientM(x, y, 0) = mod;
					gradientA(x, y, 0) = arg; // de 0 à 2*pi

					aveg += mod;
					if (mod > maxg)
						maxg = mod;
					if (mod < ming)
						ming = mod;
				}

			aveg /= (src_size.first * src_size.second);
			if (verbose)
				std::cout << std::fixed << std::setprecision(6) << "gradient: min=" << ming << " max=" << maxg << " ave=" << aveg << std::endl;

			// precalcul du filtre gaussien
			// quelle taille?
			int gaussian_size = (int) (ceil(sqrt(-2 * sigmasq * std::log(0.05)))); // coupe à 5%
			BufferImage<float> gaussian(gaussian_size + 1, gaussian_size + 1, 1);
			for (int y = 0; y < gaussian_size + 1; ++y)
				for (int x = 0; x < gaussian_size + 1; ++x)
					gaussian(x, y, 0) = (float) exp(-0.5 * ((x * x) + (y * y)) / sigmasq);

			// et on scanne l'image
			if (verbose)
				std::cout << "scanning " << std::flush;
			double angle_factor = nbins / (M_PI * 2.0);
			//  Percent percent(out_size.y-1);
			size_t inx, iny, outx, outy;
			for (iny = ds / 2, outy = 0; outy < out_size.second; iny += ds, ++outy) {
				//    if (verbose) percent=outy;
				for (inx = ds / 2, outx = 0; outx < out_size.first; inx += ds, ++outx) {
					double numpoints = 0.0, numvalid = 0.0;
					for (int p = 0; p < nbins; ++p)
						histogram[p] = 0;
					for (int sy = -gaussian_size; sy <= gaussian_size; ++sy)
						for (int sx = -gaussian_size; sx <= gaussian_size; ++sx) {
							size_t cx = inx + sx;
							size_t cy = iny + sy;
							if (/*cx < 0 ||*/ cx >= src_size.first || /*cy < 0 ||*/ cy >= src_size.second)
								continue;
							numpoints += gaussian(abs(sx), abs(sy), 0);
							if (gradientM(cx, cy, 0) < minmod)
								continue;
							numvalid += gaussian(abs(sx), abs(sy), 0);
							int bin = (int) (round(gradientA(cx, cy, 0) * angle_factor));
							if (bin >= nbins)
								bin = nbins - 1;
							histogram[bin] += gaussian(abs(sx), abs(sy), 0);
						}

					// relation de points valides
					assert(numpoints > 0.0);
					(*result)(outx, outy, 1) = (float) (numvalid / numpoints);

					// entropie de l'histogramme
					double entropy = 0, p;
					if (numvalid == 0.0) {
						(*result)(outx, outy, 0) = 0.0;
						continue;
					}
					for (int sa = 0; sa < nbins; ++sa) {
						p = histogram[sa] / numvalid;
						if (p > 0.00001)
							entropy -= p * log2(p);
					}
					(*result)(outx, outy, 0) = (float) entropy;

				}
			}
			//  if (verbose) { percent.write_ok(); }

			delete[] histogram;
			return result;
			
			}
		
		//generalisation de la structure entropique
		template < class T >
		void entropicStructureImage(BufferImage< T > const & in,
									BufferImage < T > & out){
			//bool verbose=false;
			BufferImage< T > imgin_gris(in.numCols(), in.numLines(), 1);
			BufferImage<float>* imgent;
			
			if (in.numBands() < 3) {
				return;
			}
			
			//gestion des image multi-canaux >3 (comme les images landsat avec 8 ou 9 canaux)
			if (in.numBands() >= 3) {
				// creer une image tmp de 3 canaux.
				BufferImage< T > imgtmp(static_cast<int>(in.numCols()), static_cast<int>(in.numLines()), 3);
				
				// copier les 3 canaux dans cette image tmp.
				for (int c = 0; c < 3; c++) {
					in.read(0, 0, c, in.numCols(), in.numLines(), c, 1, imgtmp.getPtr(), imgtmp.typePixel(),
							imgtmp.getPixelSpace(),	imgtmp.getLineSpace(), c);
				}
				out = imgtmp;
			}
			
			//calcul de l'image d'intensite
			size_t i, j;
			for (i = 0; i < in.numCols(); i++) {
				for (j = 0; j < in.numLines(); j++) {
					imgin_gris(i, j, 0) = (T) ((double) (in(i, j, 0) + in(i, j, 1) + in(i, j, 2)) / (3.));
				}
			}
			
			const int nbins = 16;
			const int ds = 8;
			imgent = ign::image::entropicStructureOneChannel(imgin_gris, ds, nbins, false);
			
			// copie du compte d'entropie
			//imgent[0] prend des valeurs de 0.0 � log2(N)
			//imgent[1] prend des valeurs de 0.0 � 1.0
			size_t y, x;
			for (y = 0; y < in.numLines(); ++y)
				for (x = 0; x < in.numCols(); ++x) {
					float ent_c = (*imgent)(x / ds, y / ds, 1) * 255.0f;
					if (ent_c < 0.0)
						ent_c = 0.0;
					if (ent_c > 255.0)
						ent_c = 255.0;
					out(x, y, 2) = (T) (ent_c);
				}
			delete imgent;
			
		}

	}
}

#endif //__ENTROPY_H__
