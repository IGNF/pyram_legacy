/*
 *  unit.h
 *  ign-all
 *
 *  Created by Nicolas Bellaiche on 29/12/10.
 *  Copyright 2010 Institut Geographique National - France. All rights reserved.
 *
 */

#ifndef _IGN_NUMERIC_UNIT_UNIT_H_
#define _IGN_NUMERIC_UNIT_UNIT_H_

#include <limits>

#define _USE_MATH_DEFINES
#include <math.h>

//uniquement pour les flux de debug...
#include <iostream>
#include <cmath>

#include <ignConfig.h>
//#include <ign/ublas.h>


namespace ign
{
	namespace numeric
	{
		namespace unit
		{
			/// \brief convertit un angle en radians en degres
			template < typename T_REAL >
			T_REAL rad2deg( T_REAL const& rad ){
				return rad*180.0*M_1_PI;
			}

			/// \brief convertit un angle en degre en radian
			template < typename T_REAL >
			T_REAL deg2rad( T_REAL const& deg ){
                return deg*M_PI/180.0;
			}

			/// \brief convertit un angle en degre en grades
			template < typename T_REAL >
			T_REAL deg2grad( T_REAL const& deg ){
				return deg*100./90.;
			}

			/// \brief convertit un angle en grades en degres
			template < typename T_REAL >
			T_REAL grad2deg( T_REAL const& grad ){
				return grad*90./100.;
			}

			/// \brief convertit un angle en grades en radians
			template < typename T_REAL >
			T_REAL grad2rad( T_REAL const& grad ){
				return grad/200.0*M_PI;
			}

			/// \brief convertit un angle en radians en grades
			template < typename T_REAL >
			T_REAL rad2grad( T_REAL const& rad ){
				return rad*200.0*M_1_PI;
			}

			/// \brief convertit un angle en degres decimaux en DMS
			template < typename T_REAL >
			T_REAL deg2dms( T_REAL const& deg ){
				if (deg<0.) return -deg2dms(-deg);
				double d = floor(deg);
				double m = floor((deg-d)*60.);
				double s = (deg-d-m/60.)*3600.;
				//std::cout<<"dms: "<<(int)d<<" "<<(int)m<<" "<<s<<std::endl;
				return d+0.01*m+0.0001*s;
			}

			/// \brief convertit un angle en DMS en degres decimaux
			template < typename T_REAL >
			T_REAL dms2deg( T_REAL const& dms ){
				if (dms<0.) return -dms2deg(-dms);
				double d = std::floor(dms);
				double m =  std::floor((dms - d)*1.e2);
				double s =  ((dms - d - 1e-2*m)*1.e4);
				//problemes d'arrondi...
				if (std::fabs(s-100.)<1.e-4)
				{
					m+=1.;
					s=0.;
				}
				//std::cout<<"dms2deg("<<dms<<") = "<<(int)d<<" "<<(int)m<<" "<<s<<std::endl;

				return d + m/60.+s/3600.;
			}
		}
	}
}
#endif

