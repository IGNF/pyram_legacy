#ifndef __IGN_IMAGE_ALGORITHM_DERICHE_H__
#define __IGN_IMAGE_ALGORITHM_DERICHE_H__

#include <ignBufferImage.h>

namespace ign
{
    namespace image
    {		
        /*
            Calcul de gradient et de laplacien
            Mise en souche partiel des fonctions deriche issues du Matis
        */
        
        /// \brief Filtre Regressif permettant de calculer gx,gy (g=gradient) en changeant les parametres
        ///        On peut egalement l'utiliser pour calculer le Laplacien
        ///
        ///        Fonction doublement templatee pour rester compatible avec les versions
        ///        precedentes matis du deriche qui utilisaient des images de float pour gx et gy
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
                                double c2);
        
        
        /// \brief calcul de Laplacien a partir du filtre regressif
        void Laplacien(BufferImage<float> const & in, BufferImage<float> & out, float alpha);
        
        /// \brief calcul de gx et de gy en changeant les parametres donnes au filtre auto-regressif
        ///
        ///        Fonction doublement templatee pour rester compatible avec les versions
        ///        precedentes matis du deriche qui utilisaient des images de float pour gx et gy
        template < class T, class U>
        void Deriche(BufferImage <T> const & in,
                        BufferImage <U> & gx,
                        BufferImage <U> & gy,
                        double alpha);
            
        /// \brief calcul du module de Deriche a partir de gx, gy
        template < class T, class U>
        void ModuleDeriche (BufferImage<T> const & gx, 
                            BufferImage<T> const & gy, 
                            BufferImage<U> & O);

	}
}

#include <ignImageDeriche_code.h>

#endif

