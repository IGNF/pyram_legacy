#ifndef __WATERSHED_HPP__
#define __WATERSHED_HPP__

#include <ignBufferImage.h>

namespace ign {
	namespace image {

		/// \brief Sursegmentation basee sur la ligne de partage des eaux : Bassins versants sur BufferImage<float>
		/// \param imageIn Image en entree, le gradient de l'image à segmenter, produit avec ign::image::algorithm::Deriche et ign::image::algorithm::ModuleDeriche
		/// \param imDirOut Image en sortie, ce sont les lignes de partage des eaux
		/// \param bassinsOut Resultat de l'algorithme : une image sursegmentee
		/// \param numBassinsOut Le nombre de bassins versants
		/// \param p 1/p est la precision de comparaison des flottants
		void Watershed(const ign::image::BufferImage<float> &imageIn, ign::image::BufferImage<unsigned char> &imDirOut,
				ign::image::BufferImage<int> &bassinsOut, int &numBassinsOut, float p = 10000);

	}
}

#endif //#ifndef __WATERSHED_HPP__
