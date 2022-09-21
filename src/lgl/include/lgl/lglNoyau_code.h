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
/**	\file	
	\brief Code of the template methods of lglNoyau.h
**/
/// \brief Transforms a noyau TTImage<T> into a lgl::ImageRamT<T>
template <class T>
bool BufferImageToImageRamT ( const ign::image::BufferImage<T>& in, ImageRamT<T>& out )
{
	
	ImageSite s( in.numBands(), in.numCols(), in.numLines());
	if (!out.resize(s)) {
		lglERROR("TTImageToImageRamT : unable to allocate the output image");
//		return false;
	}
	ImageSite p(0,0,0);
	for (p(2)=0;p(2)<s(2);++p(2)) {
		for (p(1)=0;p(1)<s(1);++p(1)) {
			for (p(0)=0;p(0)<s(0);++p(0)) {		
				out.set(p,in(p(1),p(2),p(0)));
			}
		}
	}
	return true;
}
/// \brief Transforms a lgl::ImageRamT<T> into a noyau TTImage<T> 
/*template <class T>
BOOL ImageRamTToBufferImage ( const ImageRamT<T>& in, ign::image::BufferImage<T>& out )
{	

	out.SetTaille(in.size(1),in.size(2));
	out.SetNbCanaux(in.size(0));
	int i,j,c;
	for (j=0;j<in.size(2);++j) {
		for (i=0;i<in.size(1);++i) {
			for (c=0;c<in.size(0);++c) {
				out(i,j,c) = (T)(in.get(ImageSite(c,i,j)));
			}
		}
	}

	return TRUE;
}
*/

/// \brief Transforms a ImageRam into a noyau TTImage<T> 
template <class U, class T>
void TImageRamToBufferImage ( U, const ImageRam& rin, ign::image::BufferImage<T>& out )
{	

	const ImageRamT<U>* in = dynamic_cast<const ImageRamT<U>*>(&rin);
    if (!in) {
        std::cout << "LGL ERRORT during transformation : ImageRamToBufferImage" <<  std::endl;
        return;
    }

	out.initialize(in->size(1),in->size(2), in->size(0));

	//out.SetNbCanaux(in->size(0));
	int i,j,c;
	for (j=0;j<in->size(2);++j) {
		for (i=0;i<in->size(1);++i) {
			for (c=0;c<in->size(0);++c) {
				out(i,j,c) = (T)(in->get(ImageSite(c,i,j)));
			}
		}
	}

}

/// \brief Transforms a ImageRam into a noyau TTImage<T> 
template <class T>
void ImageRamToBufferImage ( const ImageRam& in, ign::image::BufferImage<T>& out )
{
	lglSwitchOnTypeFunctionCall3 ( in.type(), TImageRamToBufferImage, in ,out );
}
