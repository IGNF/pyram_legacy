#ifndef SSECH_HPP_
#define SSECH_HPP_

#include <ignBufferImage.h>


namespace ign {

	namespace image {

		template<class T>
		void underSampling(ign::image::BufferImage<T>& inputImage, unsigned int factor);

#include "ignImageSampling_code.h"

			}
}


#endif /* SSECH_HPP_ */
