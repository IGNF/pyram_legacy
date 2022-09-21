#include <ctime>
#include <cstdlib>
#include "lgl/lglRandom.h"


namespace lgl {

//======================================================================
	RandomInteger::RandomInteger(I32 ss, I32 pp, I32 qq) :
			s(ss), p(pp), q(qq - pp + 1), iff(0) {
		if (s < 0)
			init_on_clock(s);
	}
//======================================================================

//======================================================================
	I32 RandomInteger::get() {

		I32 dum = 0;
		I32 j;

		if (s < 0 || iff == 0) {
			iff = 1;
#ifdef __LINUX__
			maxran=RAND_MAX;
#endif
#ifdef __WINDOWS__
			UI16 i,k;
			i=2;
			do {
				k=i;
				i<<=1;
			}while (i>0);
			maxran= k;
#endif
			srand(s);
			for (j = 0; j < 97; j++)
				dum = rand();
			for (j = 0; j < 97; j++)
				v[j] = rand();
			y = rand();
		}

		j = I32(97.0 * (F32) y / maxran);
		if (j > 96)
			j = 96;
		if (j < 0)
			j = 0;
		y = v[j];
		v[j] = rand();
		F32 x = (F32) y / maxran;
		if (x == 1.)
			return p + q - 1;
		if (dum) {
		} // trick to avoid warning
		return p + ((I32) (x * q));
	}
//======================================================================

//======================================================================
	RandomFloat::RandomFloat(I32 ss, F32 pp, F32 qq) :
			s(ss), p(pp), q(qq - pp), iff(0) {
		if (s < 0)
			init_on_clock(s);
	}
//======================================================================

//======================================================================
	F32 RandomFloat::get() {

		I32 dum = 0;
		I32 j;

		if (s < 0 || iff == 0) {
			iff = 1;
#ifdef __LINUX__
			maxran=RAND_MAX;
#endif
#ifdef __WINDOWS__
			UI16 i,k;
			i=2;
			do {
				k=i;
				i<<=1;
			}while (i>0);
			maxran= k;
#endif
			srand(s);
			for (j = 0; j < 97; j++)
				dum = rand();
			for (j = 0; j < 97; j++)
				v[j] = rand();
			y = rand();
		}
		j = I32(97.0 * (F32) y / maxran);
		y = v[j];
		v[j] = rand();
		if (dum) {
		} // trick to avoid warning
		return p + q * (F32) y / maxran;
	}
//======================================================================

//======================================================================
	void RandomInteger::init_on_clock(I32 ss) {
		s = time(NULL) - ss;
	}
//======================================================================

//======================================================================
	void RandomFloat::init_on_clock(I32 ss) {
		s = time(NULL) - ss;
	}
//======================================================================

}
//======================================================================
// EO namespace lgl + EOF
//======================================================================
