#ifndef _IGN_MATH_VECTOR3T_H_
#define _IGN_MATH_VECTOR3T_H_

#include <ignMathVectorBaseT.h>
#include <ignNumeric.h>

namespace ign 
	{
	namespace math 
		{

		/// \brief represente un vecteur d'un espace a trois dimensions
		///
		/// reecrite pour profiter de boost::ublas sur les produits matriciels
		template < typename T >
		class Vector3T : public VectorBaseT< T, 3, Vector3T< T > >
		{
		public:
			/// \brief represent base vector type
			typedef	VectorBaseT< T, 3, Vector3T< T > > 		base_vector;

			typedef typename base_vector::size_type 		size_type;
			typedef typename base_vector::difference_type 	difference_type;
			typedef typename base_vector::value_type		value_type;
			typedef typename base_vector::const_reference	const_reference;
			typedef typename base_vector::reference			reference;
			typedef typename base_vector::const_pointer		const_pointer;
			typedef typename base_vector::pointer			pointer;
			typedef typename base_vector::const_iterator	const_iterator;
			typedef typename base_vector::iterator			iterator;

			/// \brief definit la dimension
			const static size_t								dimension = 3 ;


			/// \brief build a vector on the origin
			Vector3T():
				base_vector()
			{
				(*this)[0] = (T)0;
				(*this)[1] = (T)0;
				(*this)[2] = (T)0;
			}

			/// \brief build a vector with x,y coordinates
			Vector3T ( T const& x, T const& y, T const& z ):
				base_vector()
			{
				(*this)[0] = x;
				(*this)[1] = y;
				(*this)[2] = z;
			}

			/// \brief copy constructor from a uBLAS vector expression
			template <class R>
			Vector3T (const boost::numeric::ublas::vector_expression< R >& r):
				base_vector(r)
			{

			}

			/// \brief assignment from a uBLAS vector expression
			template <class R>
			void operator=(const boost::numeric::ublas::vector_expression<R>& r)
			{
				base_vector::operator=(r);
			}
			/// \brief assignment from an other vector
			template <class R>
			void operator=(const base_vector& r)
			{
				base_vector::operator=(r);
			}

			/// \brief cross product with an other vector
			Vector3T	cross( Vector3T const& other ) const
			{
				return Vector3T(
					(*this)[1]*other[2]-(*this)[2]*other[1],
					(*this)[2]*other[0]-(*this)[0]*other[2],
					(*this)[0]*other[1]-(*this)[1]*other[0]
				);
			}

			/// \brief scalar product with an other vector
			double	scalProd( Vector3T const& other ) const
			{
                return 	( (*this)[0]*other[0]+(*this)[1]*other[1] + (*this)[2]*other[2] );
			}

			/// \brief x const accessor
			inline T const	&	x() const { return (*this)[0]; }
			/// \brief y const accessor
			inline T const	&	y() const { return (*this)[1]; }
			/// \brief z const accessor
			inline T const	&	z() const { return (*this)[2]; }

			/// \brief x accessor
			inline T &			x() { return (*this)[0]; }
			/// \brief y accessor
			inline T &			y() { return (*this)[1]; }
			/// \brief z const accessor
			inline T &			z() { return (*this)[2]; }

			/// \brief conversion to double vector
			Vector3T< double >	toVectorNd() const
			{
				return Vector3T< double >(
					numeric::Numeric< value_type >::ToDouble( x() ),
					numeric::Numeric< value_type >::ToDouble( y() ),
					numeric::Numeric< value_type >::ToDouble( z() )
				);
			}
			
			/// \brief verifie l'initialisation des coordonnees x et y (ie differents de NaN)
			bool isEmpty() const
			{
				return ( numeric::Numeric< double >::IsNaN(x()) || numeric::Numeric< double >::IsNaN(y()) );

			}
			/// \brief verifie l'initialisation des coordonnees x, y et z (ie differents de NaN)
			bool is3d() const
			{
				return ( ! isEmpty() ) && ( ! numeric::Numeric< double >::IsNaN(z()) );
			}

			/// \brief create UX vector
			static Vector3T		UX() { return Vector3T< T >( (T)1, (T)0, (T)0 ); }
			/// \brief create UY vector
			static Vector3T		UY() { return Vector3T< T >( (T)0, (T)1, (T)0 ); }
			/// \brief create UZ vector
			static Vector3T		UZ() { return Vector3T< T >( (T)0, (T)0, (T)1 ); }

		};

		/// \brief vecteur de double a 3 dimension
		typedef Vector3T<double> 	Vec3d;
		/// \brief vecteur de float a 3 dimension
		typedef Vector3T<float> 	Vec3f;
		
		/// \brief norme en 2d 
		template < typename T >
		double norm2d(const Vector3T<T>& P)
		{
			return sqrt(P.x()*P.x() + P.y()*P.y());
		}
	}
}

#endif


