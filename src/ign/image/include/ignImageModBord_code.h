namespace ign {
namespace image {


template<class T>
void flipY( ign::image::BufferImage<T> & image) {
	for (size_t i = 0; i < image.numLines(); i++) {
		for (size_t j = 0; j < image.numCols() / 2; j++) {
			for (size_t c = 0; c < image.numBands(); c++) {
				T val1 = image.get(j, i, c);
				T val2 = image.get(image.numCols() - 1 - j, i, c);
				image.set(j, i, c, val2);
				image.set(image.numCols() - 1 - j, i, c, val1);
			}
		}
	}
}

template<class T>
void flipX( ign::image::BufferImage<T> & image) {

	for (size_t j = 0; j < image.numCols(); j++) {
		for (size_t i = 0; i < image.numLines() / 2; i++) {
			for (size_t c = 0; c < image.numBands(); c++) {
				//if (i > image.numLines()) continue;
				T val1 = image.get(j, i, c);
				T val2 = image.get(j, image.numLines() - 1 - i, c);
				image.set(j, i, c, val2);
				image.set(j, image.numLines() - 1 - i, c, val1);
			}
		}
	}
}


template<class T>
ign::image::BufferImage<T> * addBorderMirror( ign::image::BufferImage<T> * image, int lmarge) {
	
	// rempli imtmp avec image.
     ign::image::BufferImage<T> imtmp(image->numCols(), image->numLines(), image->numBands(), 0);
	image->read(0, 0, 0, image->numCols(), image->numLines(), image->numBands(), 1, imtmp.getPtr(), imtmp.typePixel(), imtmp.getPixelSpace(),
			imtmp.getLineSpace(), imtmp.getBandSpace());

    // reassigne image a une taille plus grande.
	image->initialize(imtmp.numCols() + 2 * lmarge, imtmp.numLines() + 2 * lmarge, imtmp.numBands(), 0);

	// rempli l'image.
	image->write(lmarge, lmarge, 0, imtmp.numCols(), imtmp.numLines(), imtmp.numBands(), imtmp.getPtr(), imtmp.typePixel(), imtmp.getPixelSpace(),
			imtmp.getLineSpace(), imtmp.getBandSpace());

	// un buffer temporaire vertical
	imtmp.initialize(lmarge, image->numLines() - 2 * lmarge, image->numBands(), 0);

	// a gauche
	// lit dans image et rempli imtmp
	image->read(lmarge, lmarge, 0, imtmp.numCols(), imtmp.numLines(), imtmp.numBands(), 1, imtmp.getPtr(), imtmp.typePixel(), imtmp.getPixelSpace(),
			imtmp.getLineSpace(), imtmp.getBandSpace());
	flipY(imtmp);
	image->write(0, lmarge, 0, lmarge, imtmp.numLines(), imtmp.numBands(), imtmp.getPtr(), imtmp.typePixel(), imtmp.getPixelSpace(), imtmp.getLineSpace(),
			imtmp.getBandSpace());

	image->read(image->numCols() - 2 * lmarge, lmarge, 0, imtmp.numCols(), imtmp.numLines(), imtmp.numBands(), 1, imtmp.getPtr(), imtmp.typePixel(),
			imtmp.getPixelSpace(), imtmp.getLineSpace(), imtmp.getBandSpace());
	flipY(imtmp);
	image->write(image->numCols() - lmarge, lmarge, 0, lmarge, imtmp.numLines(), imtmp.numBands(), imtmp.getPtr(), imtmp.typePixel(), imtmp.getPixelSpace(),
			imtmp.getLineSpace(), imtmp.getBandSpace());

	// un buffer temporaire horizontal
	imtmp.initialize(image->numCols(), lmarge, image->numBands(), 0);
	// lit dans image et rempli imtmp
	image->read(0, lmarge, 0, imtmp.numCols(), imtmp.numLines(), imtmp.numBands(), 1, imtmp.getPtr(), imtmp.typePixel(), imtmp.getPixelSpace(),
			imtmp.getLineSpace(), imtmp.getBandSpace());

	flipX(imtmp);

	image->write(0, 0, 0, imtmp.numCols(), imtmp.numLines(), imtmp.numBands(), imtmp.getPtr(), imtmp.typePixel(), imtmp.getPixelSpace(), imtmp.getLineSpace(),
			imtmp.getBandSpace());

	image->read(0, image->numLines() - 2 * lmarge, 0, imtmp.numCols(), imtmp.numLines(), imtmp.numBands(), 1, imtmp.getPtr(), imtmp.typePixel(),
			imtmp.getPixelSpace(), imtmp.getLineSpace(), imtmp.getBandSpace());

	flipX(imtmp);

	image->write(0, image->numLines() - lmarge, 0, imtmp.numCols(), imtmp.numLines(), imtmp.numBands(), imtmp.getPtr(), imtmp.typePixel(), imtmp.getPixelSpace(),
			imtmp.getLineSpace(), imtmp.getBandSpace());


	return image;
}

template<class T>
 ign::image::BufferImage<T>* deleteBorder( ign::image::BufferImage<T> & image, int lmarge) {

     ign::image::BufferImage<T>* imtmp = new  ign::image::BufferImage<T>();

	imtmp->crop(image, lmarge, lmarge, image.numCols() - lmarge, image.numLines() - lmarge, 0);

	return imtmp;
}

}}