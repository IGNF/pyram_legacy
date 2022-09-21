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
 \brief Images allocated in RAM (2 or 3 dimensions, any type, any nb of channels)
 */
#ifndef __lglImageRam_h__
#define __lglImageRam_h__
//===========================================================
// namespace	: lgl
// classes		: 
//		ImageRam
//		ImageRamT<T>
// Images allocated in RAM (2 or 3 dimensions, any type, any nb of channels)
//===========================================================
// author		: Laurent Guigues
// history		: 
//	29/07/04	: creation
//===========================================================
#include "lgl/lglImage.h"
#include "lgl/lglMachine.h"

namespace lgl {

	//==========================================================================================
	/// \brief Abstract definition of RAM images  (2/3D, any number of channels).
	class ImageRam: public virtual ImageInOut {
	public:
		typedef ImageRam* pImageRam;

		/// Allocates a RamImage of the same actual class than this
		virtual pImageRam procreate() const = 0;
		/// Allocates a RamImage of the same actual class than this and copies the content of this
		virtual pImageRam clone() const = 0;

		///@name 3D Images operations
		///@{

		/// Returns a slice of a 3D image.

		/// \param direction : its first non null coordinate gives the direction of the slice
		/// \param position : position of the slice, e.g. direction = (0,0,1,0) position = 15 gives the slice along the y=15 plane.
		/// \param out : output 2D image
		virtual void getSlice(const ImageSite& direction, int position, pImageRam& out) const = 0;
		/// Returns 3 slices of a 3D image along the planes : x=position(1), y=position(2) and z=position(3).
		virtual void getAllSlices(const ImageSite& position, pImageRam& outx, pImageRam& outy, pImageRam& outz) const = 0;
		///@}

		///@name IO Operations
		///@{

		/// raw read from a binary file.

		/// \param f : file
		/// \param swap : swap bytes (little/big endian)
		virtual BOOL rawRead(FILE* f, BOOL swap = FALSE) {
			return false;
		}

		/// raw write from a binary file.

		/// \param f : file
		virtual BOOL rawWrite(FILE* f) const {
			return false;
		}
		///@}
	};
	//==========================================================================================

	//==========================================================================================
	/// Pointer on an ImageRam
	typedef ImageRam* pImageRam;
	//==========================================================================================
	/// Pointer on a const ImageRam
	typedef const ImageRam* pcImageRam;
	//==========================================================================================

	//==========================================================================================
	/// \brief Iterator of ImageRamT
	template<class T, class RefT> class ImageRamT_iterator;
	//==========================================================================================

	//==========================================================================================
	/// \brief Concrete RAM image of type T (2/3D, any number of channels).
	///
	/// Handled as 4D images in which a site s is an ImageSite (Point<int>)
	/// whose coordinates represent :
	/// - s(0) : channel
	/// - s(1) : x (column for 2D images)
	/// - s(2) : y (line for 2D images)
	/// - s(3) : z for 3D images
	///
	/// Hence the green component of the (x,y) pixel of a 2D RGB image is located at ImageSite(1,x,y,0) (or simply ImageSite(1,x,y)).
	/// The size of an image is coded with the same convention.
	/// e.g. :
	/// - A 256x256 3 channels 2D image has size ImageSite(3,256,256) which is of dimension 3.
	/// - A 100x128x128 1 channel 3D image has size ImageSite(1,128,128,100) which is of dimension 4.
	template<class T>
	class ImageRamT: public virtual ImageRam {
	public:
		/// iterator
		typedef ImageRamT_iterator<T, T&> iterator;
		/// const_iterator
		typedef ImageRamT_iterator<T, const T&> const_iterator;

		///@name Construction and destruction
		///@{

		/// Default ctor
		ImageRamT() :
				m_size(), m_data(0), m_data_end(0) {
		}
		/// Ctor with size and initial value
		ImageRamT(const ImageSite& size) :
				m_size(), m_data(0), m_data_end(0) {
			build(size);
		}
		/// Ctor with size and initial value
		ImageRamT(const ImageSite& size, T v) :
				m_size(), m_data(0), m_data_end(0) {
			build(size, v);
		}
		/// Copy ctor
		ImageRamT(const ImageRamT<T>& src);
		/// Copy ctor from a general ImageRam
		ImageRamT(const ImageRam& src);
		/// Destructor
		~ImageRamT() {
			free();
		}
		/// Builds the Image (memory allocation).

		/// \return TRUE iff successful
		BOOL build(const ImageSite& size);
		/// Builds the Image (memory allocation) and initializes its values with v.

		/// \return TRUE iff successful
		BOOL build(const ImageSite& size, T v);
		/// Frees the Image (memory desallocation).
		/// \return TRUE iff successful
		BOOL free();

		/// Allocates a RamImage of the same actual class than this
		virtual pImageRam procreate() const {
			return new ImageRamT<T> ;
		}
		/// Allocates a RamImage of the same actual class than this and copies the content of this
		virtual pImageRam clone() const {
			return new ImageRamT<T>(*this);
		}

		///@}

		///@name Size, #dimensions, #channels, type
		///@{

		/// Returns the size of the Image.
		///  - size()(0) : number of channels
		///  - size()(1) : number of points in x coordinate
		///  - size()(2) : number of points in y coordinate
		///  - size()(3) : number of points in z coordinate
		/// The size().dimension()-1 gives the number of dimensions of the image
		inline ImageSite size() const {
			return m_size;
		}
		/// Returns the size of the Image in the ith dimension (0:number of channels, 1:x, etc.)
		inline I32 size(I32 i) const {
			return m_size(i);
		}
		/// Returns the number of dimensions of the Image
		inline I32 dimension() const {
			return m_size.dimension() - 1;
		}
		/// Returns the number of channels the Image
		inline I32 channels() const {
			return m_size(0);
		}

		/// returns the code of the type of the image pixels (@see BasicType)
		virtual TypeCode type() const {
			return BasicType::code((T) 0);
		}

		/// Returns the domain of the image
		inline ImageBloc domain() const {
			ImageSite beg(size());
			beg(0) = beg(1) = beg(2) = beg(3) = 0;
			return ImageBloc(beg, size());
		}

		/// Resizes the image. returns TRUE if ok.
		BOOL resize(const ImageSite& p) {
			return build(p);
		}
		///
		inline BOOL is_inside(const ImageSite& p) const {
			if (p.dimension() != size().dimension())
				return false;
			for (int i = 0; i < p.dimension(); ++i) {
				if (p(i) < 0 || p(i) >= size(i))
					return false;
			}
			return true;
		}

		//inline long numberOfSites() const;
		//inline long numberOfValues() const;

		///@}

		///@name Values access
		///@{

		/// Returns the value of the image at point p as a 32 bits float
		inline F32 get(const ImageSite& p) const {
			//lglASSERT(is_inside(p));
			return (F32)(m_data[m_offset(p)]);
		}
		/// Sets the value of the image at point p as a 32 bits float
		inline void set(const ImageSite& p, F32 v) {
			lglASSERT(is_inside(p));
			m_data[m_offset(p)] = (T) v;
		}
		/// Multi-set
		inline void mset(F32 val) {
			T v = (T) val;
			for (iterator i = begin(); i != end(); ++i)
				(*i) = v;
		}

		/// \name Methods inherited from lgl::Collection<double> for auto-iteration
		/// \{
		/// Restarts the iteration to the first value
		inline void start() {
			m_cur = m_data;
		}
		/// Returns true when the iteration is past the end
		inline BOOL stop() {
			return (m_cur >= m_data_end);
		}
		/// Returns the current value as a double
		inline const double& operator *() const {
			static double d = 0;
			d = (double) (*m_cur);
			return d;
		}
		/// Steps to the next value
		inline void operator++() {
			m_cur++;
		}
		/// \}

		/// Returns a reference on the value at site p
		T& operator()(const ImageSite& p) {
			return m_data[m_offset(p)];
		}
		/// Returns a const ref on the value at site p
		const T& operator()(const ImageSite& p) const {
			return m_data[m_offset(p)];
		}
		/// Returns a ref on the value at site p(c,x,y)
		T& operator()(I32 c, I32 x, I32 y) {
			return m_data[m_offset(c, x, y)];
		}
		/// Returns a const ref on the value at site p(c,x,y)
		const T& operator()(I32 c, I32 x, I32 y) const {
			return m_data[m_offset(c, x, y)];
		}
		/// Returns a ref on the value at site p(c,x,y,z)
		T& operator()(I32 c, I32 x, I32 y, I32 z) {
			return m_data[m_offset(c, x, y, z)];
		}
		/// Returns a const ref on the value at site p(c,x,y)
		const T& operator()(I32 c, I32 x, I32 y, I32 z) const {
			return m_data[m_offset(c, x, y, z)];
		}

		/// Returns a pointer on the data
		T* getData() {
			return m_data;
		}
		/// Returns a const pointer on the data
		const T* getData() const {
			return m_data;
		}

		///@}

		///@name Iteration
		///@{
		iterator begin() {
			return iterator(m_data);
		}
		iterator end() {
			return iterator(m_data_end);
		}
		const_iterator begin() const {
			return const_iterator(m_data);
		}
		const_iterator end() const {
			return const_iterator(m_data_end);
		}
		iterator begin(const ImageSite& p) {
			return iterator(m_data + m_offset(p));
		}
		iterator end(const ImageSite& p) {
			return iterator(m_data + m_offset(p) + 1);
		}
		const_iterator begin(const ImageSite& p) const {
			return const_iterator(m_data + m_offset(p));
		}
		const_iterator end(const ImageSite& p) const {
			return const_iterator(m_data + m_offset(p) + 1);
		}
		///@}

		///@name 3D Images operations
		///@{

		/// Returns a slice of a 3D image.

		/// \param direction : its first non null coordinate gives the direction of the slice
		/// \param position : position of the slice, e.g. direction = (0,0,1,0) position = 15 gives the slice along the y=15 plane.
		/// \param out : output 2D image
		void getSlice(const ImageSite& direction, int position, pImageRam& out) const;
		/// Returns 3 slices of a 3D image along the planes : x=position(1), y=position(2) and z=position(3).
		void getAllSlices(const ImageSite& position, pImageRam& outx, pImageRam& outy, pImageRam& outz) const;
		///@}

		///@name IO Operations
		///@{

		/// raw read from a binary file.

		/// \param f : file
		/// \param swap : swap bytes (little/big endian)
		BOOL rawRead(FILE* f, BOOL swap = FALSE);

		/// raw write from a binary file.

		/// \param f : file
		BOOL rawWrite(FILE* f) const;
		///@}

	protected:
		/// size
		ImageSite m_size;
		/// data
		T* m_data;
		/// memorizes the first address after data (past the end)
		T* m_data_end;
		/// Current position (for Collection auto-iterator)
		T* m_cur;
		/// Converts the image site p into a memory offset
		inline long m_offset(const ImageSite& p) const {
			return p(0) + m_size(0) * (p(1) + m_size(1) * (p(2) + m_size(2) * p(3)));
		}
		/// Converts the image site ImageSite(c,x,y) into a memory offset
		inline long m_offset(I32 c, I32 x, I32 y) const {
			return c + m_size(0) * (x + m_size(1) * y);
		}
		/// Converts the image site ImageSite(c,x,y,z) into a memory offset
		inline long m_offset(I32 c, I32 x, I32 y, I32 z) const {
			return c + m_size(0) * (x + m_size(1) * (y + m_size(2) * z));
		}

	};
	//==========================================================================================
	// EO class ImageRam
	//==========================================================================================

	//==========================================================================================
	/// Iterator of ImageRam
	template<class T, class RefT>
	class ImageRamT_iterator {
		FRIEND_CLASS ImageRamT<T> ;
	public:
		ImageRamT_iterator() :
				m_ptr(NULL) {
		}

		RefT operator*() {
			return *m_ptr;
		}

		BOOL operator==(const ImageRamT_iterator& i) const {
			return m_ptr == i.m_ptr;
		}
		BOOL operator!=(const ImageRamT_iterator& i) const {
			return m_ptr != i.m_ptr;
		}

		ImageRamT_iterator<T, RefT>& operator++() {
			m_ptr++;
			return *this;
		}

	protected:
		T* m_ptr;
		ImageRamT_iterator(T* ptr) :
				m_ptr(ptr) {
		}

	};
//==========================================================================================
/// Template codes inclusion
#include "lgl/lglImageRam_code.h"
	//==========================================================================================

}
//==========================================================================================
// EO namespace lgl
//==========================================================================================

//==========================================================================================
// EOF
//==========================================================================================
#endif

