/* 
lGl : A C++ library needed by the multiscale image segmentation library SxS
 © 2004 Laurent Guigues (laurent.guigues@ign.fr)

 This program is free software; you can use, modify and/or redistribute it
 under the terms of the CeCILL license, which is a French law-Compatible and
 GNU GPL-Compatible free software license published by CEA, CNRS and INRIA.

 As a counterpart to the access to the source code and rights to copy,
 modify and redistribute granted by the license, users are provided only
 with a limited warranty and the software's author, the holder of the
 economic rights, and the successive licensors have only limited liability.
 See the CeCILL license for more details.

 You should have received a copy of the CeCILL license along with this software
 (files CeCILL_LICENCE_V1-fr.txt (french) and CeCILL_LICENSE_V1-en.txt (english)).
 If not visit http://www.cecill.info.
 The fact that you are presently reading this means that you have had
 knowledge of the CeCILL license and that you accept its terms.

 */
/**	\file	
 \brief Image partitioning based on the generalized Voronoi partitioning approach of Pablo Arbelaez and Laurent Cohen (Vector-Valued Image Segmentation by Generalized Voronoi Tesselations, proc. RFIA 2004).
 **/
#ifndef __lglVoronoiImageTesselation__h__
#define __lglVoronoiImageTesselation__h__
//===========================================================
// namespace	: lgl
// class		: - 
// methods		: VoronoiImagePartition
/// Image partitioning based on the generalized Voronoi partitioning approach 
/// of Pablo Arbelaez and Laurent Cohen (Vector-Valued Image Segmentation by Generalized Voronoi Tesselations, proc. RFIA 2004). 
//===========================================================
// author		: Laurent Guigues
// history		: 
//	09/09/04	: creation
//===========================================================
#include "lgl/lglFunction.h"
#include "lgl/lglImageRam.h"
#include "lgl/lglImageManip.h"
#include "lgl/lglNeighborhood.h"
#include "lgl/lglImageBasicAlgorithms.h"

namespace lgl {

	//====================================================================
	/// \brief Image partitioning based on the generalized Voronoi partitioning approach
	/// of Pablo Arbelaez and Laurent Cohen (Vector-Valued Image Segmentation by Generalized Voronoi Tesselations, in proc. RFIA 2004).
	///
	/// \param seeds is a label image providing the seeds of the Voronoi cells. Seeds values must be positive or null. Each non zero value is interpreted as a seed.
	/// \param neigh gives the type of neighborhood used for Voronoi cells growing.
	/// \param distance must be a BinaryFunction<ImageSite,ImageSite,double> which gives the distance between two ImageSite
	template<class D>
	void VoronoiImageTesselation(ImageRamT<int>& seeds, Neighborhood& neigh, D& distance);
	//====================================================================

	//====================================================================
	/// A distance function between two image sites providing the difference between the values of a mono-channel image.
	template<class T>
	class MonoChannelImageVariation: public BinaryFunction<ImageSite, ImageSite, double> {
	public:
		MonoChannelImageVariation(const ImageRamT<T>& i) :
				m_i(&i) {
		}

		inline double operator()(const ImageSite& m, const ImageSite& n) const {
			return ABS ( m_i->get(m) - m_i->get(n) );
		}
	protected:
		const ImageRamT<T>* m_i;
	};
	//====================================================================

	//====================================================================
	/// A distance function between two image sites providing the euclidian distance between the values of a multi-channel image.
	template<class T>
	class MultiChannelImageVariation: public BinaryFunction<ImageSite, ImageSite, double> {
	public:
		MultiChannelImageVariation(const ImageRamT<T>& i) :
				m_i(&i) {
			m_c = m_i->channels();
		}

		inline double operator()(const ImageSite& m, const ImageSite& n) const {
			ImageSite p(m), q(n);
			double d = 0;
			for (p(0) = 0, q(0) = 0; p(0) < m_c; ++p(0), ++q(0)) {
				double dc = m_i->get(p) - m_i->get(q);
				d += dc * dc;
			}
			return sqrt(d);
		}
	protected:
		const ImageRamT<T>* m_i;
		int m_c;
	};
	//====================================================================

	//====================================================================
	/// \brief Image partitioning based on the generalized Voronoi partitioning approach
	/// of Pablo Arbelaez and Laurent Cohen (Vector-Valued Image Segmentation by Generalized Voronoi Tesselations, in Proc. RFIA 2004).
	///
	/// Runs VoronoiImageTesselation using the local optima of the image as seeds
	/// and ImageVariation as distance.
	/// \param image input image
	/// \param neigh type of neighborhood used both for local optima determination and Voronoi cells growing
	/// \output labels output label image
	/// \output number number of zones in the output partition
	template<class T>
	void LocalOptimaVoronoiTesselation(const ImageRamT<T>& image, Neighborhood& neigh, ImageRamT<int>& labels, int& number);
//====================================================================

/// Template codes inclusion
#include "lgl/lglVoronoiImageTesselation_code.h"

}
//===========================================================
// EO namespace lgl
//===========================================================

//===========================================================
// EOF
//===========================================================
#endif

