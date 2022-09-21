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
	\brief Code of IndexedHeap
*/
//============================================================================
template <class T, class CT, class IT>
std::ostream& operator << (std::ostream& s, const IndexedHeap<T,CT,IT>& t)
{
  s << "[";
  for (I32 i=0; i<t.size(); i++) s << t[i] << " "  ;
  s << "]";
  return s;
}
//============================================================================


//===========================================================
template <class T, class CT, class IT>
IndexedHeap<T,CT,IT>::IndexedHeap ( const CT& comp, const IT& index ) 
: m_c(&comp), m_i(&index) {}
//===========================================================


//===========================================================
template <class T, class CT, class IT>
void IndexedHeap<T,CT,IT>::set( const CT& comp ) 
{ m_c = &comp; }
//===========================================================

//===========================================================
template <class T, class CT, class IT>
void IndexedHeap<T,CT,IT>::set ( const IT& index ) 
{ m_i = &index; }
//===========================================================


//===========================================================
template <class T, class CT, class IT>
I32 IndexedHeap<T,CT,IT>::insert(T t)
{
  m_p.push_back(t);
  (*m_i)(t) = size()-1;
  return upsort(size()-1);
}
//===========================================================

//===========================================================
template <class T, class CT, class IT>
T& IndexedHeap<T,CT,IT>::top()
{
	lglASSERT( size() > 0)
	return m_p.front();
}
//===========================================================


//===========================================================
template <class T, class CT, class IT>
const T& IndexedHeap<T,CT,IT>::top() const
{
	lglASSERT( size() > 0)
	return m_p.front();
}
//===========================================================

//===========================================================
template <class T, class CT, class IT>
T IndexedHeap<T,CT,IT>::remove_top()
{
	lglASSERT( size() > 0 ) 
	T f(m_p[0]);
	(*m_i)(f) = -1;
	T last = m_p.back();
	m_p.pop_back();
	if (m_p.size()>0) {
		m_p[0] = last;
		(*m_i)(last) = 0;
		downsort(0);
	}
	return f;
}
//============================================================================



//============================================================================
template <class T, class CT, class IT>
T IndexedHeap<T,CT,IT>::remove(I32 n)
{
	lglASSERT ( (n>=0)&&(n<size()) ) 
	T f(m_p[n]);
	(*m_i)(f) = -1;
	T last = m_p.back();
	m_p.pop_back();
	int tmp=m_p.size();
	if (m_p.size()>0 && n<tmp) {
		m_p[n] = last;
		(*m_i)(last) = n;
		downsort(n);
	}
	return f;
}
//============================================================================


//============================================================================
template <class T, class CT, class IT>
void IndexedHeap<T,CT,IT>::clear()
{
	for (typename std::vector<T>::iterator i=m_p.begin(); i!=m_p.end(); ++i) { (*m_i)(*i)=-1; }
	m_p.clear();
}
//============================================================================


//============================================================================
template <class T, class CT, class IT>
I32  IndexedHeap<T,CT,IT>::father( I32  i) const
{
	return ((i-1)/2);
}
//============================================================================

//============================================================================
template <class T, class CT, class IT>
I32  IndexedHeap<T,CT,IT>::rightson( I32  i) const
{ 
	return (i*2+2);
}
//============================================================================

//============================================================================
template <class T, class CT, class IT>
I32  IndexedHeap<T,CT,IT>::leftson( I32  i) const
{ 
	return (i*2+1);
}
//============================================================================

//============================================================================
template <class T, class CT, class IT>
void IndexedHeap<T,CT,IT>::swap(I32 i, I32 j)
{
	T tmp = m_p[i];
	m_p[i] = m_p[j];
	m_p[j] = tmp;
	// update indices  
	(*m_i)(m_p[i]) = i;
	(*m_i)(m_p[j]) = j;
}
//============================================================================



//============================================================================
template <class T, class CT, class IT>
I32  IndexedHeap<T,CT,IT>::upsort(I32  i)
{
	//if (i==0) return i;
	I32  j = father(i);
	while ((i>0)&&(*m_c)(m_p[i],m_p[j])) {
		swap(i,j);
		i = j;
		j = father(j);
	}	
	return i;
}
//============================================================================


//============================================================================
template <class T, class CT, class IT>
I32  IndexedHeap<T,CT,IT>::downsort(I32  i)
{
	do {
		UI32  ls = leftson(i);
		if (ls<m_p.size()) {
			bool lc = ((*m_c)(m_p[i],m_p[ls]));
			UI32  rs = ls + 1;
			bool rc = true;
			if (rs<m_p.size()) rc = ((*m_c)(m_p[i],m_p[rs]));
			if  ( !lc ) { 
				if ( !rc ) { 
					if ((*m_c)(m_p[ls],m_p[rs])) { 
						swap(i,ls);
						i = ls;
					}
					else { 
						swap(i,rs);
						i = rs;
					}
				}
				else {
					swap(i,ls);
					i = ls;
				}
			}
			else if ( !rc ) { 
				swap(i,rs);
				i = rs;
			}
			else return i;
		} 
		else return i;
	}
	while (true);
	return i;
}
//============================================================================


//============================================================================
// EOF
//============================================================================
