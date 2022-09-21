#ifndef NUMEROTEREGION_H_
#define NUMEROTEREGION_H_

#include <set>
#include <map>
#include <iostream>
#include <boost/progress.hpp>

#include <ignBufferImage.h>
#include <ignException.h>

using namespace ign::image;

namespace ign {
	namespace image {         

		// Passage en numerotation contigue
		template<class T>
		void renumerotation(BufferImage<T> & img);

		template<class T>
		void renumerotationSpecific(BufferImage<T> & img, std::map<T,T> LUT);

		template<class T>
		void renumerotationSpecific2(BufferImage<T> & img, std::map<T, std::set< std::pair<size_t, T> >* > &LUT);

		template<class T> class T2D{
		protected:
			T _x,_y;
		public:
			T2D(const T x, const T y){_x=x; _y=y;}
			virtual ~T2D(){}
			const T & x() const{return _x;}
			const T & y() const{return _y;}
			const bool operator<(const T2D & pt) const{
				if(_x<pt.x()){return true;}
				if(_x>pt.x()){return false;}
				if(_y<pt.y()){return true;}
				if(_y>pt.y()){return false;}
				return false;
			}
		};

		//Cette version est plus gourmande en memoire que la precedente, car elle implique de recopier l'image, ce qui pourrait poser probleme avec de tres grandes images
		template <class T> inline void renumerotation_composante_connexe( int& labelmax, BufferImage<T> & imagelabel/*, std::vector<Region> & regions_classif*/, const bool ignore0=true, const int connexite=8 /*ou 4*/);

		typedef struct RgbColor
		{
			unsigned char r;
			unsigned char g;
			unsigned char b;
		} RgbColor;

		typedef struct HsvColor
		{
			unsigned char h;
			unsigned char s;
			unsigned char v;
		} HsvColor;

		RgbColor *HsvToRgb(HsvColor hsv);

		HsvColor RgbToHsv(RgbColor rgb);

		// Passage en numerotation contigue
		template<class T>
		BufferImage<unsigned char>* renumerotationOneChannelToRGB(BufferImage<T> & img);

		
		#include <ignImageNumRegion_code.h>

	}
}

#endif /* NUMEROTEREGION_H_ */
