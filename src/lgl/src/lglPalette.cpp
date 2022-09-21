#include "lgl/lglPalette.h"
#include "lgl/lglRandom.h"

#if wxUSE_GUI

namespace lgl {

//======================================================================================
// PaletteMultilinear
//======================================================================================

//======================================================================================
	void PaletteMultilinear::set ( Type t )
	{
		m_map.clear();
		std::pair<double,RGBpoint> p;
		switch (t) {
			case gray :
			p.first = 0;
			p.second = RGBpoint(0,0,0);
			m_map.push_back(p);
			p.first = 1;
			p.second = RGBpoint(255,255,255);
			m_map.push_back(p);
			break;
			case red_blue :			/// 0-(255,0,0) 1-(0,0,255)
			p.first = 0;
			p.second = RGBpoint(255,0,0);
			m_map.push_back(p);
			p.first = 1;
			p.second = RGBpoint(0,0,255);
			m_map.push_back(p);
			break;
			case black_red_white :
			p.first = 0;
			p.second = RGBpoint(0,0,0);
			m_map.push_back(p);
			p.first = 0.5;
			p.second = RGBpoint(255,0,0);
			m_map.push_back(p);
			p.first = 1;
			p.second = RGBpoint(255,255,255);
			m_map.push_back(p);
			break;
			case black_blue_white :
			p.first = 0;
			p.second = RGBpoint(0,0,0);
			m_map.push_back(p);
			p.first = 0.5;
			p.second = RGBpoint(0,0,255);
			m_map.push_back(p);
			p.first = 1;
			p.second = RGBpoint(255,255,255);
			m_map.push_back(p);
			break;
			case black_yellow_white :
			p.first = 0;
			p.second = RGBpoint(0,0,0);
			m_map.push_back(p);
			p.first = 0.5;
			p.second = RGBpoint(255,255,0);
			m_map.push_back(p);
			p.first = 1;
			p.second = RGBpoint(255,255,255);
			m_map.push_back(p);
			break;
			case blue_white :/// 0-(0,0,255) 0.5-(0,255,255) 1-(255,255,255)
			p.first = 0;
			p.second = RGBpoint(0,0,255);
			m_map.push_back(p);
			p.first = 0.5;
			p.second = RGBpoint(0,255,255);
			m_map.push_back(p);
			p.first = 1;
			p.second = RGBpoint(255,255,255);
			m_map.push_back(p);
			break;
			case black_body :/// 0-(128,0,0) 0.25-(255,0,0) 0.5-(255,255,0) 1-(255,255,255)
			p.first = 0;
			p.second = RGBpoint(128,0,0);
			m_map.push_back(p);
			p.first = 0.25;
			p.second = RGBpoint(255,0,0);
			m_map.push_back(p);
			p.first = 0.5;
			p.second = RGBpoint(255,255,0);
			m_map.push_back(p);
			p.first = 1;
			p.second = RGBpoint(255,255,255);
			m_map.push_back(p);
			break;
			case spectrum :/// 0-(0,0,255) 0.25-(0,255,255) 0.5-(0,255,0) 0.75-(255,255,0) 1-(255,0,0)
			p.first = 0;
			p.second = RGBpoint(0,0,255);
			m_map.push_back(p);
			p.first = 0.25;
			p.second = RGBpoint(0,255,255);
			m_map.push_back(p);
			p.first = 0.5;
			p.second = RGBpoint(0,255,0);
			m_map.push_back(p);
			p.first = 0.75;
			p.second = RGBpoint(255,255,0);
			m_map.push_back(p);
			p.first = 1;
			p.second = RGBpoint(255,0,0);
			m_map.push_back(p);
			default :
			;
		}
	}
//======================================================================================

//======================================================================================
	void PaletteMultilinear::set ( Type t, BOOL negative, double vmin, double vmax )
	{
		m_vmin = vmin;
		m_vmax = vmax;
		m_neg = negative;
		set(t);
	}
//======================================================================================

//======================================================================================
	RGBpoint PaletteMultilinear::operator() ( const double& in ) const
	{
		if (m_map.size()<2) return RGBpoint(0,0,0);

		double x0, x1;
		double x = (in-m_vmin)/(m_vmax-m_vmin);
		if (x<0) x=0;
		else if (x>1) x=1;
		if (m_neg) x=1-x;
		RGBpoint y0,y1;
		std::vector< std::pair<double,RGBpoint> >::const_iterator i;
		for (i=m_map.begin(); i!=m_map.end(); ++i) {
			if ((*i).first>=x) break;
		}
		if (i==m_map.begin()) {
			x0 = (*i).first;
			y0 = (*i).second;
			++i;
			x1 = (*i).first;
			y1 = (*i).second;
		}
		else if (i==m_map.end()) {
			std::vector< std::pair<double,RGBpoint> >::const_reverse_iterator j=m_map.rbegin();
			x1 = (*j).first;
			y1 = (*j).second;
			++j;
			x0 = (*j).first;
			y0 = (*j).second;
		}
		else {
			x1 = (*i).first;
			y1 = (*i).second;
			--i;
			x0 = (*i).first;
			y0 = (*i).second;
		}
		double p = (x-x0)/(x1-x0);
		/// integer cast to avoid uchar truncations/wrap when (y1-y0) has negative coordinates
		unsigned char r = (unsigned char)((int)y0.r + ((int)y1.r - (int)y0.r)*p);
		unsigned char g = (unsigned char)((int)y0.g + ((int)y1.g - (int)y0.g)*p);
		unsigned char b = (unsigned char)((int)y0.b + ((int)y1.b - (int)y0.b)*p);
		return RGBpoint(r,g,b);
	}
//======================================================================================

//======================================================================================
	void PaletteLUT::setRandomPalette(int min, int max, int seed)
	{
		RandomInteger ri(seed,0,255);
		m_min = min;
		m_lut.resize(max-min+1);
		std::vector<RGBpoint>::iterator i;
		for (i=m_lut.begin();i!=m_lut.end();++i) {
			(*i).r = ri.get();
			(*i).g = ri.get();
			(*i).b = ri.get();
		}
	}
//======================================================================================

}

#endif /// #if wxUSE_GUI
//============================================
// EOF
//============================================
