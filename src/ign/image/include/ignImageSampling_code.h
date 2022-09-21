//#include <ign/image/algorithm/Resampler.h>
//using namespace ign::image;
//using namespace ign::image::algorithm;

template<class T>
void underSampling(BufferImage<T>& inputImage, unsigned int factor) {

	if (factor > 1) {

		// Rééchantillonage avec le socle.
		/*
		 ign::image::BufferImage<T> result(inputImage.numCols()/factor, inputImage.numLines()/factor, inputImage.numBands());
		 ign::image::algorithm::Resampler<T> resampler(&result, NULL, 0);
		 resampler.compute(inputImage, factor, ign::image::algorithm::Resampler<T>::ResampleType, 0, 0, 0,inputImage.numCols(),inputImage.numLines(),inputImage.numBands(),0,0,0);
		 result.save("testSousEch.tif");
		 inputImage = result;
		 */

		// Rééchantillonage à la main.
        ign::image::BufferImage<T> outputImage;
		outputImage.initialize(inputImage.size().first / factor, inputImage.size().second / factor, inputImage.numBands(), 0);
		for (size_t l = 0; l < outputImage.numLines(); ++l)
			for (size_t c = 0; c < outputImage.numCols(); ++c)
				for (size_t nbc = 0; nbc < outputImage.numBands(); ++nbc) {
					float val = 0;
					for (size_t dc = 0; dc < factor; ++dc)
						for (size_t dl = 0; dl < factor; ++dl)
							val += inputImage(factor * c + dc, factor * l + dl, nbc);

					val /= factor * factor;
					outputImage(c, l, nbc) = (T) val;
				}

		//inputImage = outputImage;
		inputImage.initialize(outputImage.numCols(), outputImage.numLines(), outputImage.numBands());
		outputImage.read(0, 0, 0, inputImage.numCols(), inputImage.numLines(), inputImage.numBands(), 1, inputImage.getPtr(), inputImage.typePixel(),
				inputImage.getPixelSpace(), inputImage.getLineSpace(), inputImage.getBandSpace());
	}
}

