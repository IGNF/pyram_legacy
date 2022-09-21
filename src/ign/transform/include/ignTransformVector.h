#ifndef _IGN_TRANSFORM_VECTOR_H_
#define _IGN_TRANSFORM_VECTOR_H_

#include <ignConfig.h>
#include <ignUblas.h>

#include <ignMathVector2T.h>
#include <ignMathVector3T.h>

#include <ignNumericSysUnit.h>

#define _IGN_USING_TRANSFORM_UNIT_
namespace ign {
	/// \brief regroupe les transformations applicables sur les geometries
	/// ( et les radiometries, gerees pour l'instant sous forme (x, y, z)=(col, lig, rad)
	namespace transform {
#ifndef _IGN_USING_TRANSFORM_UNIT_
		typedef ign::ublas::vector<double> Vector;
#else
		class Vector
		{
		public:
			/// \brief constructeur
			Vector(size_t n = 2, const ign::numeric::unit::SysUnitPtr& sysUnit = ign::numeric::unit::SysUnit::Null);
			/// \brief constructeur de copie
			Vector(const Vector &other);
			/// \brief constructeur a partir d'un VectorT
			template < typename T, size_t N, typename Derived >
			Vector(const ign::math::VectorBaseT<T,N, Derived> &other);
			/// \brief constructeur a partir de deux doubles
			Vector(double x, double y);		
			/// \brief constructeur a partir de trois doubles
			Vector(double x, double y, double z);	
			/// \brief constructeur a partir d'un ublas::vector
			template < typename T>
			Vector(ign::ublas::vector<T> const& other);
			/// \brief constructeur a partir d'un std::vector
			template < typename T>
			Vector(std::vector<T> const& other);
			/// \brief destructeur
			virtual												~Vector();
			
			/// \brief operateur d'affectation
			Vector &											operator = (const Vector &other);	
			/// \brief 
			Vector &											operator -= (const Vector &other);	
			/// \brief 
			Vector &											operator += (const Vector &other);	
			
			/// \brief ajoute terme a terme
			Vector												operator +( Vector const & other ) const;	
			/// \brief soustrait terme a terme
			Vector												operator -( Vector const & other ) const;
			/// \brief operateur de comparaison
			bool                                                operator == (const Vector& other) const;

			///\brief 
			inline double&										x() {return _data[0];}
			///\brief 
			inline double&										y() {return _data[1];}
			///\brief 
			inline double&										z() {return _data[2];}
			
			///\brief 
			inline const double&								x() const{return _data[0];}
			///\brief 
			inline const double&								y() const{return _data[1];}
			///\brief 
			inline const double&								z() const{return _data[2];}
			/// \brief operateur [] (CONST)			
			inline double const&								operator [] ( size_t const& n ) const	{return _data[n];}
			/// \brief operateur []
			inline double&										operator [] ( size_t const& n )			{return _data[n];}
			/// \brief operateur ()(CONST)
			inline double const&								operator () ( size_t const& n ) const	{return _data(n);}
			/// \brief operateur ()
			inline double&										operator () ( size_t const& n )			{return _data(n);}
			/// \brief cast en Vec3d
			inline operator										math::Vec3d() const {	return math::Vec3d(_data[0], _data[1], _data[2]);	}
			/// \brief cast en Vec2d
			inline operator										math::Vec2d() const {	return math::Vec2d(_data[0], _data[1]);	}
			/// \brief cast en boost vector
			inline operator										ign::ublas::vector<double>() const { return _data;	}
			/// \brief cast en VectorT
			template < typename T, size_t N, typename Derived  >
			inline operator										math::VectorBaseT<T,N, Derived>() const { math::VectorBaseT<T,N,Derived> v; 	for (size_t k = 0; k < N; ++k) v[k] = (*this)[k]; return v;}
			/// \brief cast en boost vector
			template < typename T >
			inline operator										ign::ublas::vector<T>() const {ign::ublas::vector<T> v(_data.size()); 	for (size_t k = 0; k < _data.size(); ++k) v[k] = (*this)[k]; return v;}
			/// \brief cast en std vector
			template < typename T >
			inline operator										std::vector<T>() const {std::vector<T> v; 	for (size_t k = 0; k < _data.size(); ++k) v.push_back((*this)[k]); return v;}
			/// \brief 
			inline size_t										size() const{return _data.size();}
			/// \brief 
			inline void											resize(size_t n) { if (_data.size()!=n) _data.resize(n);}
			/// \brief renvoie les donnees
			ign::ublas::vector<double>&							data() ;
			/// \brief renvoie les donnees (CONST)
			const ign::ublas::vector<double>&					data() const;
			/// \brief renvoie le systeme d'unites
			/// renvoie NULL si non defini
			numeric::unit::SysUnitPtr							sysUnit() ;
			/// \brief renvoie le systeme d'unites  (CONST)
			/// renvoie NULL si non defini
			const numeric::unit::SysUnitPtr						sysUnit() const;
			/// \brief met a jour le systeme d'unites (CONST)
			void												setSysUnit(const numeric::unit::SysUnitPtr&);
		private:
			/// \brief data
			ign::ublas::vector<double>							_data;
			
			/// \brief le systeme d'unites
			numeric::unit::SysUnitPtr							_sysUnit;
		};
		
		///
		///
		///
		std::ostream& operator << ( std::ostream& s, Vector const& v );
#endif
	}
}

#ifdef _IGN_USING_TRANSFORM_UNIT_
namespace ign
{
	namespace transform
	{
		///
		///
		///
		template < typename T, size_t N, typename Derived >
		Vector::Vector(const ign::math::VectorBaseT<T,N, Derived> &other) 
		{
			_data.resize(N); 
			_sysUnit.reset();
			for (size_t k = 0; k < N; ++k) 
				_data[k] = other[k];
		}
		
		///
		///
		///
		template < typename T>
		Vector::Vector(ign::ublas::vector<T> const& other)
		:_data(other),
		_sysUnit(NULL){;}
		
		///
		///
		///
		template < typename T>
		Vector::Vector(std::vector<T> const& other) 
		{
			_data.resize(other.size()); 
			_sysUnit.reset();
			for (size_t k = 0; k < other.size(); ++k)
				_data[k] = other[k];
		}
	}
}
#endif

#endif
