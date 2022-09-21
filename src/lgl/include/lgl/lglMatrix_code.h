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
	\brief Code of template classes of lglMatrix.h.
*/

template <class T>
Matrix<T>::Matrix (Matrix<T> const & m) : Array2D<T>(m.size()) 
{
	iterator it = this->begin();
	const_iterator itm = m.begin();
	for (; it != this->end(); ++it, ++itm) *it = *itm;
}

template <class T>
Matrix<T> & Matrix<T>::operator = (T d) 
{
	for (iterator it=this->begin(); it!=this->end(); ++it) (*it) = d;
	return (*this) ;
}

template <class T>
Matrix<T> Matrix<T>::operator * (T d) const
{
	Matrix<T> m (this->size()) ;
	const_iterator it = this->begin();
	iterator itm = m.begin(); 
	for (; it!=this->end(); ++it, ++itm) *itm = (*it) * d;
	return (m) ;
}

template <class T>
Matrix<T> Matrix<T>::operator / (T d) const
{
	Matrix<T> m (this->GetTaille().x, this->GetTaille().y) ;
	const_iterator it=this->begin(), fin = this->end();
	iterator itm=m.begin();
	for (; it != fin; ++it, ++itm) *itm = (*it) / d;
	return (m) ;
}

template <class T>
void Matrix<T>::operator *= (T d) 
{
	for (iterator it=this->begin(); it!=this->end(); ++it) (*it) *= d;
}

template <class T>
void Matrix<T>::operator /= (T d) 
{
	for (iterator it=this->begin(); it!=this->end(); ++it) (*it) /= d;
}

template <class T>
Matrix<T> & Matrix<T>::operator = ( const Matrix<T> & m ) 
{
	resize(m.size());
	iterator it = this->begin();
	const_iterator itm = m.begin();
	for (; it!=this->end(); ++it, ++itm) *it = *itm;
	return (*this) ;
}

template <class T>
Matrix<T> Matrix<T>::operator + (Matrix<T> const & m) const 
{
	if (this->size()!=m.size()) lglERROR("lglMatrix<T>::operator+ : adding matrices of different sizes");
	Matrix<T> p(this->size()) ;
	iterator itp = p.begin();
	const_iterator  it=this->begin(), itm=m.begin();
	for (; it!=this->end(); ++it, ++itm, ++itp) *itp = *it + *itm;
	return (p) ;
} 

template <class T>
Matrix<T> Matrix<T>::operator - (Matrix<T> const & m) const 
{
	if (this->size()!=m.size()) lglERROR("lglMatrix<T>::operator- : adding matrices of different sizes");
	Matrix<T> p(this->size()) ;
	iterator itp = p.begin();
	const_iterator  it=this->begin(), itm=m.begin();
	for (; it!=this->end(); ++it, ++itm, ++itp) *itp = *it - *itm;
	return (p) ;
} 
 
template <class T>
Matrix<T> &  Matrix<T>::operator += (Matrix<T> const & m)
{	
	if (this->size()!=m.size()) lglERROR("lglMatrix<T>::operator+= : adding matrices of different sizes");
	iterator  it=this->begin();
	const_iterator itm=m.begin();
	for (; it!=this->end(); ++it, ++itm) *it += *itm;
	return *this;
}

template <class T>
Matrix<T> &  Matrix<T>::operator -= (Matrix<T> const & m)
{  
	if (this->size()!=m.size()) lglERROR("lglMatrix<T>::operator-= : adding matrices of different sizes");
	iterator  it=this->begin();
	const_iterator itm=m.begin();
	for (; it!=this->end(); ++it, ++itm) *it -= *itm;
	return *this;
}


template <class T>
Matrix<T> Matrix<T>::operator * (Matrix<T> const & m) const 
{
	int nl = this->size(0);
	int nc = this->size(1);
	if  (nc != m.size(0)) lglERROR("lglMatrix<T>::operator* : multiplying matrices of incompatible sizes");

	int ncs =  m.size(1);
	Matrix<T> p(nl,ncs) ;
	int i,j,k ;
	for (i=0; i<nl; ++i)
		for (j=0; j<ncs;++j) {
			T &a = p(i,j) ;
			a=0 ; 
			for (k=0; k<nc; ++k) 
				a += (*this)(i,k)*m(k,j) ;
		}
	return (p) ;
}


template <class T>
Matrix<T> Matrix<T>::transpose(void) const 
{
	int nl = this->size(0), nc = this->size(1) ;
	Matrix<T> p(nc,nl);
	for (int i=0; i<nl; ++i)
		for (int j=0; j<nc; ++j) {
			p(j,i)=(*this)(i,j);
		}
	return(p);
}

template <class T>
typename Matrix<T>::_Vector Matrix<T>::operator * (const _Vector& t) const 
{
	_Vector res(this->size(0)) ;
	for (int i=0; i<res.size(); ++i) {
		res(i) = 0;
		for (int j=0; j<t.size(); ++j) res(i) += t(j)*(*this)(i,j) ;
	}
	return (res) ;
}


//=====================================================================================
// Eigen vectors/values decomposition based on jacobi rotations 
//=====================================================================================
#define ROTATE(a,i,j,k,l) \
	g=a(i,j);h=a(k,l);a(i,j)=g-s*(h+g*tau);\
	a(k,l)=h+s*(g-h*tau);

template <class T>
void Matrix<T>::eigen_decomposition( Matrix& v, _Vector& d) const 
{
	Matrix a(*this);
	int j,iq,ip,i,n;
	double tresh,theta,tau,t,sm,s,h,g,c;
	std::vector<F32> b;
	if (this->size(0)!=this->size(1)){ 
		lglERROR("lgl::Matrix<T>::eigen_decomposition : non square matrix");
	}
	v.resize(this->size());
	n=this->size(0);
	b.resize(n);
	d.resize(n);
	std::vector<double> z(n);
	for (ip=0;ip<n;ip++) {
		for (iq=0;iq<n;iq++) v(ip,iq)=0.0;
		v(ip,ip)=1.0;
	}

	for (ip=0;ip<n;ip++) {
		b[ip]=d[ip]=a(ip,ip);
		z[ip]=0;
	}
	int nrot=0;
	for (i=1;i<=50;i++) {
		sm=0.0;
		for (ip=0;ip<n-1;ip++) {
			for (iq=ip+1;iq<n;iq++)
				sm += fabs(a(ip,iq));
		}
		if (sm == 0.0) {
			return;
		}
		if (i < 4)
			tresh=0.2*sm/(n*n);
		else
			tresh=0.0;
		for (ip=0;ip<n-1;ip++) {
			for (iq=ip+1;iq<n;iq++) {
				g=100.0*fabs(a(ip,iq));
				if (i > 4 && fabs(d[ip])+g == fabs(d[ip])
					&& fabs(d[iq])+g == fabs(d[iq]))
					a(ip,iq)=0.0;
				else if (fabs(a(ip,iq)) > tresh) {
					h=d[iq]-d[ip];
					if (fabs(h)+g == fabs(h))
						t=(a(ip,iq))/h;
					else {
						theta=0.5*h/(a(ip,iq));
						t=1.0/(fabs(theta)+sqrt(1.0+theta*theta));
						if (theta < 0.0) t = -t;
					}
					c=1.0/sqrt(1+t*t);
					s=t*c;
					tau=s/(1.0+c);
					h=t*a(ip,iq);
					z[ip] -= h;
					z[iq] += h;
					d[ip] -= h;
					d[iq] += h;
					a(ip,iq)=0.0;
					for (j=0;j<=ip-1;j++) {
						ROTATE(a,j,ip,j,iq)
					}
					for (j=ip+1;j<=iq-1;j++) {
						ROTATE(a,ip,j,j,iq)
					}
					for (j=iq+1;j<n;j++) {
						ROTATE(a,ip,j,iq,j)
					}
					for (j=0;j<n;j++) {
						ROTATE(v,j,ip,j,iq)
					}
					++nrot;
				}
			}
		}
		for (ip=0;ip<n;ip++) {
			b[ip] += z[ip];
			d[ip]=b[ip];
			z[ip]=0;
		}
	}
	lglERROR("lgl::Matrix<T>::eigen_decomposition : too much iterations");
}

#undef ROTATE

/*
bool Matrix<T>::EstDans(TPoint2D<int> const pt)
{
  if((pt.x < nrows()) &&
     (pt.y < ncols()) &&
     (pt.x >= 0) && (pt.y >= 0))
    return true;
  else
    return false;
}

bool Matrix<T>::EstDans(int const li, int const col)
{
  if((li < nrows()) &&
     (col < ncols()) &&
     (li >= 0) && (col >= 0))
    return true;
  else
    return false;

}



Matrix<T> Matrix<T>::Inverse (double epsilon) const 
{
  Matrix<T> tmp = (*this), v (taille.x, 1) ;
  gaussj (tmp, v, epsilon) ;
  return (tmp) ;
}

void Matrix<T>::InverseEnPlace (double epsilon)
{
  Matrix<T> v (taille.x, 1) ;
  gaussj (*this, v, epsilon) ;
}


Matrix<T> Matrix<T>::Cholevsky() const
{
  Matrix<T> tmp = (*this);
  TTableau1D < double > p(taille.x);
  choldc(tmp,p);
  int i,j;
  for (i=0; i<taille.x; ++i) 
    {
      tmp(i,i)=p[i];
      for (j=i+1; j< taille.x;++j) tmp(i,j)=0;
    }
  return (tmp);
}

void Matrix<T>::ResoudCholevsky(TTableau1D < double > const & B, TTableau1D < double > & X) const
{
  Matrix<T> tmp=(*this);
  TTableau1D < double > p(taille.x);
  choldc(tmp,p);
  cholsl(tmp,p,B,X);
}

void Matrix<T>::ResoudCholevskyEnPlace(TTableau1D < double > const & B, TTableau1D < double > & X)
{
  TTableau1D < double > p(taille.x);
  choldc(*this,p);
  cholsl(*this,p,B,X);
}

void Matrix<T>::ResoudSVD(TTableau1D < double > const & B, TTableau1D < double > & X) const
{
  Matrix<T> tmp = (*this);
  int m = tmp.GetTaille().x;
  int n = tmp.GetTaille().y;
  Matrix<T> U(m, n), Diag(n, n), V(n,n);
  svdcmp(tmp, U, Diag, V);
  svdsmalltozero(Diag);
  svbksb(U,Diag,V,B,X);
}

void Matrix<T>::ResoudSVDEnPlace(TTableau1D < double > const & B, TTableau1D < double > & X)
{
  int m = GetTaille().x;
  int n = GetTaille().y;
  Matrix<T> U(m, n), Diag(n, n), V(n,n);
  svdcmp(*this, U, Diag, V);
  svdsmalltozero(Diag);
  svbksb(U,Diag,V,B,X);
}



Matrix<T> Matrix<T>33ProduitVectoriel (const TPoint3D<double> & p) {
	Matrix<T> n(3,3) ;
	n=0;
	n(0,1) = -p.z;
	n(0,2) =  p.y;
	n(1,0) =  p.z;
	n(1,2) = -p.x;
	n(2,0) = -p.y;
	n(2,1) =  p.x;
	return (n) ;
}
*/

/*
tampon4D Matrix<T>::operator * ( RP2D p) const {
	tampon4D pp (3) ;
	pp.v[0]=p.x ;
	pp.v[1]=p.y ;
	pp.v[2]=p.t ;
	return operator * (pp) ;
}

TPoint3D<double> Matrix<T>::operator * (TPoint3D<double> const & t) const{
	if (taille.x!=3 || taille.y!=3) throw Erreur("Matrix<T> et TPoint3D<double> de dimension incompatible\n");
	TPoint3D<double> res(0,0,0);
	for (int i=0;i<3;++i){
		res.x+=(*this)(0,i)*t[i];
		res.y+=(*this)(1,i)*t[i];
		res.z+=(*this)(2,i)*t[i];

	}
	return res;
}

Point Matrix<T>::operator * (const Point& t) const 
{
	if (taille.x!=2 || taille.y!=2) throw Erreur("Matrix<T> et TPoint2D<double> de dimension incompatible\n");
	TPoint2D<double> res(0,0);
	for (int i=0;i<2;++i){
		res.x+=(*this)(0,i)*t[i];
		res.y+=(*this)(1,i)*t[i];
	}
	return res;
}
*/

/*
Matrix<T> Matrix<T>Identite (int t ) {
  Matrix<T> n(t,t) ;
  n=0;
  int i ;
  for (i=0; i<t;++i) n(i,i)=1;
  return (n) ;
}

Matrix<T> Matrix<T>33RotationAxe (const TPoint3D<double>& p, const double &alpha) {
  Matrix<T> m=Matrix<T>33ProduitVectoriel (p) ;
  Matrix<T> t=Matrix<T>Identite(3) + m*sin(alpha) + m*m*(1-cos(alpha));
  return t ;
}
*/
/*
Matrix<T> Matrix<T>44Translation (RP3D p) {
	Matrix<T> m(4,4) ;
	RP3D q = p ;
	q.normalise( );
	m=0 ;
	m[0][0] = m[1][1] = m[2][2] = m[3][3] = p.t ;
	m[0][3] = p.x ;
	m[1][3] = p.y ;
	m[2][3] = p.z ;
	return (m) ;
}

Matrix<T> Matrix<T>44RotationAxe (TPoint3D<double> axe, TPoint3D<double> pointAxe, double alpha) {
	Matrix<T> mTrans = Matrix<T>44Translation (pointAxe) ;
	TPoint3D<double> pointAxeOpp (-pointAxe.x, -pointAxe.y, -pointAxe.z) ;
	Matrix<T> mTransInv = Matrix<T>44Translation (pointAxeOpp) ;
	Matrix<T> mRot33 = Matrix<T>33RotationAxe (axe,  alpha) ;
	Matrix<T> mRot44(4,4) ;
	mRot44 = 0 ;
	mRot44[3][3]= 1 ;
	int i, j ;
	for (i=0; i<3; ++i)
	for (j=0; j<3; ++j) mRot44[i][j]=mRot33[i][j] ;
   return mTrans*mRot44*mTransInv ;
}
*/

// Codes LUMat, LURes et LUInv fournis par Christian Thom.
// Ils ne sont censes fonctionner que sur les Matrix<T>s carrees.
/*int LUMat( Matrix<T> &  A, double valzero ) {
	int n = A.iGetTaille () ;
    register int k, j, i, flg=0;
    register double x;
    for( i=0; i<n; i++ ) {
        for( j=i; j<n; j++ ){
            x = A[i][j];
            for( k=0; k<i; k++ )    x -= A[i][k]*A[k][j];
            if( i==j ){
                if( x <= 0 ) {
					flg --;
                    SORTIEMESSAGE ( "Pivot " << i<< "<=0 : " << x << "\n"); 
                    A[i][i] = valzero;
                } else  A[i][i] = sqrt( x );
            } else A[i][j] = A[j][i] = x/A[i][i];
        }
    }
	return( flg );
}
*/
/*      Resolution apres decomposition. A sort de la fonction precedente
*/
/*
void LURes( Matrix<T> & A, TCNRTableau1D < double > & d)
{
	int n = A.iGetTaille() ;
    register double x;
    register int j, i;*/
/*      D'abord descente        */
    /*for( i=0; i<n; i++ ){
        x = d[i];
        for( j=0; j<i; j++ ) x -= d[j] * A[i][j];
        d[i] = x/A[i][i];
    }*/
/*      Ensuite remontee        */
    /*for( i=n-1; i>=0; i-- ){
        x = d[i];
        for( j=i+1; j<n; j++ )  x -= d[j] * A[i][j];
        d[i] = x/A[i][i];
    }
}*/

/*      LU inversion de Matrix<T>     */
/*
void LUInv( Matrix<T> & A, Matrix<T> & B)
{
	int n = A.iGetTaille() ;
    int i;
    LUMat (A, n) ;
    B = 0.0 ;
    for( i=0; i<n; i++ ) {
        B[i][i] = 1.0 ;
        LURes( A, B[i]);
    }
}
  */  
