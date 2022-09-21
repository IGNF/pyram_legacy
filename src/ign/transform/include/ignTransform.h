#ifndef _IGN_TRANSFORM_TRANSFORM_H_
#define _IGN_TRANSFORM_TRANSFORM_H_

#include <ignConfig.h>

#include <ignMathVectorT.h>
#include <ignMathVector3T.h>
#include <ignMathBoxNT.h>
#include <ignUblas.h>

#include <ignTransformSystem.h>

namespace ign {
	/// \brief regroupe les transformations applicables sur les geometries
	/// ( et les radiometries, gerees pour l'instant sous forme (x, y, z)=(col, lig, rad)
	namespace transform {
		
		/// \brief represente une transformation point a point entre deux systemes,
		///
		/// les pointeurs vers system1 et system2 peuvent etre partages, 
		/// donc ils doivent etre geres par l'appelant
		///
		
		class IGN_TRANSFORM_API Transform {
			friend class GridTransform;
			friend class MultiTransform;
		private:
			//no default constructor allowed as we have to specify the inherited type of systems
			Transform();
		public:
			/// \brief constructeur
			Transform(SystemPtr const& sys1, SystemPtr const& sys2);
			/// \brief destructeur
			virtual								~Transform();
			
			/// \brief indique si la transformation directe existe
			virtual bool						hasDirect() const = 0;
			
			/// \brief indique si la transformation inverse existe
			virtual bool						hasInverse() const = 0;
			
            /// \brief applique une transformation sur un vecteur de double, la taille du vecteur informe de la dimension,
			/// les vecteurs ne contiennent pas de NaN
			void								direct( const transform::Vector& pt1, transform::Vector& pt2 ) const;
			/// \brief applique la transformation inverse si elle xiste
			void								inverse( const transform::Vector& pt1, transform::Vector& pt2) const;
			
			/// \brief applique une transformation sur un vecteur de points 3D (surchargeable si pb de perf en point a point)
			void								direct( std::vector<transform::Vector*> & points )const;
			/// \brief applique la transformation inverse si elle existe (hasInverse) sur un tableau de points
			void								inverse( std::vector<transform::Vector*> & points )const;
			
			/// \brief renvoie la verticale locale (du repere sys1), (0, 0, 1) par defaut, 
			/// la direction de visee lcoale pour une transfo image
			virtual math::Vec3d					localVertical(const math::Vec3d& p) const;
			/// \brief renvoie le code du systeme de vertical (demande GeoVIew pour gerer les epipolaires notamment)
			std::string							verticalSystem() const;
			/// \brief mete a jour le code du systeme de vertical
			void								verticalSystem(const std::string& systeVert) ;		
			
			/// \brief renvoie l'identifiant unique du systeme 1
			std::string							getSystem1Id() const;
			/// \brief renvoie l'identifiant unique du systeme 2
			std::string							getSystem2Id() const;
			
			/// \brief renvoie le systeme1
			const SystemPtr	&					getSystem1()const;
			/// \brief renvoie le systeme2
			const SystemPtr	&					getSystem2()const;
						
			/// \brief renvoie le systeme a partir de la chaine System::toString() s'il existe, NULL sinon
			const SystemRegistry&				getSystemRegistry() const;
	
			
			///\brief reprojette une bbox. 
			///@warning ATTENTION avec les types non signes avec des Transform qui renvoient des valeurs negatives...
			template < typename T, size_t N , typename T2, size_t N2>
			void								directBox( const math::BoxNT<T, N> & box1, math::BoxNT<T2, N2> & box2 ) const;
			///\brief reprojette une bbox. 
			///@warning ATTENTION avec les types non signes avec des Transform qui renvoient des valeurs negatives...
			template < typename T, size_t N , typename T2, size_t N2>
			void								inverseBox( const math::BoxNT<T, N> & box1, math::BoxNT<T2, N2> & box2 ) const;
			
			/// \brief verification du type d'une classe  derivee (const)
			template < typename T >
			bool										is() const 
			{	
				return (typeid(*this)==typeid(T));
			}
			
			/// \brief down cast sur une classe  derivee (const)
			template < typename T >
			T const &									as() const 
			{	
				T const * cast = dynamic_cast< T const * >( this );
				if ( cast == NULL ) {IGN_THROW_EXCEPTION("[Transform::as] bad cast");	}
				return *cast;
			}
			/// \brief down cast sur une classe  derivee
			template < typename T >
			T  &										as()  
			{	
				T  * cast = dynamic_cast< T * >( this );
				if ( cast == NULL ) {IGN_THROW_EXCEPTION("[Transform::as] bad cast");	}
				return *cast;
			}
			
		private:
			///\brief systeme de depart de la transfo dans le sens direct
			SystemPtr									_system1;
			///\brief systeme de depart de la transfo dans le sens inverse
			SystemPtr									_system2;
			
		protected:
			///\brief systeme de vertical
			std::string									_verticalSystem;

			/// \brief renvoie le systeme1
			SystemPtr									getSystem1();
			/// \brief renvoie le systeme2
			SystemPtr									getSystem2();
			
			/// \brief met a jour le systeme1
			void 										setSystem1(SystemPtr);
			/// \brief met a jour le systeme2
			void 										setSystem2(SystemPtr);
			
			/// \brief applique une transformation sur un vecteur de double, la taille du vecteur informe de la dimension,
			/// les vecteurs ne contiennent pas de NaN
			virtual void								_system1ToSystem2( const transform::Vector& pt1, transform::Vector& pt2 ) const  = 0 ;
			/// \brief applique la transformation inverse si elle xiste
			virtual void								_system2ToSystem1( const transform::Vector& pt1, transform::Vector& pt2) const = 0 ;
			/// \brief applique une transformation sur un vecteur de points 3D (surchargeable si pb de perf en point a point)
			/// @warning: l'unite resultante est necessairement celle du systeme en sortie et non celle des points en entree
			/// en effet, il ya des transformations avec des unites en sortie incompatibles avec les unites en entree (XYZ -> LPH par exemple)
			virtual void								_system1ToSystem2( std::vector<Vector*> & points )const;
			/// \brief applique la transformation inverse si elle existe (hasInverse) sur un tableau de points
			/// @warning: l'unite resultante est necessairement celle du systeme en sortie et non celle des points en entree
			/// en effet, il ya des transformations avec des unites en sortie incompatibles avec les unites en entree (XYZ -> LPH par exemple)
			virtual void								_system2ToSystem1( std::vector<Vector*> & points )const;
		};
	}
}

///
///
///
namespace ign {
	namespace transform {
		template < typename T, size_t N , typename T2, size_t N2>
		void		Transform::			directBox( const math::BoxNT<T, N> & box1, math::BoxNT<T2, N2> & box2 ) const
		{
			Vector v1(N), v2(std::max(N,N2));
			for (size_t i = 0; i<(size_t)std::pow(2.,(int)N); ++i)
			{
				int k(0);
				for (; k < (int)N; ++k) v1[k] = box1.minN(k);
				int cpt = 0;
				k = static_cast<int>(i);
				//on decompose en base 2
				while (k>=1)
				{
					if (k%2 == 1) v1[cpt] = box1.maxN(cpt);
					cpt++;
					k/=2;
				}
				
				direct(v1, v2);
				
				if ( i == 0)	
				{
					for (k = 0 ; k < (int)N2; ++k) box2.setBounds(k, v2[k], v2[k]);
				}
				else
				{
					for (k = 0 ; k < (int)N2; ++k) 
						box2.bounds(k).expandToInclude(v2[k]);
				}
			}
		}
		///\brief reprojette une bbox. 
		///@warning ATTENTION avec les types non signes avec des Transform qui renvoient des valeurs negatives...
		template < typename T, size_t N, typename T2, size_t N2>
		void		Transform::			inverseBox( const math::BoxNT<T, N> & box1, math::BoxNT<T2, N2> & box2 ) const
		{
			Vector v1(N), v2(std::max(N,N2));
			for (size_t i = 0; i<(size_t)std::pow(2.,(int)N); ++i)
			{
				int k = 0;
				for (; k < (int)N; ++k) v1[k] = box1.minN(k);
				int cpt = 0;
				k = static_cast<int>(i);
				//on decompose en base 2
				while (k>=1)
				{
					if (k%2 == 1) v1[cpt] = box1.maxN(cpt);
					cpt++;
					k/=2;
				}
				inverse(v1, v2);
				
				if ( i == 0)
				{
					for (k = 0 ; k < (int)N2; ++k) 
						box2.setBounds(k, v2[k], v2[k]);
				}
				else
				{
					for (k = 0 ; k < (int)N2; ++k) 
						box2.bounds(k).expandToInclude(v2[k]);
				}
			}
		}
	}
}
		
#endif
