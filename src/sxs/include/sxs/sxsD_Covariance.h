/* 

 SxS : A C++ implementation of the scale climbing algorithm for multiscale image segmentation.
 © 2004 Laurent Guigues (laurent.guigues@ign.fr)

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
#ifndef __SXS_D_COVARIANCE__
#define __SXS_D_COVARIANCE__
//===========================================================
#include <lgl/lglMatrix.h>

#include "sxs/sxsScaleClimbingUser.h"
//===========================================================
// namespace	: sxs
// classes		: 
//	D_Covariance_User
//	D_Covariance_NodeAttribute
//	D_Covariance_EdgeAttribute
/*! \file 
 \brief Classes implementing fidelity energies (D) based on a variance/covariance matrix within the regions.
 */
//===========================================================
// author		: Laurent Guigues
// history		: 
//	25/07/04		creation
//===========================================================


namespace sxs {

	//===========================================================
	/// Minimal graph node attribution when using the energy class D_Covariance
	class D_Covariance_NodeAttribute: public ScaleClimbingNodeAttribute {
	public:
		//===========================================================
		/// ctor
		D_Covariance_NodeAttribute() :
				m_Dcov_n(0), m_Dcov_M1(dimension(), 0), m_Dcov_M2(dimension(), dimension(), 0) {
		}
		//===========================================================

		//===========================================================
		/// D_Covariance_NodeAttribute : Returns a ref on the dimension of the space (static).
		/// MUST BE INITIALIZED BEFORE ANY ATTRIBUTE CONSTRUCTION !
		static lgl::DimensionType& dimension() {
			static lgl::DimensionType d = 1;
			return d;
		}
		//===========================================================
		/// D_Covariance_NodeAttribute : Returns the number of observations attached to the node
		lgl::I32 number_of_observations() const {
			return m_Dcov_n;
		}
		/// D_Covariance_NodeAttribute : Returns a ref on the number of observations attached to the node
		lgl::I32& number_of_observations() {
			return m_Dcov_n;
		}
		//===========================================================

		//===========================================================
		void addObservation(const lgl::Vector<lgl::F32>& o) {
			m_Dcov_n++;
			m_Dcov_M1 += o;
			for (int i = 0; i < dimension(); ++i) {
				for (int j = 0; j < dimension(); ++j) {
					m_Dcov_M2(i, j) += o[i] * o[j];
				}
			}
		}
		//===========================================================

		//===========================================================
		/// D_Covariance_NodeAttribute : Returns a ref on the vector of the sums of observations
		/// (unormalized order 1 moments).
		lgl::Vector<lgl::F32>& sum() {
			return m_Dcov_M1;
		}
		/// D_Covariance_NodeAttribute : Returns a const ref on the vector of the sums of observations
		/// (unormalized order 1 moments).
		const lgl::Vector<lgl::F32>& sum() const {
			return m_Dcov_M1;
		}
		//===========================================================
		/// D_Covariance_NodeAttribute : Returns a ref on the matrix of the sums of squares of observations
		/// (unormalized/uncentered order 2 moments).
		lgl::Matrix<lgl::F32>& sum_of_squares() {
			return m_Dcov_M2;
		}
		/// D_Covariance_NodeAttribute : Returns a const ref on the matrix of the sums of squares of observations
		/// (unormalized/uncentered order 2 moments).
		const lgl::Matrix<lgl::F32>& sum_of_squares() const {
			return m_Dcov_M2;
		}
		//===========================================================

	protected:
		//===========================================================
		/// D_Covariance_NodeAttribute : number of observations
		lgl::I32 m_Dcov_n;
		/// D_Covariance_NodeAttribute : Order one moments
		lgl::Vector<lgl::F32> m_Dcov_M1;
		/// D_Covariance_NodeAttribute : Order two moments
		lgl::Matrix<lgl::F32> m_Dcov_M2;
		//===========================================================

	};
	//===========================================================
	// EO D_Covariance_NodeAttribute
	//===========================================================

	//===========================================================
	/// Minimal graph edge attribution when using the energy class D_Covariance
	class D_Covariance_EdgeAttribute: public ScaleClimbingEdgeAttribute {
	public:
		/// ctor
		D_Covariance_EdgeAttribute() {
		}
	};
	//===========================================================
	// EO D_Covariance_EdgeAttribute
	//===========================================================

	//===========================================================
	/// ImageScaleClimbingUser implementing a fidelity Energy (D) based on a variance/covariance matrix of image values within regions.
	///
	/// \note The observations are NOT initialised which means that the user must do it
	/// (@see ImageD_Covariance)
	class D_Covariance_User: public virtual ImageScaleClimbingUser {
	public:
		/// Ctor
		D_Covariance_User() {
		}

		//===========================================================
		// Energy evaluation callbacks
		//===========================================================
		/// Returns the fidelity energy value of a Node
		virtual lgl::F32 SCCB_D_eval(Node n) {
			D_Covariance_NodeAttribute* a = dynamic_cast<D_Covariance_NodeAttribute*>(n.getAttribute());
			return m_D_eval(a->sum(), a->sum_of_squares(), a->number_of_observations());
		}
		/// Returns the fidelity  energy value of an Edge, that is of the union of its two end Nodes
		virtual lgl::F32 SCCB_D_eval(Edge e) {
			D_Covariance_NodeAttribute* fa = dynamic_cast<D_Covariance_NodeAttribute*>(e.getInitialNode().getAttribute());
			D_Covariance_NodeAttribute* ta = dynamic_cast<D_Covariance_NodeAttribute*>(e.getFinalNode().getAttribute());
			return m_D_eval(fa->sum() + ta->sum(), fa->sum_of_squares() + ta->sum_of_squares(),
					fa->number_of_observations() + ta->number_of_observations());
		}
		//===========================================================

		//===========================================================
		/// Called at the insertion of an ImageSite in a Node (called in buildGraph()).
		///
		/// The user can at this point fill the Node's attribute with custom values
		virtual void SCCB_D_addSiteToNode(Node n, const lgl::ImageSite& s) {
		}
		//===========================================================

		//===========================================================
		// Graph management / energy update related callback
		//===========================================================
		// Complexity Energy (C) updates dedicated callbacks
		//===========================================================
		/// Called before the contraction of Edge e (beginning of contract(e)).
		///
		/// Its Node from() will survive and to() will die.
		/// The user can hence update the attributes of from() in order to
		/// reflect its merging with to().
		virtual void SCCB_D_onEdgeContraction(Edge e) {
			D_Covariance_NodeAttribute* fa = dynamic_cast<D_Covariance_NodeAttribute*>(e.getInitialNode().getAttribute());
			D_Covariance_NodeAttribute* ta = dynamic_cast<D_Covariance_NodeAttribute*>(e.getFinalNode().getAttribute());
			fa->sum() += ta->sum();
			fa->sum_of_squares() += ta->sum_of_squares();
			fa->number_of_observations() += ta->number_of_observations();
		}
		//===========================================================

	protected:
		///
		lgl::F32 m_D_eval(const lgl::Vector<lgl::F32>& M1,
				const lgl::Matrix<lgl::F32>& M2, lgl::I32 N) const {
			// N*Trace(Cov)
			lgl::F32 NTr = 0;
			//lglLOG("m_D_eval "<<M1.size()<<" "<<N<<" = ");
			for (unsigned int i = 0; i < M1.size(); i++) {
				//	lglLOG(M1(i)/N<<" ");
				NTr += M2(i, i) - M1(i) * M1(i) / N;
			}
			//lglLOG(" : "<<NTr<<std::endl);
			return sqrt(N * NTr);
		}
	};
//===========================================================
// EO D_Covariance
//===========================================================

}
//===========================================================
// EO namespace sxs
//===========================================================

#endif

