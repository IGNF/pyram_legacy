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
	\brief Codes of BitSlot and BitManager<T>
*/
//=====================================================================================
// binary write in a stream
//=====================================================================================
template <class T>
std::ostream& operator < (std::ostream& s, T x)
{
	const UI8 sz = 8*sizeof(T);
	for (UI8 i=sz-1;i>=0;i--) {
		T mask = 1 << i;
		(x & mask) ? s << "1" : s << "0";
	}
	return s;
}
//=====================================================================================




//=====================================================================================
// class BitManager
//=====================================================================================


//=====================================================================================
// static members 
//=====================================================================================
// size of vectors
template <class T> const UI8 BitManager<T>::m_size = 8*sizeof(T);
// masks
template <class T> T BitManager<T>::m_mask[64*sizeof(T)*sizeof(T)]; 
// masks computed ?
template <class T> BOOL BitManager<T>::m_must_compute_mask = 1;
//=====================================================================================



//=====================================================================================
template <class T>
BitManager<T>::BitManager(T u) : m_use(u), m_n(0) 
{
	if (m_must_compute_mask) compute_mask();
	for (UI8 i=0; i<m_size; i++) if (u & m_mask[i]) m_n++;
}
//=====================================================================================


//=====================================================================================
template <class T>
BitManager<T>::~BitManager() {}
//=====================================================================================

//=====================================================================================
template <class T>
BitManager<T>::BitManager( const BitManager<T>& f ) { m_use = f.m_use; m_n = f.m_n;}
//=====================================================================================

//=====================================================================================
template <class T>
BitManager<T>& BitManager<T>::operator = ( const BitManager<T>& f) 
{ m_use = f.m_use; m_n = f.m_n; return *this; }
//=====================================================================================

//=====================================================================================
template <class T>
bool BitManager<T>::operator == (BitManager<T>& f) const
{ return (m_use==f.m_use); }
//=====================================================================================

//=====================================================================================
template <class T>
int BitManager<T>::nbBits() {return m_size;}
//=====================================================================================

//=====================================================================================
template <class T>
int BitManager<T>::nbBitsFree() const {return m_size-m_n;}
//=====================================================================================

//=====================================================================================
template <class T>
int BitManager<T>::nbBitsUsed() const {return m_n;}
//=====================================================================================

//=====================================================================================
template <class T>
T BitManager<T>::usedBitsMap() const {return m_use;}
//=====================================================================================


//=====================================================================================
template <class T>
inline BOOL BitManager<T>::allocate(UI8 l, BitSlot& s)
{ 
	// are there enough bits free ? 
	if (l>nbBitsFree()) return FALSE; 
	// current position, shifted mask and max position 
	UI8 p = 0;
	T ma = m_mask[(l-1)*m_size];
	UI8 lim = m_size-l;
	// shifts the mask while some bits set under the mask and not at the end 
	while((m_use & ma)&&(p<lim)) {p++; ma = ma << 1;}
	// no l contiguous bits 
	if (m_use & ma) return FALSE;
	// slot allocation
	m_use = m_use | ma;
	m_n += l;
	// BitSlot construction 
	s = BitSlot(p,l);
	// succesful
	return TRUE; 
}
//=====================================================================================


//=====================================================================================
template <class T>
inline void BitManager<T>::free(BitSlot s)
{ 
	// unsets the bits of the slot
	m_use = m_use & negmask(s); 
	// updates used bits count 
	m_n -= s.getLength(); 
}
//=====================================================================================


//=====================================================================================
template <class T>
inline T BitManager<T>::mask(BitSlot s)  {return m_mask[s.getPosition()+s.getN()*m_size];}
//=====================================================================================

//=====================================================================================
template <class T>
inline T BitManager<T>::negmask(BitSlot s)  {return ~mask(s);}
//=====================================================================================

//=====================================================================================
template <class T>
inline void BitManager<T>::set(T& f, BitSlot s)  
{ f = f | mask(s); }
//=====================================================================================

//=====================================================================================
template <class T>
inline void BitManager<T>::set(T& f, BitSlot s, T v)  
{ f = (f & negmask(s))|((v << s.getN()) & mask(s)); }
//=====================================================================================

//=====================================================================================
template <class T>
inline void BitManager<T>::unset(T& f, BitSlot s)  
{ f = f & negmask(s); }
//=====================================================================================

//=====================================================================================
template <class T>
inline void BitManager<T>::set(T& f)  
{ f = mask(BitSlot(0,m_size)); }
//=====================================================================================

//=====================================================================================
template <class T>
inline void BitManager<T>::unset(T& f)  
{ f = 0; }
//=====================================================================================

//=====================================================================================
template <class T>
inline T BitManager<T>::get(T f, BitSlot s)  
{ return (f & mask(s))>>s.getN(); }
//=====================================================================================

//=====================================================================================
template <class T>
inline void BitManager<T>::bnot(T& f, BitSlot s)  
{ f = f ^ mask(s); }
//=====================================================================================

//=====================================================================================
template <class T>
inline void BitManager<T>::band(T& f, BitSlot s, T v)  
{ f = f & ((v << s.getN())|negmask(s)); }
//=====================================================================================

//=====================================================================================
template <class T>
inline void BitManager<T>::bor(T& f, BitSlot s, T v)  
{ f = f | ((v << s.getN())&mask(s)); }
//=====================================================================================

//=====================================================================================
template <class T>
inline void BitManager<T>::mset(T& f, T Mask)  
{ f = f | Mask;}
//=====================================================================================

//=====================================================================================
template <class T>
inline void BitManager<T>::munset(T& f, T Mask)  
{ f = f & ~Mask;}
//=====================================================================================

//=====================================================================================
template <class T>
void BitManager<T>::compute_mask()
{
	// masks of j+1 bits 
	for (UI8 j=0;j<m_size;j++) {
		// mask of position 0 
		m_mask[j*m_size] = ~((~0) << (j+1));
		// shifted masks 
		for (UI8 i=1;i<m_size;i++) {
			m_mask[i+j*m_size] = m_mask[i-1+j*m_size] << 1;
		}
	}
	m_must_compute_mask = FALSE;
}
//=====================================================================================

//=====================================================================================
// EOF
//=====================================================================================


