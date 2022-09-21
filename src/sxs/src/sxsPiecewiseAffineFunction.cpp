/* 

 SxS : A C++ implementation of the scale climbing algorithm for multiscale image segmentation.
 � 2004 Laurent Guigues (laurent.guigues@ign.fr)

 This program is free software; you can use, modify and/or redistribute it
 under the terms of the CeCILL license, which is a French law-Compatible and
 GNU GPL-Compatible free software license published by CEA, CNRS and INRIA.

 As a counterpart to the access to the source code and rights to copy,
 modify and redistribute granted by the license, users are provided only
 with a limited warranty and the software's author, the holder of the
 economic rights, and the successive licensors have only limited liability.
 See the CeCILL license for more details.

 You should have received a copy of the CeCILL license along with this software
 (files CeCILL_LICENCE_V1-fr.txt (french) and CeCILL_LICENSE_V1-en.txt (english)).
 If not visit http://www.cecill.info.
 The fact that you are presently reading this means that you have had
 knowledge of the CeCILL license and that you accept its terms.

 */
/*! \file 
 \brief Code of the methods of the class PiecewiseAffineFunction.
 */

#include <sxs/sxsPiecewiseAffineFunction.h>
#undef M_PI
#include <limits>


namespace sxs {

//==============================================================================
	lgl::F32 PiecewiseAffineFunction::operator ()(lgl::F32 x) const {
		if (size() < 1)
			return 0;
		const_iterator i = begin();
		for (++i; (i != end()) && ((*i).x() < x); ++i)
			;
		i--;
		return ((*i).y() + (*i).s() * (x - (*i).x()));
	}
//==============================================================================

//==============================================================================
	void PiecewiseAffineFunction::operator +=(const PiecewiseAffineFunction& f) {
		reverse_iterator i = rbegin(), ei = rend();
		const_reverse_iterator j = f.rbegin(), ej = f.rend();

		PiecewiseAffineFunction res;

		while ((i != ei) && (j != ej) && (res.size() < maxNboPieces())) {
			if ((*i).x() >= (*j).x()) {
				AffinePiece p(*i);
				p.y() += (*j).y() + (*j).s() * ((*i).x() - (*j).x());
				p.s() += (*j).s();
				res.push_front(p);
				if ((*i).x() == (*j).x())
					++j;
				++i;
			} else {
				AffinePiece p(*j);
				p.y() += (*i).y() + (*i).s() * ((*j).x() - (*i).x());
				p.s() += (*i).s();
				res.push_front(p);
				++j;
			}
		}
		if (res.front().x() > 0) {
			res.front().y() -= res.front().s() * res.front().x();
			res.front().x() = 0;
		}
		*this = res;
	}
//====================================================================================

//====================================================================================
	lgl::BOOL PiecewiseAffineFunction::infimum(const AffinePiece& m, lgl::F32& xi) {
		//std::cout << "PAF::infimum( "<<*this<<","<<m<<")"<<std::endl;

//	iterator i=end(),ei=begin();
//	--i; 
//	--ei;

		reverse_iterator i = rbegin(), ei = rend();
		iterator ii = end(); //A verifier...

		// m's slope must be lower than the last piece's slope
		if (m.s() > (*i).s())
			return false;

		// special case of equal slope
		if (m.s() == (*i).s()) {
			lgl::F32 y = m.y() + m.s() * ((*i).x() - m.x());
			if (y > (*i).y()) {
				xi = (std::numeric_limits<lgl::F32>::max)();
				return true;
			} else if (y == (*i).y()) {
				xi = (*i).x();
				return true;
			} else {
				++i;
				--ii;
			}
		}
		for (; i != ei; ++i, --ii) {
			xi = (m.x() * m.s() - (*i).x() * (*i).s() - (m.y() - (*i).y())) / (m.s() - (*i).s());
			if (xi > (*i).x())
				break;
		}

		//(*this).erase(++i,end());
		(*this).erase(ii, end());

		push_back(AffinePiece(xi, m.y() + m.s() * (xi - m.x()), m.s()));

		//std::cout << "xi   : "<<xi<<std::endl;
		//std::cout << "POST : "<<*this<<std::endl;
		return true;
	}
//==============================================================================

//==============================================================================
// I/O
//==============================================================================

//==============================================================================
	lgl::BOOL PiecewiseAffineFunction::readBinary(std::ifstream& f) {
		/*
			Int4 N;
			fic.read((char*)&N,sizeof(Int4));
			Int4 i;
			m_p.Flush();
			for (i=0;i<N;i++) {
			Float8 x,y,z;
			fic.read((char*)&x,sizeof(Float8));
			fic.read((char*)&y,sizeof(Float8));
			fic.read((char*)&z,sizeof(Float8));
			Morceau m(x,y,z);
			m_p.Add(m);
			}
			*/
		return true;
	}
//==============================================================================

//==============================================================================
	lgl::BOOL PiecewiseAffineFunction::writeBinary(std::ofstream& f) {
		/*
			Int4 N = m_p.size();
			fic.write((char*)&N,sizeof(Int4));
			Int4 i;
			for (i=0;i<NbMorceaux();i++) {
			fic.write((char*)&m_p[i].x,sizeof(Float8));
			fic.write((char*)&m_p[i].y,sizeof(Float8));
			fic.write((char*)&m_p[i].z,sizeof(Float8));
			}
			*/
		return true;
	}
//==============================================================================

}
