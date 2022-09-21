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
	\brief Codes of the templates classes defined in lglImageRam.h
*/
//==========================================================================================
// code of ImageRamT<T> methods, included by lglImageRam.h
//=========================================================================================

//==========================================================================================
template <class T>
BOOL ImageRamT<T>::build( const ImageSite& sz ) 
{
  if (sz==m_size) return TRUE;
	
  if (!free()) return FALSE;

  ImageSite mmax(sz);
  mmax(0)--;
  long dim = sz(0);
  for (int i=1; (i<4) && (sz(i)>0) ; ++i) { 
    dim *= sz(i);
    mmax(i)--;
  }

  m_data = new T[dim];

  if (!m_data) return FALSE;

  m_size = sz;
  m_data_end = m_data + m_offset(mmax) + 1;

  return TRUE;
} 
//==========================================================================================


//==========================================================================================
template <class T>
BOOL ImageRamT<T>::build( const ImageSite& sz, T v ) 
{
  if (sz==m_size) return TRUE;

  if (!free()) return FALSE;

  ImageSite mmax(sz);
  mmax(0)--;
  long dim = sz(0);
  for (int i=1; (i<4) && (sz(i)>0) ; ++i) { 
    dim *= sz(i);
    mmax(i)--;
  }

  m_data = new T[dim];

  if (!m_data) return FALSE;

  m_size = sz;
  m_data_end = m_data + m_offset(mmax) + 1;

  if (sizeof(T)==1) {
    int iv = (int)v;
    memset(m_data,iv,m_data_end-m_data);
  }
  else {
    for (iterator i=begin(); i!=end(); ++i) (*i) = v;
  }
  return TRUE;
} 
//==========================================================================================

//==========================================================================================
template <class T>
BOOL ImageRamT<T>::free() 
{	
  if (!m_data) return TRUE;
  delete[] m_data;
  m_data = m_data_end = NULL;
  m_size = ImageSite();
  return TRUE;
}
//==========================================================================================

//==========================================================================================
template <class T>
ImageRamT<T>::ImageRamT(const ImageRamT<T>& src)
  : m_size(), m_data(0), m_data_end(0) 
{
  build(src.m_size);
  memcpy(m_data,src.m_data,m_data_end-m_data);
}
//==========================================================================================



//==========================================================================================
template <class T>
void ImageRamT<T>::getSlice( const ImageSite& direction, int position, pImageRam& rout ) const
{
  if (size().dimension()!=4) return;
  ImageRamT<T>* out = dynamic_cast<ImageRamT<T>*>(rout);	
  if (!out) out = new ImageRamT<T>;
  rout = out;

  //	wxLogMessage("getSlice %d,%d,%d p=%d",direction(1),direction(2),direction(3),position);

  if (direction(1)!=0) {
    out->resize(ImageSite(size(0),size(2),size(3)));
    //		wxLogMessage ( "getxSlice : %d %d ", out.size(1),out.size(2));
    int i,j;
    for (i=0;i<size(3);i++) {
      for (j=0;j<size(2);j++) {
	T *ip = m_data + m_offset(0,position,j,i);
	T *op = out->m_data + out->m_offset(0,j,i);
	memcpy(op,ip,size(0)*sizeof(T));
      }
    }
  }
  else if (direction(2)!=0) {
    out->resize(ImageSite(size(0),size(1),size(3)));
    //		wxLogMessage ( "getySlice : %d %d ", out.size(1),out.size(2));
    int i;
    for (i=0;i<size(3);i++) {
      T *ip = m_data + m_offset(0,0,position,i);
      T *op = out->m_data + out->m_offset(0,0,i);
      memcpy(op,ip,size(0)*size(1)*sizeof(T));
    }
  }
  else if (direction(3)!=0) {
    out->resize(ImageSite(size(0),size(1),size(2)));
    //		wxLogMessage ( "getzSlice : %d %d ", out.size(1),out.size(2));
    T *ip = m_data + m_offset(0,0,0,position);
    T *op = out->m_data;
    memcpy(op,ip,size(0)*size(1)*size(2)*sizeof(T));
  }
  //	wxLogMessage("getCut OK");
}
//==========================================================================================

//==========================================================================================
template <class T>
void ImageRamT<T>::getAllSlices( const ImageSite& position, pImageRam& routx, pImageRam& routy, pImageRam& routz ) const
{
  if (size().dimension()!=4) return;
  ImageRamT<T>* outx = dynamic_cast<ImageRamT<T>*>(routx);	
  if (!outx) outx = new ImageRamT<T>;
  ImageRamT<T>* outy = dynamic_cast<ImageRamT<T>*>(routy);	
  if (!outy) outy = new ImageRamT<T>;
  ImageRamT<T>* outz = dynamic_cast<ImageRamT<T>*>(routz);	
  if (!outz) outz = new ImageRamT<T>;
  routx = outx;
  routy = outy;
  routz = outz;

  outx->resize(ImageSite(size(0),size(2),size(3)));
  outy->resize(ImageSite(size(0),size(1),size(3)));
  int i,j;
  for (i=0;i<size(3);i++) {
    for (j=0;j<size(2);j++) {
      T *ip = m_data + m_offset(0,position(1),j,i);
      T *op = outx->m_data + outx->m_offset(0,j,i);
      memcpy(op,ip,size(0));
    }
    T *ip = m_data + m_offset(0,0,position(2),i);
    T *op = outy->m_data + outy->m_offset(0,0,i);
    memcpy(op,ip,size(0)*size(1)*sizeof(T));
  }
  outz->resize(ImageSite(size(0),size(1),size(2)));
  T *ip = m_data + m_offset(0,0,0,position(3));
  T *op = outz->m_data;
  memcpy(op,ip,size(0)*size(1)*size(2)*sizeof(T));
}
//==========================================================================================
		




//==========================================================================================
template <class T>
BOOL ImageRamT<T>::rawRead( FILE* f, BOOL swap )
{
  // read the data 
  size_t n = numberOfValues(); 
  if ( fread( m_data, sizeof(T), n, f) != n ) return false;

  // swap endians ? 
  if ( swap && (sizeof(T)>1) ) {
    Machine::swapEndians(m_data,(long) n);
  }	
  return true;
}
//==========================================================================================
		


//==========================================================================================
template <class T>
BOOL ImageRamT<T>::rawWrite( FILE* f ) const 
{
  // write the data 
  size_t n = numberOfValues(); 
  if ( fwrite( m_data, sizeof(T), n, f) != n ) {
    return false;
  }
  return true;
}
//==========================================================================================



		
