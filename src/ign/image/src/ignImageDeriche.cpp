
#include <ignImageDeriche.h>

using namespace ign::image;

///
///
///
void ign::image::Laplacien(BufferImage<float> const & in, BufferImage<float> & out, float alpha)
{
    
    double exp_al=(double) exp((double)(-alpha));
	double exp_alcarre=exp_al*exp_al;
    
	double a1, a2, a3, a4, a5, a6, a7, a8, b1, b2, c1, c2;
	a1 = a5 = c1 = c2 = 1.;
	a2 = a4 = a6 = a8 = 0.;
	a3 = a7 = exp_al;
	b1 = exp_al;
	b2 = 0;
	
	FiltreRegressif(in, out, a1, a2, a3, a4, a5, a6, a7, a8, b1, b2, c1, c2);
    
	a1 = a4 = a5 = a8 = 0.;
	a2 = a3 = a6 = a7 = 1.;
	c1 = c2 = (1-exp_alcarre)/2;
	b1 = 2*exp_al;
	b2 = -exp_alcarre;
    
	BufferImage<float> r1;
	FiltreRegressif(in, r1, a1, a2, a3, a4, a5, a6, a7, a8, b1, b2, c1, c2);
	
	for (int i = 0; i<static_cast<int>(out.numCols()); ++i)
    {
		for (int j = 0; j<static_cast<int>(out.numLines()); ++j)
        {
			out(i,j) -= r1(i,j);
        }
    }
}

