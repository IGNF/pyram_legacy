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
#ifndef __sxsE_PolygonalBoundaryImageCovariance_h__
#define __sxsE_PolygonalBoundaryImageCovariance_h__
//===========================================================
// namespace	: sxs
//
/*! \file 
 \brief Classes implementing an energy base on polygonal boundary modeling (C) and image covariance (D)
 */
//===========================================================
// author		: Laurent Guigues
// history		: 
//	30/07/04		creation
//===========================================================
#include "sxs/sxsC_PolygonalBoundary.h"
#include "sxs/sxsC_PolygonalBoundaryMeasures.h"
#include "sxs/sxsD_ImageCovariance.h"
#if wxUSE_GUI
#include <wx/frame.h>
#include <wx/dcclient.h>
#endif

#define GMZOOM 7

namespace sxs {

	//=================================================================================
	/// \brief Graph nodes attributes for the PolygonalBoundaryImageCovariance energy
	class E_PolygonalBoundaryImageCovariance_NodeAttribute: public C_PolygonalBoundary_NodeAttribute, public D_ImageCovariance_NodeAttribute {
	};
	//==================================================================================

	//=================================================================================
	/// \brief Graph edges attributes for the PolygonalBoundaryImageCovariance energy
	class E_PolygonalBoundaryImageCovariance_EdgeAttribute: public C_PolygonalBoundary_EdgeAttribute, public D_ImageCovariance_EdgeAttribute {
	};
	//=================================================================================

	//=================================================================================
	/// \brief ScaleClimbingUser for the PolygonalBoundaryImageCovariance energy
	///
	class E_PolygonalBoundaryImageCovariance_User: public C_PolygonalBoundary_User, public D_ImageCovariance_User {

	public:

		/// Ctor
		E_PolygonalBoundaryImageCovariance_User() {
		}

		/// Returns the complexity energy value of a Node
		virtual lgl::F32 SCCB_C_eval(Node n) {
			//	  std::cout << "C_eval(Node) ";
			std::vector<std::vector<PolygonalBoundary> > bnd;
			getPolygonalBoundary(getScaleClimbing().getGraph(), n, bnd);
#if wxUSE_GUI
			if (m_graphical_mode)
			{
				//	    std::cout << "draw ";
				wxClientDC dc(m_frame);
				//	    dc.Clear();
				//	    dc.SetPen(*wxBLACK_PEN);
				//	    drawPolygonalBoundaries(getScaleClimbing().getGraph(),dc,3,false);
				dc.SetPen(*wxRED_PEN);
				drawPolygonalBoundaries(bnd,dc,GMZOOM,false);

				//	    std::cout << "ok"<<std::endl;
			}
#endif //#if wxUSE_GUI
			//	  std::cout << "ok"<<std::endl;
			lgl::F32 L = Length(bnd);
			//	  std::cout << "L ="<<L<<std::endl;

			return L;
		}
		/// Returns the complexity energy value of an Edge, that is of the union of its two end Nodes
		virtual lgl::F32 SCCB_C_eval(Edge n) {
			//	  std::cout << "C_eval(Edge) ";
			std::vector<std::vector<PolygonalBoundary> > bnd;
			getPolygonalBoundary(getScaleClimbing().getGraph(), n.getInitialNode(), n.getFinalNode(), bnd);

#if wxUSE_GUI
			if (m_graphical_mode) {
				//	    std::cout << "draw ";
				wxWindow * win = m_frame;
				wxClientDC dc(win);
				//	    dc.Clear();
				//   dc.SetPen(*wxBLACK_PEN);
				// drawPolygonalBoundaries(getScaleClimbing().getGraph(),dc,3,false);

				std::vector< std::vector<PolygonalBoundary> > bnd2;
				getPolygonalBoundary(getScaleClimbing().getGraph(), n.getInitialNode()/*, n.getFinalNode()*/, bnd2);
				dc.SetPen(*wxCYAN_PEN);
				drawPolygonalBoundaries(bnd2,dc,GMZOOM,false);

				std::vector< std::vector<PolygonalBoundary> > bnd3;
				getPolygonalBoundary(getScaleClimbing().getGraph(), n.getFinalNode(), bnd3);
				dc.SetPen(*wxGREEN_PEN);
				drawPolygonalBoundaries(bnd3,dc,GMZOOM,false);

				//	    wxStopWatch sw; while (sw.Time()<1000) {};
				dc.SetPen(*wxRED_PEN);
				drawPolygonalBoundaries(bnd,dc,GMZOOM,false);

				//	    sw.Start(0); while (sw.Time()<1000) {};

				//	    std::cout << "ok"<<std::endl;
			}
#endif //#if wxUSE_GUI
			//	  std::cout << "ok"<<std::endl;
			lgl::F32 L = Length(bnd);
			//	  std::cout << "L ="<<L<<std::endl;

			return L;
		}

		/// \brief Allocates a new MumfordShahNodeAttribute.
		virtual ScaleClimbingNodeAttribute* SCCB_Q_newNodeAttribute() {
			E_PolygonalBoundaryImageCovariance_NodeAttribute* a = new E_PolygonalBoundaryImageCovariance_NodeAttribute;
			// Explicit upcast is needed because of multiple path inheritence
			C_PolygonalBoundary_NodeAttribute* aa = a;
			return aa;
		}

		/// \brief Allocates a new MumfordShahEdgeAttribute.
		virtual ScaleClimbingEdgeAttribute* SCCB_Q_newEdgeAttribute() {
			E_PolygonalBoundaryImageCovariance_EdgeAttribute* a = new E_PolygonalBoundaryImageCovariance_EdgeAttribute;
			// Explicit upcast is needed because of multiple path inheritence
			C_PolygonalBoundary_EdgeAttribute* aa = a;
			return aa;
		}

	};
//=================================================================================

}
//===========================================================
// EO namespace sxs
//===========================================================

#endif

