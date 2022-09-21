#ifndef __SDI_RADIOMETRY_RADIOTRANSFORM_H__
#define __SDI_RADIOMETRY_RADIOTRANSFORM_H__

#include <iostream>
#include <map>
#include <limits>
#include <stdio.h>

#include <ignMathVector3T.h>
#include <ignTransform.h>

#include <ignImageSystemRadio.h>

namespace ign
{
	namespace image
	{
		namespace radiometry 
		{
			///\brief classe de gestion des transformations radiometriques vituelle pure
			/// Seules les classes filles peuvent etre instanciees
			/// Pour permettre le chainage des transformations et utiliser les coordonnees image ou terrain lors de l'application des transformations,
			/// on stocke utilise des vector a 3+n dimension
			/// - les 3 premieres composantes representent des coordonnees (a priori col, lig, 0 ou x, y, z)
			/// - les n suivantes representent les canaux a traiter
			template <class Tin, class Tout> 
			class RadioTransform : public ign::transform::Transform
			{
			protected:
				/// rp : ne suffit pas, il me faut aussi des coordonnees image en entree
				virtual void	apply(double const & valIn, size_t const & nChannel, double & valOut)const=0 ;
				/// \brief
				virtual void	applyToPoint(double const & c, double const & l, double const & valIn, size_t const & nChannel , double & valOut)const = 0;
				
			public:
				///\brief constructeur par defaut
				RadioTransform (SystemRadioPtr const& sys1, SystemRadioPtr const& sys2);
				/// \brief destructeur
				virtual			~RadioTransform();
				/// \brief indique si la transformation directe existe
				bool			hasDirect () const ;
				/// \brief indique si la transformation inverse existe
				bool			hasInverse() const ;
				
			protected:
				/// \brief applique une transformation sur un point
				/// les Vec3d stockent respectivement en x, y et z les coordonnees en colonnes et lignes du pixel traite, et sa radiometrie
				virtual void	_system1ToSystem2 ( const ign::transform::Vector& pt1, ign::transform::Vector& pt2 ) const  ;
				/// \brief
				virtual void	_system2ToSystem1( const ign::transform::Vector& pt1, ign::transform::Vector& pt2 ) const  ;
							
			};
		}
	}
}

///
///
///

namespace ign
{
	namespace image
	{
		namespace radiometry 
		{
			template <class Tin, class Tout> 
			RadioTransform<Tin, Tout>::RadioTransform(SystemRadioPtr const&sys1, SystemRadioPtr const&sys2)
			: ign::transform::Transform(sys1, sys2)
			{
			}
			
			///
			///
			///			
			template <class Tin, class Tout> 
			RadioTransform<Tin, Tout> ::~RadioTransform() 
			{
				//std::cout<<"~RadioTransform"<<std::endl;
			}
			
			///
			///
			///			
			template <class Tin, class Tout> 
			bool	RadioTransform<Tin, Tout> ::hasDirect() const 
			{
				return true;
			}
			
			///
			///
			///
			template <class Tin, class Tout> 
			bool	RadioTransform<Tin, Tout> ::hasInverse() const 
			{
				return false;
			}
			
			///
			///
			///			
			template <class Tin, class Tout> 
			void	RadioTransform<Tin, Tout> ::_system1ToSystem2(const ign::transform::Vector &pt1, ign::transform::Vector& pt2 )const
			{
				//IGN_ASSERT(pt1.size() > 3 );
				//IGN_ASSERT(pt2.size()==pt1.size());
				if( pt1.size() <= 3) IGN_THROW_EXCEPTION("[RadioTransform<Tin, Tout> ::_system1ToSystem2] pt1.size() <= 3 ");
				if( pt2.size()!=pt1.size()) IGN_THROW_EXCEPTION("[RadioTransform<Tin, Tout> ::_system1ToSystem2] pt2.size()!=pt1.size() ");
				
				pt2[0] = pt1[0];
				pt2[1] = pt1[1];
				pt2[2] = pt1[2];
				for (size_t i = 3 ; i < pt1.size() ; i++)
				{
					double valOut;
					applyToPoint(pt1[0],pt1[1],pt1[i],i-3,valOut);
					pt2[i] = valOut;
				}
			}
			
			///
			///
			///
			template <class Tin, class Tout> 
			void	RadioTransform<Tin, Tout> ::_system2ToSystem1(const ign::transform::Vector &pt1, ign::transform::Vector& pt2 )const
			{
				IGN_ASSERT(false);
			}
		}
	}
}

#endif 
