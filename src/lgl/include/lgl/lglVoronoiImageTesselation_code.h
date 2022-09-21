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
	\brief Codes related to lglVoronoiImageTesselation.h
**/
//====================================================================
// Attributed pixels used by GVIP (GenralizedVoronoiImagePartitioner).
// The operator < is used for IndexedHeap sorting
class GVIP_attributed_pixel 
{
protected:

	// position
	ImageSite m_p;
	// distance 
	float m_d;
	// label
	int m_l;

public :
		
	GVIP_attributed_pixel( const ImageSite& position, float distance=0, int label=-1 ) : m_p(position), m_d(distance), m_l(label) {}
	// comparison on m_d
	bool operator < ( const GVIP_attributed_pixel& n ) const { return m_d<n.m_d; }

	// accessors
	ImageSite& site() { return m_p; }
	float& distance() { return m_d; }
	int& label() { return m_l; }

	const ImageSite& site() const { return m_p; }
	float distance() const { return m_d; }
	int label() const { return m_l; }
};
//====================================================================

//====================================================================
inline std::ostream& operator << ( std::ostream& o, const GVIP_attributed_pixel& p ) {
	o << "(" << p.site() << ","<<p.distance() <<","<<p.label()<<")";
	return o;
}
//====================================================================


//====================================================================
/// index on GVIP_attributed_pixels
class GVIP_attributed_pixel_index {
	public :
		ImageRamT<int>* m_i;
		GVIP_attributed_pixel_index( ImageRamT<int>& i ) : m_i (&i) {}
		inline int& operator() (GVIP_attributed_pixel& p) const { return (*m_i)(p.site()); } 
};
//====================================================================


//====================================================================
template <class D>
void VoronoiImageTesselation (	ImageRamT<int>& seeds,
								Neighborhood& V,
								D& dist)
{
  //[RTS] ImageSite t = seeds.size();
	// index uses seeds 
    GVIP_attributed_pixel_index index(seeds);
    
	// priority queue on pixels
    IndexedHeap<	GVIP_attributed_pixel,
					Less<GVIP_attributed_pixel>,
					GVIP_attributed_pixel_index> 
		heap( Less<GVIP_attributed_pixel>::staticInstance(), index );

	ImageRamT<int>::iterator it;
	// transform seeds correctly 
	for (it=seeds.begin();it!=seeds.end();++it) {
		// unreached
		if (*it==0) *it = -1;
		// reached 
		else *it = -(*it)-1;
	}
	// builds the heap
	ImageBloc sit = seeds.domain();
	for (sit.start();!sit.stop();++sit) {
		// label
		int l = (int)(seeds.get(*sit));
		// if is a seed
		if (l<-1) {
			// insert neighbours into heap
			for (V.begin(); !V.end(); ++V) {
				ImageSite n = (*sit) + (*V);
				if (!seeds.is_inside(n)) continue;
				// neighbour's label
				int nl = (int)(seeds.get(n));
				// allready reached
				if (nl<-1) continue;
				// distance 
				double d = dist((*sit),n);
				// point not reached yet
				if (nl==-1) {
					heap.insert(GVIP_attributed_pixel(n,d,l));
				}
				else {
					// distance lower ?
					if ( d < heap[nl].distance()) {
						// new path better
						heap[nl].distance() = d;
						heap[nl].label() = l;
						heap.upsort(nl);
					}
				}
			}
		}
	}
	// seeds growing 
	int nb = 1;
	while (heap.size()>0) {
		nb++;
		// pop up first pixel
		GVIP_attributed_pixel p = heap.remove_top();
		// labelling
		seeds.set(p.site(),p.label());		
		// neighbours
		for (V.begin(); !V.end(); ++V) {
			ImageSite n = p.site() + (*V);
			if (!seeds.is_inside(n)) continue;			
			int nl = (int)(seeds.get(n));
			// already reached
			if (nl<-1) continue;
			// distance
			double d = p.distance() + dist(p.site(),n);
			// not reached yet
			if (nl==-1) {
				heap.insert(GVIP_attributed_pixel(n,d,p.label()));
			}
			else {
				// distance lower ?
				if (d < heap[nl].distance()) {
					// new path better
					heap[nl].distance() = d;
					heap[nl].label() = p.label();
					heap.upsort(nl);
				}
			}
		}
	}
	// remaps correctly seeds to [1,n]
	for (it=seeds.begin();it!=seeds.end();++it) *it = -(*it)-1;
}
//====================================================================





//====================================================================
template <class T>
void LocalOptimaVoronoiTesselation(const ImageRamT<T>& image, Neighborhood& neigh,
										ImageRamT<int>& labels, int& number)
{
	if (image.channels()==1) {
		LocalOptima(image,neigh,labels,number);
		MonoChannelImageVariation<T> distance(image);
		VoronoiImageTesselation ( labels, neigh, distance );
	}
	else {
		ImageRam* intensity = 0;
		averageChannels(image,intensity);
		ImageRamT<T>* tint = dynamic_cast<ImageRamT<T>*>(intensity);
		LocalOptima(*tint,neigh,labels,number);
		MultiChannelImageVariation<T> distance(image);
		VoronoiImageTesselation ( labels, neigh, distance );
		delete intensity;
	}
}
//====================================================================
	
