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
/**	\file	
	\brief Code of lgl::SparseArray<T> 
**/
//=============================================================================
// to get memset
//#include <memory>
//=============================================================================

//=============================================================================
// static members
template <class T>
UI32 SparseArray<T>::m_size256ptr = 256*sizeof(void*);
template <class T>
UI32 SparseArray<T>::m_size256T = 256*sizeof(T);
//=============================================================================
#ifdef __GNUC__
//#ifdef __MAC__
#if defined(__MAC__) || defined(__APPLE__)
#include <machine/endian.h>
#else
#if !defined(WIN32)
#include <endian.h>
#endif
#endif
#if BYTE_ORDER == BIG_ENDIAN
#define LGL_BIG_ENDIAN
#endif
#endif

//=============================================================================
// Little/Big Endians
#ifndef LGL_BIG_ENDIAN
const unsigned int byte0=0;
const unsigned int byte1=1;
const unsigned int byte2=2;
const unsigned int byte3=3;
#else
const unsigned int byte0=3;
const unsigned int byte1=2;
const unsigned int byte2=1;
const unsigned int byte3=0;
#undef LGL_BIG_ENDIAN
#endif

//=============================================================================


//=============================================================================
template <class T> SparseArray<T>::SparseArray(T free, BOOL memset_allowed)
: m_free(free), m_memset_allowed (memset_allowed) 
{
	m_root = new void*[256]; 
	memset(m_root,0,m_size256ptr);
} 
//=============================================================================


//=============================================================================
template <class T> SparseArray<T>::SparseArray(const SparseArray& ex)
{
	m_free = ex.m_free;
	m_memset_allowed = ex.m_memset_allowed;
	m_root = new void*[256]; 
	memset(m_root,0,m_size256ptr);
	int    i0,i1,i2,i3;
	void **p0,**p1,**p2;
	void **ep0,**ep1,**ep2;
	T    *p3;
	T    *ep3;
	p0 = m_root;
	ep0 = ex.m_root;
	for (i0=0;i0<256;i0++) {
		if (ep1=(void**)*ep0) {
			*p0 = new void*[256];
			p1 = (void**)*p0;
			for (i1=0;i1<256;i1++) {
				if (ep2=(void**)*ep1) {
					*p1 = new void*[256];
					p2 = (void**)*p1; 
					for (i2=0;i2<256;i2++) {
						if (ep3=(T*)*ep2) {
							*p2 = new T[256];
							p3 = (T*)*p2;
							for (i3=0;i3<256;i3++) p3[i3] = ep3[i3];
						}
						p2++; 
						ep2++;
					}
				}
				p1++;
				ep1++;
			}
		}
		p0++;
		ep0++;
	}
} 
//=============================================================================


//=============================================================================
template <class T> SparseArray<T>& SparseArray<T>::operator = (const SparseArray<T>& ex)
{
	if (this!=&ex) {
		m_free = ex.m_free;
		m_memset_allowed = ex.m_memset_allowed;

		//  cur.i = 0;
		m_root = new void*[256]; 
		memset(m_root,0,m_size256ptr);
		int    i0,i1,i2,i3;
		void **p0,**p1,**p2;
		void **ep0,**ep1,**ep2;
		T    *p3;
		T    *ep3;
		p0 = m_root;
		ep0 = ex.m_root;
		for (i0=0;i0<256;i0++) {
			if (ep1=(void**)*ep0) {
				*p0 = new void*[256];
				p1 = (void**)*p0;
				for (i1=0;i1<256;i1++) {
					if (ep2=(void**)*ep1) {
						*p1 = new void*[256];
						p2 = (void**)*p1; 
						for (i2=0;i2<256;i2++) {
							if (ep3=(T*)*ep2) {
								*p2 = new T[256];
								p3 = (T*)*p2;
								for (i3=0;i3<256;i3++) p3[i3] = ep3[i3];
							}
							p2++; 
							ep2++;
						}
					}
					p1++;
					ep1++;
				}
			}
			p0++;
			ep0++;
		}		
	}
	return (*this);
} 
//=============================================================================


//=============================================================================
template <class T> SparseArray<T>::~SparseArray()
{ 
	void **p1,**p2;
	T *p3;
	int i0,i1,i2;  
	for (i0=0;i0<256;i0++) {
	  if ((p1=(void**)m_root[i0])) {
			for (i1=0;i1<256;i1++) {
			  if ((p2=(void**)p1[i1])) {
					for (i2=0;i2<256;i2++) { 
					  if ((p3=(T*)p2[i2])) delete[] p3;
					}
					delete[] p2; 
				}
			}
			delete[] p1;
		}
	}
	delete[] m_root;
}
//=============================================================================


//=============================================================================
template <class T> T& SparseArray<T>::operator [] (index rang) const 
{
	UI32_4xUI8 cur;
	cur.i = rang;
	void **p0,**p1,**p2;
	T *p3;

	p0 = m_root+cur.b[byte3];
	if ((p1 = (void**)*p0)) {
		p1 += cur.b[byte2];
		if ((p2 = (void**)*p1)) {
			p2 += cur.b[byte1];
			if ((p3 = (T*)*p2)) {
				p3 += cur.b[byte0];
			} 
			else {
				*p2 = new T[256];	
				p3  = (T*)*p2;
				if (m_memset_allowed) {
					memset(p3,0,SparseArray<T>::m_size256T);
				}
				else {
					T* pp = p3;
					for (int i=0;i<256;i++) {*pp=m_free; pp++;}
				}
				p3 += cur.b[byte0];
			}
		}
		else {
			*p1 = new void*[256];
			p2  = (void**)*p1;
			memset(p2,0,m_size256ptr);
			p2 += cur.b[byte1];
			*p2 = new T[256];
			p3  = (T*)*p2;
			if (m_memset_allowed) {
				memset(p3,0,SparseArray<T>::m_size256T);
			}
			else {
				T* pp = p3;
				for (int i=0;i<256;i++) {*pp=m_free; pp++;}
			}
			p3 += cur.b[byte0];
		}
	}
	else {
		*p0 = new void*[256]; 
		p1  = (void**)*p0;
		memset(p1,0,m_size256ptr);
		p1 += cur.b[byte2];
		*p1 = new void*[256];
		p2  = (void**)*p1;
		memset(p2,0,m_size256ptr);
		p2 += cur.b[byte1];
		*p2 = new T[256];
		p3  = (T*)*p2;
		if (m_memset_allowed) {
			memset(p3,0,SparseArray<T>::m_size256T);
		}
		else {
			T* pp = p3;
			for (int i=0;i<256;i++) {*pp=m_free; pp++;}
		}
		p3 += cur.b[byte0];
	}
	return(*p3);	
}
//=============================================================================


//=============================================================================
// iterators
//=============================================================================


//=============================================================================
template <class T>
typename SparseArray<T>::iterator SparseArray<T>::begin() 
{
	iterator it(m_root,m_free,(void*)&m_end);
	it.cur.i = 0;
	it.p0 = m_root;
	bool vide = true;
	unsigned char avance = 1;
	// while P0
	while ((avance)&&(vide)) {
		// if P1
	  if ((it.p1=(void**)*it.p0)) {
			// while P1
			while ((avance)&&(vide)) {
				// if P2
			  if ((it.p2=(void**)*it.p1)) {
					// while P2
					while ((avance)&&(vide)) {
						// if P3
					  if ((it.p3=(T*)*it.p2)) {
							// while P3
							while ((avance)&&(vide=(*it.p3==m_free))) {
								it.p3++;
								avance = ++it.cur.b[byte0];
							}
						}
						// end if P3
						if (vide) {
							it.p2++;
							avance = ++it.cur.b[byte1];
						}
					}
					// end while P2
				}
				// end if P2
				if (vide) {
					it.p1++;
					avance = ++it.cur.b[byte2];
				}
			}
			// end while P1
		}
		// end if P1
		if (vide) {
			it.p0++;
			avance = ++it.cur.b[byte3];
			// si avance == 0 on a boucle, tout le tableau est vide : on pointe sur m_end
			if (!avance) it.p3 = &m_end;
		}
	}
	// end while P0
	return (it);
}
//=============================================================================


//=============================================================================
template <class T>
typename SparseArray<T>::const_iterator SparseArray<T>::begin() const
{
	const_iterator it(m_root,m_free,(void*)&m_end);
	it.cur.i = 0;
	it.p0 = m_root;
	bool vide = true;
	unsigned char avance = 1;
	// while P0
	while ((avance)&&(vide)) {
		// if P1
	  if ((it.p1=(void**)*it.p0)) {
			// while P1
			while ((avance)&&(vide)) {
				// if P2
			  if ((it.p2=(void**)*it.p1)) {
					// while P2
					while ((avance)&&(vide)) {
						// if P3
					  if ((it.p3=(T*)*it.p2)) {
							// while P3
							while ((avance)&&(vide=(*it.p3==m_free))) {
								it.p3++;
								avance = ++it.cur.b[byte0];
							}
						}
						// end if P3
						if (vide) {
							it.p2++;
							avance = ++it.cur.b[byte1];
						}
					}
					// end while P2
				}
				// end if P2
				if (vide) {
					it.p1++;
					avance = ++it.cur.b[byte2];
				}
			}
			// end while P1
		}
		// end if P1
		if (vide) {
			it.p0++;
			avance = ++it.cur.b[byte3];
			// si avance == 0 on a boucle, tout le tableau est vide
			if (!avance) it.p3 = it.m_end; //&m_end;
		}
	}
	// end while P0
	return (it);
}
//=============================================================================


//=============================================================================
template <class T>
typename SparseArray<T>::iterator SparseArray<T>::end()
{ 
	iterator it(m_root,m_free,(void*)&m_end);
	it.p3 = it.m_end;
	return (it);
}
//=============================================================================


//=============================================================================
template <class T>
typename SparseArray<T>::const_iterator SparseArray<T>::end() const
{ 
	const_iterator it(m_root,m_free,(void*)&m_end);
	it.p3 = it.m_end;
	return (it);
}
//=============================================================================


//=============================================================================
template <class T> 
typename SparseArray<T>::iterator SparseArray<T>::begin(index rang)
{
	iterator it(m_root,m_free,(void*)&m_end);
	it.cur.i = rang;

	it.p0 = m_root+it.cur.b[byte3];
	if ((it.p1 = (void**)*it.p0)) {
		it.p1 += it.cur.b[byte2];
		if ((it.p2 = (void**)*it.p1)) {
			it.p2 += it.cur.b[byte1];
			if ((it.p3 = (T*)*it.p2)) {
				it.p3 += it.cur.b[byte0];
			} 
			else {
				*it.p2 = new T[256];	
				it.p3  = (T*)*it.p2;
				if (m_memset_allowed) {
					memset(it.p3,0,SparseArray<T>::m_size256T);
				}
				else	{
					T* pp = it.p3;
					for (int i=0;i<256;i++) {*pp=m_free; pp++;}
				}
				it.p3 += it.cur.b[byte0];
			}
		}
		else {
			*it.p1 = new void*[256];
			it.p2  = (void**)*it.p1;
			memset(it.p2,0,m_size256ptr);
			it.p2 += it.cur.b[byte1];
			*it.p2 = new T[256];
			it.p3  = (T*)*it.p2;
			if (m_memset_allowed) {
				memset(it.p3,0,SparseArray<T>::m_size256T);
			}
			else {
				T* pp = it.p3;
				for (int i=0;i<256;i++) {*pp=m_free; pp++;}
			}
			it.p3 += it.cur.b[byte0];
		}
	}
	else {
		*it.p0 = new void*[256]; 
		it.p1  = (void**)*it.p0;
		memset(it.p1,0,m_size256ptr);
		it.p1 += it.cur.b[byte2];
		*it.p1 = new void*[256];
		it.p2  = (void**)*it.p1;
		memset(it.p2,0,m_size256ptr);
		it.p2 += it.cur.b[byte1];
		*it.p2 = new T[256];
		it.p3  = (T*)*it.p2;
		if (m_memset_allowed) {
			memset(it.p3,0,SparseArray<T>::m_size256T);
		}
		else {
			T* pp = it.p3;
			for (int i=0;i<256;i++) {*pp=m_free; pp++;}
		}
		it.p3 += it.cur.b[byte0];
	}
	return(it);	
}
//=============================================================================


//=============================================================================
template <class T> 
typename SparseArray<T>::const_iterator SparseArray<T>::begin(index rang) const
{
	const_iterator it(m_root,m_free,(void*)&m_end);
	it.cur.i = rang;
	it.p0 = m_root+it.cur.b[byte3];
	if (it.p1 = (void**)*it.p0) {
		it.p1 += it.cur.b[byte2];
		if (it.p2 = (void**)*it.p1) {
			it.p2 += it.cur.b[byte1];
			if (it.p3 = (T*)*it.p2) {
				it.p3 += it.cur.b[byte0];
			} 
			else {
				*it.p2 = new T[256];	
				it.p3  = (T*)*it.p2;
				if (m_memset_allowed) {
					memset(it.p3,0,SparseArray<T>::m_size256T);
				}
				else {
					T* pp = it.p3;
					for (int i=0;i<256;i++) {*pp=m_free; pp++;}
				}
				it.p3 += it.cur.b[byte0];
			}
		}
		else {
			*it.p1 = new void*[256];
			it.p2  = (void**)*it.p1;
			memset(it.p2,0,m_size256ptr);
			it.p2 += it.cur.b[byte1];
			*it.p2 = new T[256];
			it.p3  = (T*)*it.p2;
			if (m_memset_allowed) {
				memset(it.p3,0,SparseArray<T>::m_size256T);
			}
			else {
				T* pp = it.p3;
				for (int i=0;i<256;i++) {*pp=m_free; pp++;}
			}
			it.p3 += it.cur.b[byte0];
		}
	}
	else {
		*it.p0 = new void*[256]; 
		it.p1  = (void**)*it.p0;
		memset(it.p1,0,m_size256ptr);
		it.p1 += it.cur.b[byte2];
		*it.p1 = new void*[256];
		it.p2  = (void**)*it.p1;
		memset(it.p2,0,m_size256ptr);
		it.p2 += it.cur.b[byte1];
		*it.p2 = new T[256];
		it.p3  = (T*)*it.p2;
		if (m_memset_allowed) {
			memset(it.p3,0,SparseArray<T>::m_size256T);
		}
		else {
			T* pp = it.p3;
			for (int i=0;i<256;i++) {*pp=m_free; pp++;}
		}
		it.p3 += it.cur.b[byte0];
	}
	return(it);	
}
//=============================================================================


//=============================================================================
template <class T, class RefT> 
SparseArrayIterator<T,RefT>::SparseArrayIterator(void** r, T free, void* end)
: m_free(free), m_end((T*)end), m_root(r) {}
//=============================================================================

//=============================================================================
template <class T, class RefT> 
SparseArrayIterator<T,RefT>::SparseArrayIterator(const SparseArrayIterator<T,RefT>& i)
: m_free(i.m_free), m_end(i.m_end), m_root(i.m_root), 
	p0(i.p0), p1(i.p1), p2(i.p2), p3(i.p3), cur(i.cur)
{}
//=============================================================================


//=============================================================================
template <class T, class RefT> 
SparseArrayIterator<T,RefT>& SparseArrayIterator<T,RefT>::operator = 
(const SparseArrayIterator<T,RefT>& i)
{ 
	m_free=i.m_free;
	m_end=i.m_end;
	m_root=i.m_root; 
	p0=i.p0;
	p1=i.p1; 
	p2=i.p2; 
	p3=i.p3;
	cur.i=i.cur.i;
	return (*this);
}
//=============================================================================


//=============================================================================
template <class T, class RefT> 
SparseArrayIterator<T,RefT>& SparseArrayIterator<T,RefT>::operator++()
{
//	lglLOG("#op++["<<cur.i<<"]");
  do {// Avancée dans le segment terminal
    ++p3;
    // Avancée de l'indice dans ce segment : bouclage a 0 = fin du segment
    if ((++cur.b[byte0])==0) {
      do { // Avancée du segment superieur 
        ++p2;
        if ((++cur.b[byte1])==0) { // Remontee
          do {
            ++p1;
            if ((++cur.b[byte2])==0) { // Remontee
              do {
                ++p0;
                // niveau racine : bouclage = fin de tableau
                if ((++cur.b[byte3])==0) { p3 = m_end; return (*this); }
              }
              while (*p0==0);
              p1 = (void**)*p0;
            }
          }
          while (*p1==0);
          p2 = (void**)*p1;
        }
      }
      while (*p2==0);
      p3 = (T*)*p2;
    }
  }
  while (*p3==m_free);
  return (*this);
}
//=============================================================================


//=============================================================================
template <class T> 
typename SparseArray<T>::hole_iterator SparseArray<T>::begin_hole()
{
	hole_iterator it(m_root,m_free,(void*)&m_end,m_memset_allowed);
  it.cur.i = 0;
  it.p0 = m_root;
  bool occupe = true;
  unsigned char avance = 1;
  while ((avance)&&(occupe)) {
    // P1 posit sur p0[0]
    if (it.p1=(void**)*it.p0) {
      while ((avance)&&(occupe)) {
	      // P2 posit sur p1[0]
	      if (it.p2=(void**)*it.p1) {
	        while ((avance)&&(occupe)) {
	          // P3 posit sur p2[0]
	          if (it.p3=(T*)*it.p2) {
	            // tant que <256 et cases occupee
	            while ((avance)&&(occupe=(*it.p3!=m_free))) {
		            // incr P3
		            it.p3++;
  					// et compteur (si retour 0 => fin)
	  				avance = ++it.cur.b[byte0];
	            } 
	            // On sort car cellule courante vide ou fin de segment	      
	          }
	          // si P3 non alloue => vide
	          else {
	            // on l'alloue et on se posit sur 1er
	            occupe = false;
	            *it.p2 = new T[256];
	            it.p3  = (T*)*it.p2;
	            if (m_memset_allowed) {
					memset(it.p3,0,SparseArray<T>::m_size256T);
				}
				else {
					T* pp = it.p3;
					for (int i=0;i<256;i++) {*pp=m_free; pp++;}
				}
	          }   
	          // P3 OKOK
	          // Si occupe on avance
	          if (occupe) {
	            // incr P2
	            it.p2++;
	            avance = ++it.cur.b[byte1];
	            // si avance = 0 (on a boucle) on remonte
	          }
	        }
	      }
	      // si P2 non alloue => vide
	      else {
	        occupe = false;
	        *it.p1 = new void*[256];
	        it.p2  = (void**)*it.p1;
	        memset(it.p2,0,m_size256ptr);
	        *it.p2 = new T[256];
	        it.p3  = (T*)*it.p2;
	        if (m_memset_allowed) {
						memset(it.p3,0,SparseArray<T>::m_size256T);
					}
					else {
						T* pp = it.p3;
						for (int i=0;i<256;i++) {*pp=m_free; pp++;}
					}
	      }
	      // P2 OK
	      // si occupe on avance
	      if (occupe) {
	        it.p1++;
	        avance = ++it.cur.b[byte2];
	        // si avance = 0 (on a boucle) on remonte
	      }
      } 
    }
    // si P1 non alloue => vide
    else {
      occupe = false;
      *it.p0 = new void*[256]; 
      it.p1  = (void**)*it.p0;
      memset(it.p1,0,m_size256ptr);
      *it.p1 = new void*[256];
      it.p2  = (void**)*it.p1;
      memset(it.p2,0,m_size256ptr);
      *it.p2 = new T[256];
      it.p3  = (T*)*it.p2;
      if (m_memset_allowed) {
				memset(it.p3,0,SparseArray<T>::m_size256T);
			}
			else {
				T* pp = it.p3;
				for (int i=0;i<256;i++) {*pp=m_free; pp++;}
			}
    }
    // P1 OK 
    // si occupe on avance
    if (occupe) {
      it.p0++;
      avance = ++it.cur.b[byte3];
      // si avance = 0 (on a boucle) : le tableau est PLEIN !!!
      if (!avance) it.p3=&m_end;
    }
  }
  return (it);
}
//=============================================================================


//=============================================================================
template <class T> 
typename SparseArray<T>::const_hole_iterator SparseArray<T>::begin_hole() const
{
	const_hole_iterator it(m_root,m_free,(void*)&m_end,m_memset_allowed);
  it.cur.i = 0;
  it.p0 = m_root;
  bool occupe = true;
  bool avance = true;
  while ((avance)&&(occupe)) {
    // P1 posit sur p0[0]
    if (it.p1=(void**)*it.p0) {
      while ((avance)&&(occupe)) {
	      // P2 posit sur p1[0]
	      if (it.p2=(void**)*it.p1) {
	        while ((avance)&&(occupe)) {
	          // P3 posit sur p2[0]
	          if (it.p3=(T*)*it.p2) {
	            // tant que <256 et cases occupee
	            while ((avance)&&(occupe=(*it.p3!=m_free))) {
		            // incr P3
		            it.p3++;
  		          // et compteur (si retour 0 => fin)
	  	          avance = ++it.cur.b[byte0];
	            } 
	            // On sort car cellule courante vide ou fin de segment	      
	          }
	          // si P3 non alloue => vide
	          else {
	            // on l'alloue et on se posit sur 1er
	            occupe = false;
	            *it.p2 = new T[256];
	            it.p3  = (T*)*it.p2;
	            if (m_memset_allowed) {
								memset(it.p3,0,SparseArray<T>::m_size256T);
							}
							else {
								T* pp = it.p3;
								for (int i=0;i<256;i++) {*pp=m_free; pp++;}
							}
	          }   
	          // P3 OKOK
	          // Si occupe on avance
	          if (occupe) {
	            // incr P2
	            it.p2++;
	            avance = ++it.cur.b[byte1];
	            // si avance = 0 (on a boucle) on remonte
	          }
	        }
	      }
	      // si P2 non alloue => vide
	      else {
	        occupe = false;
	        *it.p1 = new void*[256];
	        it.p2  = (void**)*it.p1;
	        memset(it.p2,0,m_size256ptr);
	        *it.p2 = new T[256];
	        it.p3  = (T*)*it.p2;
	        if (m_memset_allowed) {
						memset(it.p3,0,SparseArray<T>::m_size256T);
					}
					else {
						T* pp = it.p3;
						for (int i=0;i<256;i++) {*pp=m_free; pp++;}
					}
	      }
	      // P2 OK
	      // si occupe on avance
	      if (occupe) {
	        it.p1++;
	        avance = ++it.cur.b[byte2];
	        // si avance = 0 (on a boucle) on remonte
	      }
      } 
    }
    // si P1 non alloue => vide
    else {
      occupe = false;
      *it.p0 = new void*[256]; 
      it.p1  = (void**)*it.p0;
      memset(it.p1,0,m_size256ptr);
      *it.p1 = new void*[256];
      it.p2  = (void**)*it.p1;
      memset(it.p2,0,m_size256ptr);
      *it.p2 = new T[256];
      it.p3  = (T*)*it.p2;
      if (m_memset_allowed) {
				memset(it.p3,0,SparseArray<T>::m_size256T);
			}
			else {
				T* pp = it.p3;
				for (int i=0;i<256;i++) {*pp=m_free; pp++;}
			}
    }
    // P1 OK 
    // si occupe on avance
    if (occupe) {
      it.p0++;
      avance = ++it.cur.b[byte3];
      // si avance = 0 (on a boucle) : le tableau est PLEIN !!!
      if (!avance) it.p3=&m_end;
    }
  }
  return (it);
}
//=============================================================================


//=============================================================================
template <class T>
typename SparseArray<T>::hole_iterator SparseArray<T>::end_hole()
{ 
	hole_iterator it(m_root,m_free,(void*)&m_end,m_memset_allowed);
	it.p3 = it.m_end;
	return (it);
}
//=============================================================================


//=============================================================================
template <class T>
typename SparseArray<T>::const_hole_iterator SparseArray<T>::end_hole() const
{ 
	const_hole_iterator it(m_root,m_free,(void*)&m_end);
	it.p3 = it.m_end;
	return (it);
}
//=============================================================================


//=============================================================================
template <class T> 
typename SparseArray<T>::hole_iterator SparseArray<T>::begin_hole(index rang)
{
	hole_iterator it(m_root,m_free,(void*)&m_end,m_memset_allowed);
	it.cur.i = rang;
//  cout << cur.i << " = " << int(cur.b[byte3]) << " - "<< int(cur.b[byte2])
//  <<" - "<<int(cur.b[byte1])<<" - "<<int(cur.b[byte0])<<std::endl;
	it.p0 = m_root+it.cur.b[byte3];
	if (it.p1 = (void**)*it.p0) {
		it.p1 += it.cur.b[byte2];
		if (it.p2 = (void**)*it.p1) {
			it.p2 += it.cur.b[byte1];
			if (it.p3 = (T*)*it.p2) {
				it.p3 += it.cur.b[byte0];
			} 
			else {
				*it.p2 = new T[256];	
				it.p3  = (T*)*it.p2;
				if (m_memset_allowed) {
					memset(it.p3,0,SparseArray<T>::m_size256T);
				}
				else {
					T* pp = it.p3;
					for (int i=0;i<256;i++) {*pp=m_free; pp++;}
				}
				it.p3 += it.cur.b[byte0];
			}
		}
		else {
			*it.p1 = new void*[256];
			it.p2  = (void**)*it.p1;
			memset(it.p2,0,m_size256ptr);
			it.p2 += it.cur.b[byte1];
			*it.p2 = new T[256];
			it.p3  = (T*)*it.p2;
			if (m_memset_allowed) {
				memset(it.p3,0,SparseArray<T>::m_size256T);
			}
			else {
				T* pp = it.p3;
				for (int i=0;i<256;i++) {*pp=m_free; pp++;}
			}
			it.p3 += it.cur.b[byte0];
		}
	}
	else {
		*it.p0 = new void*[256]; 
		it.p1  = (void**)*it.p0;
		memset(it.p1,0,m_size256ptr);
		it.p1 += it.cur.b[byte2];
		*it.p1 = new void*[256];
		it.p2  = (void**)*it.p1;
		memset(it.p2,0,m_size256ptr);
		it.p2 += it.cur.b[byte1];
		*it.p2 = new T[256];
		it.p3  = (T*)*it.p2;
		if (m_memset_allowed) {
			memset(it.p3,0,SparseArray<T>::m_size256T);
		}
		else {
			T* pp = it.p3;
			for (int i=0;i<256;i++) {*pp=m_free; pp++;}
		}
		it.p3 += it.cur.b[byte0];
	}
	return(it);	
}
//=============================================================================


//=============================================================================
template <class T> 
typename SparseArray<T>::const_hole_iterator SparseArray<T>::begin_hole(index rang) const
{
	const_hole_iterator it(m_root,m_free,(void*)&m_end,m_memset_allowed);
	it.cur.i = rang;
//  cout << cur.i << " = " << int(cur.b[byte3]) << " - "<< int(cur.b[byte2])
//  <<" - "<<int(cur.b[byte1])<<" - "<<int(cur.b[byte0])<<std::endl;
	it.p0 = m_root+it.cur.b[byte3];
	if (it.p1 = (void**)*it.p0) {
		it.p1 += it.cur.b[byte2];
		if (it.p2 = (void**)*it.p1) {
			it.p2 += it.cur.b[byte1];
			if (it.p3 = (T*)*it.p2) {
				it.p3 += it.cur.b[byte0];
			} 
			else {
				*it.p2 = new T[256];	
				it.p3  = (T*)*it.p2;
				if (m_memset_allowed) {
					memset(it.p3,0,SparseArray<T>::m_size256T);
				}
				else {
					T* pp = it.p3;
					for (int i=0;i<256;i++) {*pp=m_free; pp++;}
				}
				it.p3 += it.cur.b[byte0];
			}
		}
		else {
			*it.p1 = new void*[256];
			it.p2  = (void**)*it.p1;
			memset(it.p2,0,m_size256ptr);
			it.p2 += it.cur.b[byte1];
			*it.p2 = new T[256];
			it.p3  = (T*)*it.p2;
			if (m_memset_allowed) {
				memset(it.p3,0,SparseArray<T>::m_size256T);
			}
			else {
				T* pp = it.p3;
				for (int i=0;i<256;i++) {*pp=m_free; pp++;}
			}
			it.p3 += it.cur.b[byte0];
		}
	}
	else {
		*it.p0 = new void*[256]; 
		it.p1  = (void**)*it.p0;
		memset(it.p1,0,m_size256ptr);
		it.p1 += it.cur.b[byte2];
		*it.p1 = new void*[256];
		it.p2  = (void**)*it.p1;
		memset(it.p2,0,m_size256ptr);
		it.p2 += it.cur.b[byte1];
		*it.p2 = new T[256];
		it.p3  = (T*)*it.p2;
		if (m_memset_allowed) {
			memset(it.p3,0,SparseArray<T>::m_size256T);
		}
		else {
			T* pp = it.p3;
			for (int i=0;i<256;i++) {*pp=m_free; pp++;}
		}
		it.p3 += it.cur.b[byte0];
	}
	return(it);	
}
//=============================================================================

//=============================================================================
template <class T, class RefT> 
SparseArrayHoleIterator<T,RefT>::SparseArrayHoleIterator(void** r, T free, void* end, bool init)
: m_free(free), m_end((T*)end), m_memset_allowed(init), m_root(r) {}
//=============================================================================


//=============================================================================
template <class T, class RefT> 
SparseArrayHoleIterator<T,RefT>::SparseArrayHoleIterator(const SparseArrayHoleIterator<T,RefT>& i)
: m_free(i.m_free), m_end(i.m_end), m_memset_allowed(i.m_memset_allowed), m_root(i.m_root), 
	p0(i.p0), p1(i.p1), p2(i.p2), p3(i.p3), cur(i.cur)
{}
//=============================================================================


//=============================================================================
template <class T, class RefT> 
SparseArrayHoleIterator<T,RefT>& SparseArrayHoleIterator<T,RefT>::operator = 
(const SparseArrayHoleIterator<T,RefT>& i)
{ 
	m_free=i.m_free;
	m_end=i.m_end;
	m_memset_allowed=i.m_memset_allowed;
	m_root=i.m_root; 
	p0=i.p0;
	p1=i.p1; 
	p2=i.p2; 
	p3=i.p3;
	cur.i=i.cur.i;
	return (*this);
}
//=============================================================================


//=============================================================================
template <class T, class RefT> 
SparseArrayHoleIterator<T,RefT>& SparseArrayHoleIterator<T,RefT>::operator++()
{
	do {// Avancée dans le segment terminal
		++p3;
		// Avancée de l'indice dans ce segment : bouclage a 0 = fin du segment
		if ((++cur.b[byte0])==0) {
			// Avancée du segment superieur 
			++p2;
			if ((++cur.b[byte1])==0) { // Remontee
				++p1;
				if ((++cur.b[byte2])==0) { // Remontee
					++p0;
					// niveau racine : bouclage = fin de tableau
					if ((++cur.b[byte3])==0) { p3 = m_end; return (*this); }
					// Si segment N1 non alloué : on l'alloue
					if (*p0==0) {
						*p0 = new void*[256]; 
						p1  = (void**)*p0;
						memset(p1,0,SparseArray<T>::m_size256ptr);
					}
					else p1  = (void**)*p0;
				}
				// Si segment N2 non alloue
				if (*p1==0) {
					*p1 = new void*[256];
					p2  = (void**)*p1;
					memset(p2,0,SparseArray<T>::m_size256ptr);
				}
				else p2 = (void**)*p1;
			}
			// Si segment N3 non alloué
			if (*p2==0) {
				*p2 = new T[256];
				p3  = (T*)*p2;
				if (m_memset_allowed) {
					memset(p3,0,SparseArray<T>::m_size256T);
				}
			  else {
					T* pp = p3;
					for (int i=0;i<256;i++) {*pp=m_free; pp++;}
				}
			}
			else p3 = (T*)*p2;
    }
  }
  while (*p3!=m_free);
  return (*this);
  
}
//=============================================================================

//=============================================================================
template <class T> UI32 SparseArray<T>::freeUnusedMemory (BOOL verbose)
{
	if (verbose) {
		lglLOG ( "SparseArray<T>::freeUnusedMemory"<<std::endl);
	}
	UI32 mem = 0;
	// unsigned char s1,s2,
	bool vide2,vide3,vide4;
	int i,j,k,l;
	void **n1,**n2;
	T* seg;
	// balayage racine 
	for (i=0;i<256;i++) {
		// si branche allouee
	  if ((n1=(void**)m_root[i])) {
			// balayage niveau 1 
			vide2 = true;
			for (j=0; j<256; j++) {
			  if ((n2=(void**)n1[j])) {
					// balayage niveau 2
					vide3 = true;
					for (k=0; k<256; k++) {
					  if ((seg=(T*)n2[k])) {
							// balayage segment terminal
							vide4 = true;
							l = 0;
							while ((vide4)&&(l<256)) { vide4 = (seg[l]==m_free); l++; }
							// si toutes cellule vides
							if (vide4) {
								if (verbose) {
									int ii = 256*(k+256*(j+256*i));
									lglLOG( "Level 3 segment " << seg << " : " 
									<< ii << " - " << ii+255 << " empty : " 
									<< SparseArray<T>::m_size256T << " bytes freed"<<std::endl );
								}
								// destruction segment
								delete[] seg; 
								n2[k]=NULL; 
								mem+=SparseArray<T>::m_size256T;
							}
							vide3 = (vide3)&&(vide4);
						}
					}
					// si toutes cellules N2 vides
					if (vide3) {
				  		if (verbose) {
			  	  			int ii = 256*(256*(j+256*i));
			  				lglLOG( "Level 2 segment " << n2 << " : " 
			  				<< ii << " - " << ii+256*256-1 << " empty : " 
			  				<< m_size256ptr << " bytes freed"<<std::endl );
			  			} 
			  			delete[] n2; 
				  		n1[j]=0; 
				  		mem+=m_size256ptr;
					}
					vide2 = (vide2)&&(vide3);
					// fin balayage N2
				}
			}
			// si N1 vide 
			if (vide2) {
				if (verbose) {
					int ii = 256*(256*(256*i));
					lglLOG( "Level 1 segment " << n1 << " : " 
					<< ii << " - " << ii+256*256*256-1 << " empty : " 
					<< m_size256ptr << " bytes freed"<<std::endl );
				} 
				delete[] n1; 
				m_root[i]=0; 
				mem+=m_size256ptr;
			}
		}
	}  
	if (verbose) {
		lglLOG ( mem << " total byte(s) freed" <<std::endl );
	}
	return(mem);
}
//=============================================================================


//=============================================================================
template <class T> UI32 SparseArray<T>::memoryUsage(BOOL verbose) const
{ 
	UI32 mem = m_size256ptr; // basic alloc = m_root
	void **p1,**p2;
	T    *p3;
	int i0,i1,i2;  
	UI32 l1m=0,l2m=0,l3m=0; 
	for (i0=0;i0<256;i0++) {
	  if ((p1=(void**)m_root[i0])) {
			l1m += m_size256ptr;
			for (i1=0;i1<256;i1++) {
			  if ((p2=(void**)p1[i1])) {
					l2m += m_size256ptr;
					for (i2=0;i2<256;i2++) { 
					  if ((p3=(T*)p2[i2])) {
							l3m += m_size256T;
						}
					}
				}
			}
		}
	}
	mem = mem + l1m + l2m + l3m;
	if (verbose) {
		lglLOG("SparseArray<T>::memoryUsage : L1="<<l1m<<" L2="<<l2m<<" L3="<<l3m<<" --- T="<<mem<<std::endl);
	}
	return (mem);
}
//=============================================================================


//=============================================================================
template <class T> 
UI32 SparseArray<T>::size() const
{ 
	index n = 0;
	void **p1,**p2;
	T    *p3;
	int i0,i1,i2,i3;  
	for (i0=0;i0<256;i0++) {
		if (p1=(void**)m_root[i0]) {
			for (i1=0;i1<256;i1++) {
				if (p2=(void**)p1[i1]) {
					for (i2=0;i2<256;i2++) { 
						if (p3=(T*)p2[i2]) {
							for (i3=0;i3<256;i3++) {
								if (p3[i3]!=m_free) n++;
							}
						}
					}
				}
			}
		}
	}
	return (n);
}
//=============================================================================



//=============================================================================
template <class T>  std::ostream& operator << (std::ostream& s, SparseArray<T>& t)
{
	s << "[";
	for (typename SparseArray<T>::iterator i=t.begin(); i!=t.end(); ++i) {
		s << *i << " ";
	}
	s << "]";
	return s;
}
//=============================================================================



//==============================================================================
// EOF
//==============================================================================



