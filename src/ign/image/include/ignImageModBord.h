#ifndef __MODE_BORD_H__
#define __MODE_BORD_H__

namespace ign {
	namespace image {
			
		template<class T>
		ign::image::BufferImage<T> * addBorderMirror( ign::image::BufferImage<T> * image, int lmarge) ;

		template<class T>
		ign::image::BufferImage<T>* deleteBorder( ign::image::BufferImage<T> & image, int lmarge);
 
	}
}

#include <ignImageModBord_code.h>

#endif // __MODE_BORD_H__
