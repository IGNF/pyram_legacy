#ifndef __COMPOSE_CHANNELS_OP_H__
#define __COMPOSE_CHANNELS_OP_H__

#include <ignTransform.h>
#include <ignTransformSystem.h>
#include <ignBufferImage.h>

#include <math.h>

namespace ign {
namespace image {

///
/// \brief Operateur pour combiner plusieurs canaux en un seul.
///
template <class Tin, class Tout>
class ComposeChannelOp
{
    public :
    static void Compute(BufferImage<Tin>const& imgIn, const ign::transform::Transform &transform, BufferImage<Tout>& imgOut) {

        imgOut.initialize(imgIn.numCols(), imgIn.numLines(), 1);
        ign::transform::Vector vIn(3 + imgIn.numBands());
        ign::transform::Vector vOut(3+ imgIn.numBands());

        for (size_t l = 0; l < imgIn.numLines(); ++l) {
            for (size_t c = 0; c < imgIn.numCols(); ++c) {

                vIn[0] = l;
                vIn[1] = c;
                vIn[2] = 0;//imgIn.numBands()
                //remplir le vIn ave les radiometries
                for (size_t b = 0; b < imgIn.numBands(); b++) {
                    vIn[3 + b] = imgIn(c, l, b);
                }
                transform.direct(vIn, vOut);
                imgOut(c,l, 0) = vOut[3];
            }
        }
    }
};

///
/// \brief Class abstraite de tranformation de plusieurs canaux en un seul.
///
template <class Tin, class Tout>
class ComposeChannelTransform : public ign::transform::Transform {

    public :
        ComposeChannelTransform():ign::transform::Transform(ign::transform::SystemUnknown::Instance(), ign::transform::SystemUnknown::Instance() ) {}
        bool            hasDirect() const{ return true; }
        bool            hasInverse() const{ return false; }

    protected:
        virtual void	_system1ToSystem2( const transform::Vector& pt1, transform::Vector& pt2 ) const = 0;
        void            apply(double const & valIn, size_t const & nChannel, double & valOut)const{ IGN_THROW_EXCEPTION("SumChannelTransform::apply(...) SHOULD NOT be called"); }
        void            applyToPoint(double const & c, double const & l, double const & valIn, size_t const & nChannel , double & valOut)const { IGN_THROW_EXCEPTION("SumChannelTransform::applyToPoint(...) SHOULD NOT be called"); }
        void			_system2ToSystem1( const transform::Vector& pt1, transform::Vector& pt2) const { IGN_THROW_EXCEPTION("SumChannelTransform::_system2ToSystem1() SHOUND NOT be called"); }
};

///
/// \brief La somme des canaux.
///
template <class Tin, class Tout>
class SumComposeChannelTransform : public ign::image::ComposeChannelTransform<Tin,Tout> {
public :
    SumComposeChannelTransform(): ComposeChannelTransform<Tin,Tout>() {}
protected:
 void 								_system1ToSystem2( const transform::Vector& pt1, transform::Vector& pt2 ) const;
};

///
/// \brief La somme des canaux divisee par le nombre de canaux.
///
template <class Tin, class Tout>
class SumPerNumBandComposeChannelTransform : public ign::image::ComposeChannelTransform<Tin,Tout> {
public :
    SumPerNumBandComposeChannelTransform(): ComposeChannelTransform<Tin,Tout>() {}
protected:
 void 								_system1ToSystem2( const transform::Vector& pt1, transform::Vector& pt2 ) const;
};

///
/// \brief La norme euclidienne des canaux, donc la racine de la somme des carres.
///
template <class Tin, class Tout>
class EuclidianNormComposeChannelTransform : public ign::image::ComposeChannelTransform<Tin,Tout> {
public :
    EuclidianNormComposeChannelTransform(): ComposeChannelTransform<Tin,Tout>() {}
protected:
 void 								_system1ToSystem2( const transform::Vector& pt1, transform::Vector& pt2 ) const;
};

///
/// \brief On garde la maximum des canaux. Bons resultats, quand on compose des gradients.
///
template <class Tin, class Tout>
class MaximumComposeChannelTransform : public ign::image::ComposeChannelTransform<Tin,Tout> {
public :
    MaximumComposeChannelTransform(): ComposeChannelTransform<Tin,Tout>() {}
protected:
 void 								_system1ToSystem2( const transform::Vector& pt1, transform::Vector& pt2 ) const;
};


///
/// IMPLEMENTATION DES METHODES TEMPLATES
/// On peut mettre ce code dans un autre fichier
///

///
///
///
template <class Tin, class Tout>
void	SumComposeChannelTransform<Tin,Tout>::_system1ToSystem2 ( const ign::transform::Vector& pt1, ign::transform::Vector& pt2 ) const
{
    if( pt1.size() <= 3) IGN_THROW_EXCEPTION("[SumChannelTransform<Tin, Tout> ::_system1ToSystem2] pt1.size() <= 3 ");
    if( pt2.size()!=pt1.size()) IGN_THROW_EXCEPTION("[SumChannelTransform<Tin, Tout> ::_system1ToSystem2] pt2.size()!=pt1.size() ");

    pt2[0] = pt1[0];
    pt2[1] = pt1[1];
    pt2[2] = pt1[2];

    Tout valOut = 0;
    for (size_t i = 3 ; i < pt1.size() ; i++)
    {
         valOut += pt1[i];
         pt2[i] = 0;
    }
    pt2[3] = valOut;
}

///
///
///
template <class Tin, class Tout>
void	SumPerNumBandComposeChannelTransform<Tin,Tout>::_system1ToSystem2 ( const ign::transform::Vector& pt1, ign::transform::Vector& pt2 ) const
{
    if( pt1.size() <= 3) IGN_THROW_EXCEPTION("[SumChannelTransform<Tin, Tout> ::_system1ToSystem2] pt1.size() <= 3 ");
    if( pt2.size()!=pt1.size()) IGN_THROW_EXCEPTION("[SumChannelTransform<Tin, Tout> ::_system1ToSystem2] pt2.size()!=pt1.size() ");

    pt2[0] = pt1[0];
    pt2[1] = pt1[1];
    pt2[2] = pt1[2];

    Tout valOut = 0;
    for (size_t i = 3 ; i < pt1.size() ; i++) {
        valOut += pt1[i];
        pt2[i] = 0;
    }
    pt2[3] = valOut / (pt1.size() -3);
}

///
///
///
template <class Tin, class Tout>
void	EuclidianNormComposeChannelTransform<Tin,Tout>::_system1ToSystem2 ( const ign::transform::Vector& pt1, ign::transform::Vector& pt2 ) const
{
    if( pt1.size() <= 3) IGN_THROW_EXCEPTION("[SumChannelTransform<Tin, Tout> ::_system1ToSystem2] pt1.size() <= 3 ");
    if( pt2.size()!=pt1.size()) IGN_THROW_EXCEPTION("[SumChannelTransform<Tin, Tout> ::_system1ToSystem2] pt2.size()!=pt1.size() ");

    pt2[0] = pt1[0];
    pt2[1] = pt1[1];
    pt2[2] = pt1[2];

    Tout valOut = 0;

    for (size_t i = 3 ; i < pt1.size() ; i++) {
        valOut += pt1[i] * pt1[i];
        pt2[i] = 0;
    }
    pt2[3] = sqrt(valOut);
}

///
///
///
template <class Tin, class Tout>
void	MaximumComposeChannelTransform<Tin,Tout>::_system1ToSystem2 ( const ign::transform::Vector& pt1, ign::transform::Vector& pt2 ) const
{
    if( pt1.size() <= 3) IGN_THROW_EXCEPTION("[SumChannelTransform<Tin, Tout> ::_system1ToSystem2] pt1.size() <= 3 ");
    if( pt2.size()!=pt1.size()) IGN_THROW_EXCEPTION("[SumChannelTransform<Tin, Tout> ::_system1ToSystem2] pt2.size()!=pt1.size() ");

    pt2[0] = pt1[0];
    pt2[1] = pt1[1];
    pt2[2] = pt1[2];

    Tout valOut = 0;
    for (size_t i = 3 ; i < pt1.size() ; i++) {
        Tout tmp = (Tout) pt1[i];
        if (tmp > valOut) {
            valOut = tmp;
        }
        pt2[i] = 0;
    }
    pt2[3] = sqrt(valOut);
}

}}

#endif //__COMPOSE_CHANNELS_OP_H__
