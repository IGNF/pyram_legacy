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
	\brief Code of the methods defined in lglBasicAlgorithms.h
*/
//==========================================================================================
/// Computes the connected components of I for the N-neighborhood relationship.
template <class T>
void ConnectedComponents( const ImageRamT<T>& I, Neighborhood& N, ImageRamT<int>& C, int& number)
{
	// Output image size = input size 
	ImageSite s(I.size());
	// but... ONE CHANNEL even if input is multi-channel ! (bug fix 06/10/04)
	s(0)=1;
	// 
	C.resize(s);
	C.mset(-1);
	// current I value  
	double li;
	number=0;
	
	ImageBloc sit = I.domain();
	for (sit.start();!sit.stop();++sit) {
		if (C.get(*sit)!=-1) continue;
		C.set((*sit),number);
		li = I.get(*sit);
		// queue
		std::vector<ImageSite> Q;
		Q.push_back(*sit);
		while (Q.size()>0) {
			ImageSite p = Q.back();
			Q.pop_back();
			for (N.begin();!N.end();++N) {
				ImageSite v = p + (*N);
				if ( (I.is_inside(v)) && (C.get(v)==-1) && (I.get(v)==li) ) {
					C.set(v,number);
					Q.push_back(v);
				}
			}
		}
		++number;
	}
}
//==========================================================================================

	
//==========================================================================================
/// Computes the local optima of I for the N-neighborhood relationship.
/// Returns a label image in which a value is nonzero iff its a local optimum.
/// Local optima are mapped to {1,..,number}
template <class T>
void LocalOptima( const ImageRamT<T>& I, Neighborhood& N, ImageRamT<int>& opt, int& number)
{
	// Computes the flat zones of I for N, i.e. its Conn. Comp.
	ConnectedComponents(I,N,opt,number);		

	// Number of inferior/superior neighbours of each flat zone
	std::vector<int> nvinf(number),nvsup(number);
	for (int i=0;i<number;++i) {
		nvinf[i]=nvsup[i]=0;
	}
	// count
	ImageBloc it = opt.domain();
	
	for (it.start();!it.stop();++it) {
		int l = (int)(opt.get(*it));
		double v = I.get(*it);
		for (N.begin(); !N.end(); ++N) {
			ImageSite n = (*it)+(*N);
			if (!I.is_inside(n)) continue;
			if (opt.get(n)==l) continue;
			double vv = I.get(n);
			if (vv<v) nvinf[l]++;
			else if (vv>v) nvsup[l]++;
		}
	}

	// local extrema 
	number = 0;
	ImageRamT<int>::iterator lit;
	for (lit=opt.begin();lit!=opt.end();++lit) {
		if ( (nvinf[*lit]!=0) ^ (nvsup[*lit]!=0) ) {
			number++;
			*lit = number;
			nvinf[*lit] = nvsup[*lit] = 0;
		}
		else {
			*lit = 0;
			nvinf[*lit] = nvsup[*lit] = 0;
		}
	}

}
//==========================================================================================

