/* 

 lGl : A C++ library needed by the multiscale image segmentation library SxS
 � 2004 Laurent Guigues (laurent.guigues@ign.fr)

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
/*! \file 
 \brief General definitions for image management (ImageSite, ImageDomain, Image, ImageIn, ImageOut, ImageInOut)
 */
#ifndef __lglImage_h__
#define __lglImage_h__
//===========================================================
// namespace	: lgl
// classe		: Image
// Abstract image definition : ancestor of all images (file/ram/mosaics..., 2D/3D...)
//===========================================================
// author		: Laurent Guigues
// history		: 
//	29/07/04	: creation
//===========================================================
#include <ostream>
#include <string>

#include "lgl/lglGeneral.h"
#include "lgl/lglPoint4D.h"
#include "lgl/lglVector.h"
#include "lgl/lglCollection.h"


namespace lgl {

	//==========================================================================================
	/// Structure representing the position of a value in a 2D or 3D image.
	/// If s is an ImageSite then :
	/// s.dimension() is the dimension of the image + 1
	/// - s(0) : is the channel
	/// - s(1) : the x coordinate
	/// - s(2) : the y coordinate if the image is 2D
	/// - s(3) : the z coordinate if the image is 3D
	typedef IPoint4D ImageSite;
	//==========================================================================================

	//==========================================================================================
	/// Abstract definition of an image domain, that is a collection of ImageSite.
	/// Ancestor of ImageBloc, ImageNeighborhood.
	typedef Collection<ImageSite> ImageDomain;
	//==========================================================================================

	//==========================================================================================
	/// The collection of the ImageSite contained into a rectangular zone of an image.
	/// The iteration is made in "video order" i.e. x then y, then z in 3D.
	/// NB : The channels are not iterated, only the sites.
	class ImageBloc: public ImageDomain {
	public:
		/// Ctor with the bounds of the Bloc.
		/// end is excluded from the domain.
		/// If for any dimension d, end(d)>beg(d) then the bounds are reordered
		ImageBloc(const ImageSite& beg = ImageSite(0), const ImageSite& end = ImageSite(0)) {
			setBounds(beg, end);
		}

		/// Sets the bounds of the bloc
		void setBounds(const ImageSite& beg, const ImageSite& end) {
			//			wxLogMessage("ImageBloc::setBounds((%d,%d,%d),(%d,%d,%d))",beg(1),beg(2),beg(3),end(1),end(2),end(3));
			if (beg.dimension() != end.dimension()) {
				lglERROR(std::string("ImageBloc::setBounds : points of different dimensions"));
			}
			m_beg = beg;
			m_end = end;
			for (int i = 1; i < m_beg.dimension(); i++) {
				if (m_beg(i) > m_end(i)) {
					int t = m_beg(i);
					m_beg(i) = m_end(i);
					m_end(i) = t;
				}
			}
			//
			m_not_empty = m_beg < m_end;
			//lglLOG("m_ne="<<m_not_empty);
			start();
		}

		inline ImageSite size() const {
			return m_end - m_beg;
		}

		/// Returns the starting site of the bloc
		inline const ImageSite& starting() const {
			return m_beg;
		}
		/// Returns the stopping site of the bloc (past the end)
		inline const ImageSite& stopping() const {
			return m_end;
		}

		/// Restarts the iteration to  the first object
		inline void start() {
			m_cur = m_beg;
			m_end_reached = !m_not_empty;
			//lglLOG("beg::m_er="<<m_end_reached);
		}
		/// Returns true if the iteration is past the end
		inline BOOL stop() {
			return m_end_reached;
		}
		/// Returns the current object
		inline const ImageSite& operator *() const {
			return m_cur;
		}
		/// Steps to the next object of the collection
		inline void operator++() {
			m_cur(1)++;if
(						m_cur(1)>=m_end(1)) {
				m_cur(1) = m_beg(1);
				m_cur(2)++;
				if ( m_cur(2)>=m_end(2) ) {
					m_cur(2) = m_beg(2);
					m_cur(3)++;
					if ( m_cur(3)>=m_end(3) ) {
						m_end_reached = true;
					}
				}
			}
		}

	protected:
		ImageSite m_beg, m_end, m_cur;
		BOOL m_not_empty,m_end_reached;
	};
	//==========================================================================================

	inline std::ostream& operator <<(std::ostream& s, const ImageBloc& p) {
		s << "[" << p.starting() << "," << p.stopping() << "]";
		return s;
	}

	//==========================================================================================
	/// Abstract image definition.
	class Image {
	public:
		///@Name Size, #dimensions, #channels
		///@{

		/// Returns the size of the Image.

		///  - size()(0) : number of channels
		///  - size()(1) : number of points in x coordinate
		///  - size()(2) : number of points in y coordinate
		///  - size()(3) : number of points in z coordinate
		/// e.g. :
		///  * a 256x256 3 channels image has size (3,256,256,0) which is an ImageSite of dimension 3.
		///  * a 100x128x128 1 channel image has size (1,128,128,100) ...

		virtual ~Image() {
		}

		virtual ImageSite size() const = 0;
		/// Returns the size of the Image in the ith dimension
		virtual I32 size(I32 i) const =0;
		/// Returns the dimension of the Image
		virtual I32 dimension() const =0;
		/// Returns the number of channels the Image
		virtual I32 channels() const =0;
		/// Returns the domain of the image
		virtual ImageBloc domain() const =0;
		/// Returns the total number of sites in the image
		inline long numberOfSites() const {
			long n = size(1);
			for (int i = 2; (i < 4) && (size(i) > 0); ++i)
				n *= size(i);
			return n;
		}
		/// Returns the total number of values in the image (=numberOfSites()*channels())
		inline long numberOfValues() const {
			return numberOfSites() * size(0);
		}
		///@}

		///@Name Type
		///@{

		/// returns the code of the type of the Image pixels (@see BasicType)
		virtual TypeCode type() const = 0;

		///@}

		/// returns true if the point is inside the image domain
		virtual BOOL is_inside(const ImageSite& p) const {
			return false;
		}
	};
	//==========================================================================================
	// EO class Image
	//==========================================================================================

	//==========================================================================================
	/// Abstract input image definition.
	class ImageIn: public virtual Image, public Collection<double> {
	public:
		///@name Read access to image values
		///@{
		/// Returns the value of the image at point p as a 32 bits float
		virtual F32 get(const ImageSite& p) const = 0;
		///@}

	};
	//==========================================================================================
	// EO class ImageIn
	//==========================================================================================

	//==========================================================================================
	/// Abstract output image definition.
	class ImageOut: public virtual Image {
	public:
		/// Resizes the image. returns TRUE if ok.
		virtual BOOL resize(const ImageSite& p) {
			return FALSE;
		}

		///@name Write access to image values
		///@{
		/// Sets the value of the image at point p
		virtual void set(const ImageSite& p, F32 v) = 0;
		/// Sets all the values of the image to v
		virtual void mset(F32 v) = 0;
		///@}

		virtual void crop(const ImageIn& i, ImageBloc& b) {
			/* MUST ADD A TEST FOR VALID CROP : AT LEAST AN ASSERTION */
			//lglLOG("Crop : "<<b<<" sz="<<b.size()<<std::endl);
			resize(b.size());
			for (b.start(); !b.stop(); ++b)
				this->set(*b - b.starting(), i.get(*b));
		}
	};
	//==========================================================================================
	// EO class ImageOut
	//==========================================================================================

	//==========================================================================================
	/// Abstract input/output image definition.
	class ImageInOut: public virtual ImageIn, public virtual ImageOut {
	};
//==========================================================================================
// EO class ImageInOut
//==========================================================================================
}
//==========================================================================================
// EO namespace lgl
//==========================================================================================

//==========================================================================================
// EOF
//==========================================================================================
#endif

