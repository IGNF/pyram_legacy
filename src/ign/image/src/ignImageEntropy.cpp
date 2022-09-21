/* -*-mode: C++; coding: utf-8;-*- */

/** Salade --- Roger Trias i Sanz  2002-2005
 Systeme Automatique de Localisation Agraire et de Detection d'Equi-cultures
 */

//-----------------
//#define _USE_MATH_DEFINES
//#undef M_PI
//#include <math.h>

#include <string>
#include <iostream>
#include <cassert>

#include <ignStringTools.h>
#include <ignImageDeriche.h>
#include <ignImageEntropy.h>


#define log2(a) log10((double)a)/log10((double)2)

using namespace ign::image;
using namespace std;
namespace ign {

	namespace image {


		/// angle d'un vecteur (x, y), mais renvoie 0 pour des valeurs faibles de x et y
		double atan2_thr(double y, double x) {//definition QP020415
			if (fabs(x) < 1e-3 && fabs(y) < 1e-3)
				return 0.0;
			else
				return atan2(y, x);
		}

		// Calcul de la structuration entropique
		BufferImage<float>* entropicStructureOneChannel(const BufferImage<unsigned char> & image, 
			int ds, 
			int nbins,
			float minmod, 
			float sigmasq, 
			float alpha,
			bool verbose) {

			ImageSize src_size = image.size(1);
			ImageSize out_size((int) (ceil(1.0 * src_size.first / ds)), (int) (ceil(1.0 * src_size.second / ds)));
			BufferImage<float>* result = new BufferImage<float>(static_cast<int>(out_size.first), static_cast<int>(out_size.second), 2, 0.0);


			//qp 110515
			BufferImage<unsigned char> imgin(static_cast<int>(image.numCols()), static_cast<int>(image.numLines()), 1);
			std::cout<<"transformation en char commencee"<<std::endl;

			double *histogram = new double[nbins];

			// calcul du module et argument du gradient
			if (verbose)
				std::cout << "preparing... " << std::flush;
			BufferImage<float> gradientX, gradientY, gradientM, gradientA;
			gradientX.initialize(static_cast<int>(src_size.first), static_cast<int>(src_size.second), 1);
			gradientY.initialize(static_cast<int>(src_size.first), static_cast<int>(src_size.second), 1);
			gradientM.initialize(static_cast<int>(src_size.first), static_cast<int>(src_size.second), 1);
			gradientA.initialize(static_cast<int>(src_size.first), static_cast<int>(src_size.second), 1);

			Deriche(image, gradientX, gradientY, alpha);
			double ming = 1000.0, maxg = 0.0, aveg = 0.0;
			for (int y = 0; y < static_cast<int>(src_size.second); ++y)
				for (int x = 0; x < static_cast<int>(src_size.first); ++x) {
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
			int gaussian_size = (int) (ceil(sqrt(-2 * sigmasq * log(0.05)))); // coupe à 5%
			BufferImage<float> gaussian(gaussian_size + 1, gaussian_size + 1, 1);
			for (int y = 0; y < gaussian_size + 1; ++y)
				for (int x = 0; x < gaussian_size + 1; ++x)
					gaussian(x, y, 0) = (float) exp(-0.5 * ((x * x) + (y * y)) / sigmasq);

			// et on scanne l'image
			if (verbose) std::cout << "scanning " << std::flush;
			double angle_factor = nbins / (M_PI * 2.0);
			int outy(0), outx(0);
			for (int iny = ds / 2, outy = 0; outy < out_size.second; iny += ds, ++outy) {
				for (int inx = ds / 2, outx = 0; outx < out_size.first; inx += ds, ++outx) {
					double numpoints = 0.0, numvalid = 0.0;
					for (int p = 0; p < nbins; ++p)
						histogram[p] = 0;
					for (int sy = -gaussian_size; sy <= gaussian_size; ++sy)
						for (int sx = -gaussian_size; sx <= gaussian_size; ++sx) {
							int cx = static_cast<int>(inx) + sx;
							int cy = static_cast<int>(iny) + sy;
							if (cx >= src_size.first || /*cy < 0 ||*/ cy >= src_size.second)
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
					(*result)(outx, outy, 1) = (float) (numvalid / numpoints);//essai pour 1 seul canal 060515

					// entropie de l'histogramme
					double entropy = 0;//, p;
					if (numvalid == 0.0) {
						(*result)(outx, outy, 0) = 0.0;
						continue;
					}
					for (int sa = 0; sa < nbins; ++sa) {
						double p = histogram[sa] / numvalid;
						if (p > 0.00001)
							entropy -= p * log2(p);
					}
					(*result)(outx, outy, 0) = (float) entropy;
					// on pourrait faire entropy/=log2(nbins) pour la normaliser, la faire revenir à 0.0 .. 1.0
					// mais pas pour le moment
				}
			}
			std::cout<<"on est dans l'entropie (1)"<<std::endl;
			delete[] histogram;
			return result;
		}



		//qp re-test
			BufferImage<float>* entropicStructureOneChannel3(const BufferImage<unsigned char> & image, 
			int ds, 
			int nbins,
			float minmod, 
			float sigmasq, 
			float alpha,
			bool verbose) {

			ImageSize src_size = image.size(1);
			ImageSize out_size((int) (ceil(1.0 * src_size.first / ds)), (int) (ceil(1.0 * src_size.second / ds)));
			BufferImage<float>* result = new BufferImage<float>(static_cast<int>(out_size.first), static_cast<int>(out_size.second), 2, 0.0);


			//qp 110515
			//declaration de l'image de sorties
			BufferImage<unsigned char> imgin0(static_cast<int>(image.numCols()), static_cast<int>(image.numLines()), 1);
			BufferImage<unsigned char> imgin1(static_cast<int>(image.numCols()), static_cast<int>(image.numLines()), 1);
			std::cout<<"transformation en cher commencee"<<std::endl;

			double *histogram = new double[nbins];

			// calcul du module et argument du gradient
			if (verbose)
				std::cout << "preparing... " << std::flush;
			BufferImage<float> gradientX, gradientY, gradientM, gradientA;
			gradientX.initialize(static_cast<int>(src_size.first), static_cast<int>(src_size.second), 1);
			gradientY.initialize(static_cast<int>(src_size.first), static_cast<int>(src_size.second), 1);
			gradientM.initialize(static_cast<int>(src_size.first), static_cast<int>(src_size.second), 1);
			gradientA.initialize(static_cast<int>(src_size.first), static_cast<int>(src_size.second), 1);

			Deriche(image, gradientX, gradientY, alpha);
			double ming = 1000.0, maxg = 0.0, aveg = 0.0;
			for (int y = 0; y < static_cast<int>(src_size.second); ++y)
				for (int x = 0; x < static_cast<int>(src_size.first); ++x) {
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
			int gaussian_size = (int) (ceil(sqrt(-2 * sigmasq * log(0.05)))); // coupe à 5%
			BufferImage<float> gaussian(gaussian_size + 1, gaussian_size + 1, 1);
			for (int y = 0; y < gaussian_size + 1; ++y)
				for (int x = 0; x < gaussian_size + 1; ++x)
					gaussian(x, y, 0) = (float) exp(-0.5 * ((x * x) + (y * y)) / sigmasq);

			// et on scanne l'image
			if (verbose)
				std::cout << "scanning " << std::flush;
			double angle_factor = nbins / (M_PI * 2.0);
			//  Percent percent(out_size.y-1);
			int inx, iny, outx, outy;
			for (iny = ds / 2, outy = 0; outy < static_cast<int>(out_size.second); iny += ds, ++outy) {
				//    if (verbose) percent=outy;
				for (inx = ds / 2, outx = 0; outx < static_cast<int>(out_size.first); inx += ds, ++outx) {
					double numpoints = 0.0, numvalid = 0.0;
					for (int p = 0; p < nbins; ++p)
						histogram[p] = 0;
					for (int sy = -gaussian_size; sy <= gaussian_size; ++sy)
						for (int sx = -gaussian_size; sx <= gaussian_size; ++sx) {
							int cx = static_cast<int>(inx) + sx;
							int cy = static_cast<int>(iny) + sy;
							if (/*cx < 0 ||*/ cx >= src_size.first || /*cy < 0 ||*/ cy >= src_size.second)
								continue;
							numpoints += gaussian(abs(sx), abs(sy), 0);
							if (gradientM(cx, cy, 0) < minmod)
								continue;
							numvalid += gaussian(abs(sx), abs(sy), 0);
							int bin = static_cast<int>(round(gradientA(cx, cy, 0) * angle_factor));
							if (bin >= nbins)
								bin = nbins - 1;
							histogram[bin] += gaussian(abs(sx), abs(sy), 0);
						}

					// relation de points valides
					assert(numpoints > 0.0);
					(*result)(outx, outy, 1) = (float) (numvalid / numpoints);//essai pour 1 seul canal 060515

					// entropie de l'histogramme
					double entropy = 0;//, p;
					if (numvalid == 0.0) {
						(*result)(outx, outy, 0) = 0.0;
						continue;
					}
					for (int sa = 0; sa < nbins; ++sa) {
						double p = histogram[sa] / numvalid;
						if (p > 0.00001)
							entropy -= p * log2(p);
					}
					(*result)(outx, outy, 0) = (float) entropy;
					// on pourrait faire entropy/=log2(nbins) pour la normaliser, la faire revenir à 0.0 .. 1.0
					// mais pas pour le moment

					/*rate//qp 110515
					if (entropy < 0.0)
						entropy = 0.0;
					if (entropy > 255.0)
						entropy = 255.0;
					imgin0(outx, outy, 0) = (unsigned char) (entropy);
					//fin qp110515*/

					/*rate//qp 110515
					float entropy1 = (entropy/ds) * 255.0f;
					if (entropy1 < 0.0)
						entropy1 = 0.0;
					if (entropy1 > 255.0)
						entropy1 = 255.0;

					// on ecrit le result
					imgin1(outx, outy, 0) = (unsigned char) (entropy1);
					//fin 110515*/
				}
			}
			
			for (int y = 0; y < static_cast<int>(result->numLines()); ++y) {
				for (int x = 0; x < static_cast<int>(result->numCols()); ++x) {
					float entropy0 = (*result)(x / ds, y / ds, 0) * 255.0f;

					if (entropy0 < 0.0)
						entropy0 = 0.0;
					if (entropy0 > 255.0)
						entropy0 = 255.0;

					// on ecrit le result
					imgin0(x, y, 0) = static_cast<unsigned char>(entropy0);


					float entropy1 = (*result)(x / ds, y / ds, 1) * 255.0f;

					if (entropy1 < 0.0)
						entropy1 = 0.0;
					if (entropy1 > 255.0)
						entropy1 = 255.0;

					// on ecrit le result
					imgin1(x, y, 0) = (unsigned char) (entropy1);
				}
			}
			//qp110515
			std::cout<<"on est dans l'entropie (2)"<<std::endl;
			delete[] histogram;
			return result;
		}






		///fin 130515  ajout *
		BufferImage<unsigned char> entropicStructureOneChannel3a(const BufferImage<unsigned char> & image, 
			int ds, 
			int nbins,
			float minmod, 
			float sigmasq, 
			float alpha,
			bool verbose,
			bool bGradX,
			bool bGradY,
			std::string nom ) {

			ImageSize src_size = image.size(1);
			ImageSize out_size((int) (ceil(1.0 * src_size.first / ds)), (int) (ceil(1.0 * src_size.second / ds)));
			BufferImage<float>* result = new BufferImage<float>(static_cast<int>(out_size.first), static_cast<int>(out_size.second), 2, 0.0);

			//du a l'ajout de *
			BufferImage<unsigned char> imgin1(static_cast<int>(image.numCols()), static_cast<int>(image.numLines()), 1);

			double *histogram = new double[nbins];

			// calcul du module et argument du gradient
			if (verbose)
				std::cout << "preparing... " << std::flush;
			BufferImage<float> gradientX, gradientY, gradientM, gradientA;// ici gradientX, gradientY
			gradientX.initialize(static_cast<int>(src_size.first), static_cast<int>(src_size.second), 1);
			gradientY.initialize(static_cast<int>(src_size.first), static_cast<int>(src_size.second), 1);
			gradientM.initialize(static_cast<int>(src_size.first), static_cast<int>(src_size.second), 1);
			gradientA.initialize(static_cast<int>(src_size.first), static_cast<int>(src_size.second), 1);

			Deriche(image, gradientX, gradientY, alpha);
			if(bGradX)gradientX.save(nom+".tif");
			ign::tools::StringManip::SubstituteSubString(nom,"-gx_","-gy_");
			
			if(bGradY)gradientY.save(nom+".tif");
			double ming = 1000.0, maxg = 0.0, aveg = 0.0;
			for (int y = 0; y < static_cast<int>(src_size.second); ++y)
				for (int x = 0; x < static_cast<int>(src_size.first); ++x) {
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
			int gaussian_size = (int) (ceil(sqrt(-2 * sigmasq * log(0.05)))); // coupe à 5%
			//std::cout<<"avant gaussien fichier:"<<__FILE__<<" ligne: "__LINE__;
			BufferImage<float> gaussian(gaussian_size + 1, gaussian_size + 1, 1);
			//std::cout<<"apres gaussien  fichier:"<<__FILE__<<" ligne: "__LINE__;
			for (int y = 0; y < gaussian_size + 1; ++y)
				for (int x = 0; x < gaussian_size + 1; ++x)
					gaussian(x, y, 0) = (float) exp(-0.5 * ((x * x) + (y * y)) / sigmasq);

			// et on scanne l'image
			if (verbose)
				std::cout << "scanning " << std::flush;
			double angle_factor = nbins / (M_PI * 2.0);
			//  Percent percent(out_size.y-1);
			int inx, iny, outx, outy;
			for (iny = ds / 2, outy = 0; outy < static_cast<int>(out_size.second); iny += ds, ++outy) {
				//    if (verbose) percent=outy;
				for (inx = ds / 2, outx = 0; outx < static_cast<int>(out_size.first); inx += ds, ++outx) {
					double numpoints = 0.0, numvalid = 0.0;
					for (int p = 0; p < nbins; ++p)
						histogram[p] = 0;
					for (int sy = -gaussian_size; sy <= gaussian_size; ++sy)
						for (int sx = -gaussian_size; sx <= gaussian_size; ++sx) {
							int cx = inx + sx;
							int cy = iny + sy;
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
					double entropy = 0;//, p;
					if (numvalid == 0.0) {
						(*result)(outx, outy, 0) = 0.0;
						continue;
					}
					for (int sa = 0; sa < nbins; ++sa) {
						double p = histogram[sa] / numvalid;
						if (p > 0.00001)
							entropy -= p * log2(p);
					}
					(*result)(outx, outy, 0) = (float) entropy;
				}
			}
			int y, x;
			for (y = 0; y < static_cast<int>(result->numLines()); ++y) {
				for (x = 0; x < static_cast<int>(result->numCols()); ++x) {
					float entropy0 = (*result)(x / ds, y / ds, 0) * 255.0f;

					if (entropy0 < 0.0)
						entropy0 = 0.0;
					if (entropy0 > 255.0)
						entropy0 = 255.0;

					float entropy1 = (*result)(x / ds, y / ds, 1) * 255.0f;

					if (entropy1 < 0.0)
						entropy1 = 0.0;
					if (entropy1 > 255.0)
						entropy1 = 255.0;

					// on ecrit le result
					imgin1(x, y, 0) = (unsigned char) (entropy1);
				}
			}
			std::cout<<"on est dans l'entropie (3)"<<std::endl;
			delete result;
			delete[] histogram;
			return imgin1;
		}

		///
		///
		///
		// Calcul de la structuration entropique
		BufferImage<float>* entropicStructureOneChannel2(const BufferImage<unsigned char> & image, 
			int ds, 
			int nbins,
			float minmod, 
			float sigmasq, 
			float alpha,
			bool verbose,
			bool etudeMonocanal,
			int numeroBlocLigne,
			bool bGx,
			bool bGy
			) {

			ImageSize src_size = image.size(1);
			ImageSize out_size((int) (ceil(1.0 * src_size.first / ds)), (int) (ceil(1.0 * src_size.second / ds)));
			BufferImage<float>* result = new BufferImage<float>(static_cast<int>(out_size.first), static_cast<int>(out_size.second), 1, 0.0);

			double *histogram = new double[nbins];

			// calcul du module et argument du gradient
			if (verbose)
				std::cout << "preparing... " << std::flush;
			BufferImage<float> gradientX, gradientY, gradientM, gradientA;
			gradientX.initialize(static_cast<int>(src_size.first), static_cast<int>(src_size.second), 1);
			gradientY.initialize(static_cast<int>(src_size.first), static_cast<int>(src_size.second), 1);
			gradientM.initialize(static_cast<int>(src_size.first), static_cast<int>(src_size.second), 1);
			gradientA.initialize(static_cast<int>(src_size.first), static_cast<int>(src_size.second), 1);

			Deriche(image, gradientX, gradientY, alpha);

			//nx
			if(bGx){
				std::string NomGradX="gradx_"+boost::lexical_cast<std::string>(numeroBlocLigne)+".tif";
				gradientX.save(NomGradX);
			}
			if(bGy){
				std::string NomGradY="grady_"+boost::lexical_cast<std::string>(numeroBlocLigne)+".tif";
				gradientY.save(NomGradY);
			}
			//fin nx

			double ming = 1000.0, maxg = 0.0, aveg = 0.0;
			for (int y = 0; y < static_cast<int>(src_size.second); ++y)
				for (int x = 0; x < static_cast<int>(src_size.first); ++x) {
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
			int gaussian_size = (int) (ceil(sqrt(-2 * sigmasq * log(0.05)))); // coupe à 5%
			BufferImage<float> gaussian(gaussian_size + 1, gaussian_size + 1, 1);
			for (int y = 0; y < gaussian_size + 1; ++y)
				for (int x = 0; x < gaussian_size + 1; ++x)
					gaussian(x, y, 0) = (float) exp(-0.5 * ((x * x) + (y * y)) / sigmasq);

			// et on scanne l'image
			if (verbose)
				std::cout << "scanning " << std::flush;
			double angle_factor = nbins / (M_PI * 2.0);
			//  Percent percent(out_size.y-1);
			int inx, iny, outx, outy;
			for (iny = ds / 2, outy = 0; outy < static_cast<int>(out_size.second); iny += ds, ++outy) {
				for (inx = ds / 2, outx = 0; outx < static_cast<int>(out_size.first); inx += ds, ++outx) {
					double numpoints = 0.0, numvalid = 0.0;
					for (int p = 0; p < nbins; ++p)
						histogram[p] = 0;
					for (int sy = -gaussian_size; sy <= gaussian_size; ++sy)
						for (int sx = -gaussian_size; sx <= gaussian_size; ++sx) {
							int cx = inx + sx;
							int cy = iny + sy;
							if ( cx >= src_size.first || cy >= src_size.second)
								continue;
							numpoints += gaussian(abs(sx), abs(sy), 0);
							if (gradientM(cx, cy, 0) < minmod)
								continue;
							numvalid += gaussian(abs(sx), abs(sy), 0);
							int bin = static_cast<int>(round(gradientA(cx, cy, 0) * angle_factor));
							if (bin >= nbins)
								bin = nbins - 1;
							histogram[bin] += gaussian(abs(sx), abs(sy), 0);
						}

					// relation de points valides
					assert(numpoints > 0.0);
					//(*result)(outx, outy, 1) = (float) (numvalid / numpoints);//essai pour 1 seul canal 060515

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

					// on pourrait faire entropy/=log2(nbins) pour la normaliser, la faire revenir à 0.0 .. 1.0
					// mais pas pour le moment


				}
			}
			
			
			//BufferImage<unsigned char> imgin;//rate
			//en cours de teste 060515 14:40
			BufferImage<unsigned char> imgin(static_cast<int>(image.numCols()), static_cast<int>(image.numLines()), 1);
			std::cout<<"transformation en cher commencee"<<std::endl;
			//transformation en unsigned char				
			for (int y = 0; y < result->numLines(); ++y)
				for (int x = 0; x < result->numCols(); ++x) {
					float ent_c = (*result)(x / ds, y / ds, 0) * 255.0f;//peut être 0 au lieu de 1
					if (ent_c < 0.0)
						ent_c = 0.0;
					if (ent_c > 255.0)
						ent_c = 255.0;
					imgin(x, y, 0) = (unsigned char) (ent_c);//0 a la place de 2 
				}
				/*fin pas teste*/

			//  if (verbose) { percent.write_ok(); }
			std::cout<<"on est dans l'entropie (4)"<<std::endl;
			delete[] histogram;
			return result;
		}
		//fin qp060515

		///
		///
		///
		void entropicStructureImage(BufferImage<unsigned char> & imgin)
		{
		//	bool verbose=false;
			BufferImage<unsigned char> imgin_gris(static_cast<int>(imgin.numCols()), static_cast<int>(imgin.numLines()), 1);
			BufferImage<float>* imgent;

			if (imgin.numBands() < 3) {
				//si image ne comprend pas 3 bandes on sort
				return;
			}
			if (imgin.numBands() >= 3) {
				//transforme une image 3bande en image en niveau de gris.
				// creer une image tmp de 3 canaux.
				BufferImage<unsigned char> imgtmp(static_cast<int>(imgin.numCols()), static_cast<int>(imgin.numLines()), 3);

				// copier les 3 canaux dans cette image tmp.
				for (int c = 0; c < 3; c++) {
					imgin.read(0, 0, c, static_cast<int>(imgin.numCols()), static_cast<int>(imgin.numLines()), c, 1, imgtmp.getPtr(), imgtmp.typePixel(),
						imgtmp.getPixelSpace(),	imgtmp.getLineSpace(), c);
				}
				imgin = imgtmp;
			}

			//calcul de l'image d'intensite transforme niv gris
			int i, j;
			for (i = 0; i < static_cast<int>(imgin.numCols()); i++) {
				for (j = 0; j < static_cast<int>(imgin.numLines()); j++) {
					imgin_gris(i, j, 0) = (unsigned char) ((double) (imgin(i, j, 0) + imgin(i, j, 1) + imgin(i, j, 2)) / (3.));
				}
			}

			const int nbins = 16;
			const int ds = 8;
			imgent = ign::image::entropicStructureOneChannel(imgin_gris, ds, nbins, 4.0, 100, 1.0, false);

			// copie du compte d'entropie
			//imgent[0] prend des valeurs de 0.0 � log2(N)
			//imgent[1] prend des valeurs de 0.0 � 1.0
			for (int y = 0; y < static_cast<int>(imgin.numLines()); ++y)
				for (int x = 0; x < static_cast<int>(imgin.numCols()); ++x) {
					float ent_c = (*imgent)(x / ds, y / ds, 1) * 255.0f;
					if (ent_c < 0.0)
						ent_c = 0.0;
					if (ent_c > 255.0)
						ent_c = 255.0;
					imgin(x, y, 2) = (unsigned char) (ent_c);
				}
			delete imgent;
		}
		
		///
		///
		///
		//qp050515
		void entropicStructureImage2(BufferImage<unsigned char> & imgin, float alpha, int canal)
		{
			//bool verbose=false;
			BufferImage<unsigned char> imgin_gris(imgin.numCols(), imgin.numLines(), 1);
			BufferImage<float>* imgent;

			if (imgin.numBands() < 3 && alpha!=0.0) {
				//si image ne comprend pas 3 bandes on sort
				return;
			}
			else{
				//transforme une image 3bande en image en niveau de gris.
				// creer une image tmp de 3 canaux.
				BufferImage<unsigned char> imgtmp(static_cast<int>(imgin.numCols()), static_cast<int>(imgin.numLines()), 3);

				// copier les 3 canaux dans cette image tmp.
				for (int c = 0; c < imgin.numBands(); c++) {
					//recupere le canal que l'on souhaite etudier.
					imgin.read(0, 0, c, static_cast<int>(imgin.numCols()), static_cast<int>(imgin.numLines()), canal, 1, imgtmp.getPtr(), imgtmp.typePixel(),
						imgtmp.getPixelSpace(),	imgtmp.getLineSpace(), c);
				}
				imgin = imgtmp;
			}

			//calcul de l'image d'intensite transforme niv gris
			int i, j;
			for (i = 0; i < static_cast<int>(imgin.numCols()); i++) {
				for (j = 0; j < static_cast<int>(imgin.numLines()); j++) {
					imgin_gris(i, j, 0) = (unsigned char) ((double) imgin(i, j, 0) );
				}
			}

			const int nbins = 16;
			const int ds = 8;
			imgent = ign::image::entropicStructureOneChannel(imgin_gris, ds, nbins, 4.0, 100, alpha, false);
			//pas tester mais surement inutile imgent = ign::image::entropicStructureOneChannel2(imgin_gris, ds, nbins, 4.0, 100, alpha, false,false,0);
			
			std::cout<<"on est dans la fonction entropicStructureImage2 qui ecrasse l'image d'entree"<<std::endl;
			
			// copie du compte d'entropie
			//imgent[0] prend des valeurs de 0.0 � log2(N)
			//imgent[1] prend des valeurs de 0.0 � 1.0
			int y, x;
			for (y = 0; y < static_cast<int>(imgin.numLines()); ++y)
				for (x = 0; x < static_cast<int>(imgin.numCols()); ++x) {
					float ent_c = (*imgent)(x / ds, y / ds, 1) * 255.0f;
					if (ent_c < 0.0)
						ent_c = 0.0;
					if (ent_c > 255.0)
						ent_c = 255.0;
					imgin(x, y, 0) = (unsigned char) (ent_c);//0 a la place de 2 
				}
			delete imgent;
		}
		//fin

	}
}
