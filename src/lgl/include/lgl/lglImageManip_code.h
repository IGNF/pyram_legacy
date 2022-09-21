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
/*! \file 
	\brief Codes of the template methods defined in lglImageManip.h
*/
	

	//========================================================================================
	template <class I, class T>
	void Imap( I, const ImageRam& rin, std::map<T,T>& map, pImageRam& rout, int unmapped_label  )
	{
		// downcast
		const ImageRamT<I>* in = dynamic_cast<const ImageRamT<I>*>(&rin);
		ImageRamT<I>* out = dynamic_cast<ImageRamT<I>*>(rout);
		if (!out) {
			out = new ImageRamT<I>;
		}
		rout = out;
		out->resize(in->size());

		typename ImageRamT<I>::const_iterator init;
		typename ImageRamT<I>::iterator outit;
		for (init=in->begin(),outit=out->begin();init!=in->end();++init,++outit) {
			typename std::map<T,T>::iterator ai;
			ai = map.find((T)(*init));
			if (ai!=map.end()) {
				*outit = (I)((*ai).second);
			}
			else {
				if (unmapped_label == -666) *outit = *init;
				else *outit = (I)(unmapped_label);
			}
		}
	}
	//========================================================================================
	
	//========================================================================================
	/// \brief Maps the values of in to out using the map 
	template <class T>
	void map( const ImageRam& in, std::map<T,T>& map, pImageRam& out, int unmapped_label = -666 )
	{
		lglSwitchOnTypeFunctionCall5 ( in.type(), Imap, in, map, out, unmapped_label  )
	}
	//========================================================================================
