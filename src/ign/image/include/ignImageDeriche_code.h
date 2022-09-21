
namespace ign
{
    namespace image
    {
        
        ///
        ///
        ///
        template < class T, class U>
        void FiltreRegressif(BufferImage < T > const & in,
                                BufferImage < U > & out,
                                double a1,
                                double a2,
                                double a3,
                                double a4,
                                double a5,
                                double a6,
                                double a7,
                                double a8,
                                double b1,
                                double b2,
                                double c1,
                                double c2)
        {
            bool verbose = false;
            
            if (verbose) std::cout << "[FiltreRegressif] begin " << std::endl;
            
            int nbre_col = static_cast<int>(in.numCols());
            int nbre_lig = static_cast<int>(in.numLines());
            int nbre_bands = static_cast<int>(in.numBands());
            
            if (verbose) std::cout << "[FiltreRegressif] stat " << nbre_col << " " << nbre_lig << " " << nbre_bands << std::endl;

            BufferImage <U> Y1 (nbre_col, nbre_lig, nbre_bands);
            BufferImage <U> Y2 (nbre_col, nbre_lig, nbre_bands);
            
            if (verbose) std::cout << "[FiltreRegressif] initilisation de out " << std::endl;
            if (!out.isValid()) out.initialize(nbre_col, nbre_lig, nbre_bands,0.) ;
            
            if (verbose) std::cout << "[FiltreRegressif] calcul... " << std::endl;

            for(int b = 0 ; b < nbre_bands ; ++b)
            {
                if (verbose) std::cout << "[FiltreRegressif] bande #" << b << std::endl;
                for (int l=0; l<nbre_lig; ++l)
                {
                    Y1(0,l,b)=0.0;
                    Y1(1,l,b)=0.0;
                    for (int c=2; c<nbre_col; ++c)
                    {
                        Y1.set(c,l,b,(U)(a1*(double)in(c,l,b)+a2*(double)in(c-1,l,b)+b1*Y1(c-1,l,b)+b2*Y1(c-2,l,b)));
                    }
                }
                for (int l=0; l<nbre_lig; ++l)
                {
                    Y2(nbre_col-1,l,b)=0.0;
                    Y2(nbre_col-2,l,b)=0.0;
                    for (int c=nbre_col-3; c>-1; --c)
                    {
                        Y2.set(c,l,b,(U)(a3*(double)in(c+1,l,b)+a4*(double)in(c+2,l,b)+b1*Y2(c+1,l,b)+b2*Y2(c+2,l,b)));
                    }
                }
            
                for (int l=0; l<nbre_lig; ++l)
                {

                    for (int c=0; c<nbre_col; ++c)
                    {
                        out.set(c,l,b,(U)(c1*(Y1(c,l,b) + Y2(c,l,b))));
                    }
                }
            
                for (int c=0; c<nbre_col; ++c)
                {
                    Y1(c,0,b)=0.0;
                    Y1(c,1,b)=0.0;
                    for (int l=2; l<nbre_lig; ++l)
                    {
                        Y1.set(c,l,b,(U)(a5*out(c,l,b)+a6*out(c,l-1,b)+b1*Y1(c,l-1,b)+b2*Y1(c,l-2,b)));
                    }
                }
                for (int c=0; c<nbre_col; ++c)
                {
                    Y2(c,nbre_lig-1,b)=0.0;
                    Y2(c,nbre_lig-2,b)=0.0;
                    for (int l=nbre_lig-3; l>-1; --l)
                    {
                        Y2.set(c,l,b,(U)(a7*out(c,l+1,b)+a8*out(c,l+2,b)+b1*Y2(c,l+1,b)+b2*Y2(c,l+2,b)));
                    }
                }
                for (int l=0; l<nbre_lig; ++l)
                {
                    for (int c=2; c<nbre_col; ++c)
                    {
                        out.set(c,l,b,(U)(c2*(Y1(c,l,b) + Y2(c,l,b))));
                    }
                }
            }
            
            if (verbose) std::cout << "[FiltreRegressif] end " << std::endl;
        }
        
        ///
        ///
        ///
        template < class T, class U>
        void Deriche(BufferImage< T > const & in,
                        BufferImage <U> & gx,
                        BufferImage <U> & gy,
                        double alpha)
        {
            double exp_al=(double) exp((double)(-alpha));
            double exp_alcarre=exp_al*exp_al;
            
            double k = (1-exp_al)*(1-exp_al)/(1+2*alpha*exp_al-exp_alcarre);
            double a1 = 0;
            double a2 = 1;
            double a3 = -1;
            double a4 = 0;
            double a5 = k;
            double a6 = k*exp_al*(alpha-1);
            double a7 = k*exp_al*(alpha+1);
            double a8 = -k*exp_alcarre;
            double b1 = 2*exp_al;
            double b2 = -exp_alcarre;
            double c1 = -(1-exp_al)*(1-exp_al);
            double c2 = 1;
            
            FiltreRegressif(in,gx,
                            a1,
                            a2,
                            a3,
                            a4,
                            a5,
                            a6,
                            a7,
                            a8,
                            b1,
                            b2,
                            c1,
                            c2);
            
            FiltreRegressif(in,gy,
                            a5,
                            a6,
                            a7,
                            a8,
                            a1,
                            a2,
                            a3,
                            a4,
                            b1,
                            b2,
                            c2,
                            c1);
        }
        
        ///
        ///
        ///
        template < class T, class U>
        void DericheSmoothing(BufferImage< T > const & in,
                        BufferImage <U> & out,
                        double alpha)
        {
            double exp_al=(double) exp((double)(-alpha));
            double exp_alcarre=exp_al*exp_al;
            
            double k = (1-exp_al)*(1-exp_al)/(1+2*alpha*exp_al-exp_alcarre);
            //  double a1 = 0;
            //   double a2 = 1;
            //  double a3 = -1;
            //   double a4 = 0;
            double a5 = k;
            double a6 = k*exp_al*(alpha-1);
            double a7 = k*exp_al*(alpha+1);
            double a8 = -k*exp_alcarre;
            double b1 = 2*exp_al;
            double b2 = -exp_alcarre;
            //   double c1 = -(1-exp_al)*(1-exp_al);
            double c2 = 1;

            
            FiltreRegressif(in,out,
                            a5,
                            a6,
                            a7,
                            a8,
                            a5,
                            a6,
                            a7,
                            a8,
                            b1,
                            b2,
                            c2,
                            c2);
        }

        ///
        ///
        ///
        template < class T, class U >
        void ModuleDeriche (
        BufferImage<T> const & dx, 
        BufferImage<T> const & dy, 
        BufferImage<U> & O) 
        {
            O.initialize(dx.numCols(), dx.numLines(), dx.numBands(), 0);
            double tmpx,tmpy;
            for(size_t b = 0 ; b < dx.numBands() ; ++b)
                {
                for(size_t i = 0; i < dx.numCols(); i++)
                    for (size_t j = 0; j < dx.numLines(); j++) {
                        tmpx = dx.get(i, j, b); tmpx*=tmpx;
                        tmpy = dy.get(i, j, b); tmpy*=tmpy;
                        O.set(i, j, b, (T)sqrt (tmpx+tmpy) );	
                    }
                }
        }

	}
}

