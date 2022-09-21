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
#ifndef __sxsC_PolygonalBoundary_h__
#define __sxsC_PolygonalBoundary_h__
//===========================================================
#include "sxs/sxsGeneral.h"
#include "sxs/sxsImageScaleClimbingUser.h"
#include "sxs/sxsN_Verbose_User.h"

#include <list>
#include "lgl/lglPoint2D.h"
#include "lgl/lglArray2D.h"

#if wxUSE_GUI
#include <wx/frame.h>
#include <wx/dc.h>
#endif
//===========================================================
// namespace	: sxs
// classes		: ...
/*! \file 
 \brief Classes implementing 2D graphs whose edges hold a polygonal approximation of the boundary they represent (this is an implementation of the QuadEdge model for ScaleClimbingGraph).

 Despite its name, this file does not implement any C energy (i.e. it does not define any SCCB_C_eval ScaleClimbing callback).
 However, it is the basis for the implementation of any C energy based on polygonal approximations of the boundaries of the regions.
 */
//===========================================================
// author		: Laurent Guigues
// history		: 
//	13/10/04		creation
//===========================================================

namespace sxs {
	//===========================================================
	// Pre-declaration
	class PhysicalPolygonalBoundary;
	class C_PolygonalBoundary_User;
	//===========================================================

	//===========================================================
	/// \brief A point of a 2D polyline representing a piece of boundary between two regions
	class PolygonalBoundaryPoint {
	public:
		friend class PhysicalPolygonalBoundary;

		/// \brief Constructor
		PolygonalBoundaryPoint(lgl::F32 x, lgl::F32 y) :
				m_x(x), m_y(y) {
			m_next[0] = m_next[1] = 0;
		}
		/// \brief Constructor after pt
		PolygonalBoundaryPoint(lgl::F32 x, lgl::F32 y, PolygonalBoundaryPoint* pt) :
				m_x(x), m_y(y) {
			if (pt) {
				m_next[1] = pt;
				m_next[0] = pt->m_next[0];
				pt->m_next[0] = this;
			} else {
				m_next[0] = m_next[1] = 0;
			}
		}
		/// \brief Returns the x-coordinate of the point
		inline lgl::F32 x() const {
			return m_x;
		}
		/// \brief Returns the y-coordinate of the point
		inline lgl::F32 y() const {
			return m_y;
		}
		/// \brief Returns a reference on the x-coordinate of the point
		inline lgl::F32& x() {
			return m_x;
		}
		/// \brief Returns a reference on the y-coordinate of the point
		inline lgl::F32& y() {
			return m_y;
		}

		/// \brief Returns the next point when the polyline is followed in orientation
		inline PolygonalBoundaryPoint* next(int orientation = 0) const {
			return m_next[orientation];
		}
		/// \brief Returns the previous point when the polyline is followed in orientation
		inline PolygonalBoundaryPoint* prev(int orientation = 0) const {
			return m_next[1 - orientation];
		}

	protected:
		/// coordinates
		lgl::F32 m_x, m_y;
		/// next point on the boundary when followed in direct orientation (0) and reverse orientation (1)
		PolygonalBoundaryPoint* m_next[2];
	};
	//===========================================================

	//===========================================================
	/// \brief Iterator of the points of a PolygonalBoundary
	template<class Ref>
	class PolygonalBoundaryIterator {
	public:
		friend class PolygonalBoundary;
		// Default ctor (builds an end() iterator)
		PolygonalBoundaryIterator() :
				m_pt(0), m_ori(0) {
		}

		inline lgl::BOOL operator==(const PolygonalBoundaryIterator& i) const {
			return m_pt == i.m_pt;
		}
		inline lgl::BOOL operator!=(const PolygonalBoundaryIterator& i) const {
			return m_pt != i.m_pt;
		}
		void operator++() {
			m_pt = m_pt->next(m_ori);
		}
		Ref operator*() {
			return *m_pt;
		}

	protected:
		/// Current point iterated
		PolygonalBoundaryPoint* m_pt;
		/// Direction of iteration
		int m_ori;
		/// Useful constructor (hidden to the user : call PolygonalBoundary::begin/end)
		PolygonalBoundaryIterator(PolygonalBoundaryPoint* pt, int ori) :
				m_pt(pt), m_ori(ori) {
		}
	};
	//===========================================================

	//===========================================================
	/// \brief 2D Polyline representing an oriented piece of boundary between two regions. User level class.
	class PolygonalBoundary {
	public:
		friend class PhysicalPolygonalBoundary;
		friend class C_PolygonalBoundary_EdgeAttribute;
		friend class C_PolygonalBoundary_User;

		/// Iterator of the points of the boundary
		typedef PolygonalBoundaryIterator<PolygonalBoundaryPoint&> iterator;
		/// Constant iterator of the points of the boundary
		typedef PolygonalBoundaryIterator<const PolygonalBoundaryPoint&> const_iterator;

		/// \brief Default constructor
		PolygonalBoundary() :
				m_bnd(0), m_ori(0) {
		}
		/// \brief Destructor
		~PolygonalBoundary() {
		} //std::cout << "~PolBnd"; }

		/// \brief Returns the Null PolygonalBoundary
		inline static const PolygonalBoundary& Null() {
			static PolygonalBoundary b;
			return b;
		}

		/// \brief Comparison
		inline bool operator==(const PolygonalBoundary& b) const {
			return (m_bnd == b.m_bnd) && (m_ori == b.m_ori);
		}
		/// \brief Comparison
		inline bool operator!=(const PolygonalBoundary& b) const {
			return (m_bnd != b.m_bnd) || (m_ori != b.m_ori);
		}

		/// Returns the orientation of the boundary (0:physical orientation (often meaningless)/1:reverse)
		inline int getOrientation() const {
			return m_ori;
		}
		/// Sets the orientation of the boundary (0:physical orientation (often meaningless)/1:reverse)
		inline void setOrientation(int ori) {
			m_ori = ori;
		}
		/// Reverses the orientation of the boundary
		inline void reverseOrientation() {
			m_ori = 1 - m_ori;
		}
		/// Returns the boundary of reverse orientation
		inline PolygonalBoundary getReverse() const {
			return PolygonalBoundary(m_bnd, 1 - m_ori);
		}

		/// Returns an iterator of the points of the boundary initialized on the first point (orientation handled)
		inline iterator begin();
		/// Returns a past-the-end iterator of the points of the boundary (orientation handled)
		inline iterator end();
		/// Returns a constant iterator of the points of the boundary initialized on the first point (orientation handled)
		inline const_iterator begin() const;
		/// Returns a constant past-the-end iterator of the points of the boundary (orientation handled)
		inline const_iterator end() const;

		/// Returns the next PolygonalBoundary connected at the end-point of this (boundary following)
		inline PolygonalBoundary next() const;
		/// Returns the next PolygonalBoundary connected around the end-point of this (rotation around the junction)
		inline PolygonalBoundary next_around() const;
		/// Returns the previous PolygonalBoundary connected at the first point of this (boundary following in reverse direction)
		inline PolygonalBoundary prev() const;
		/// Returns the previous PolygonalBoundary connected around the first point of this (rotation around the junction)
		inline PolygonalBoundary prev_around() const;

		/// Returns the Edge holding the boundary
		inline ScaleClimbingEdge getEdge() const;

		/// Tests the coherence of the boundary (linking)
		bool test();
#if wxUSE_GUI
		//===============================================================================================
		// Drawing methods (wxWi)
		//===============================================================================================
		void draw( wxDC& dc, ign::image::segment::lgl::F32 zoom=1, bool drawPoints = true ) const;
#endif //#if wxUSE_GUI
	protected:
		/// Physical boundary
		PhysicalPolygonalBoundary* m_bnd;
		/// Orientation
		int m_ori;
		/// \brief Constructor (the default constructor builds a Null PolygonalBoundary).
		PolygonalBoundary(PhysicalPolygonalBoundary* bnd, int ori) :
				m_bnd(bnd), m_ori(ori) {
		}

	};
	//===========================================================

	//===========================================================
	/// \brief 2D Polyline representing a piece of boundary between two regions. System level class (hidden to the user).
	class PhysicalPolygonalBoundary {
	public:
		friend class PolygonalBoundary;
		friend class C_PolygonalBoundary_EdgeAttribute;
		friend class C_PolygonalBoundary_User;

		/// \brief Ctor
		PhysicalPolygonalBoundary() {
			m_point[0] = m_point[1] = 0;

		}
		/// \brief Returns a ptr on the first point of the boundary taken in orientation ori
		inline PolygonalBoundaryPoint* getFirstPoint(int ori) const {
			return m_point[ori];
		}
		/// \brief Adds a new point to the boundary (at the end in direct orientation)
		void addPoint(lgl::F32 x, lgl::F32 y) {
			m_point[1] = new PolygonalBoundaryPoint(x, y, m_point[1]);
			if (!m_point[0])
				m_point[0] = m_point[1];
		}
		/// \brief Deletes all the points of the boundary
		void deletePoints() {
			PolygonalBoundaryPoint* cur = m_point[0];
			while (cur) {
				PolygonalBoundaryPoint* prev = cur;
				cur = cur->next(0);
				delete prev;
			}
			m_point[0] = m_point[1] = 0;
		}

		/// \brief Captures the other PhysicalPolygonalBoundary (in the right orientation) and updates the graph structure.
		///
		/// \returns true iff the capture is succesfull (a common junction has been found)
		bool capture(PhysicalPolygonalBoundary* o);
		/// \brief Physically inverts the linking of the points of the boundary.
		void physicalInversion();

	protected:

		/// statistics on the intermediate points suppressed between a point and the next one
		/// for dynamic polyline simplification
		typedef struct {
			/// number of intermediate points
			lgl::I32 m_n;
			/// Sum of their coords (1st moment)
			lgl::F32 m_sx, m_sy;
			/// Quadratic sums (2nd moments)
			lgl::F32 m_sxx, m_sxy, m_syy;
		} Point_stats;

		/// Initial point : 0 : in direct orientation / 1 : reverse
		PolygonalBoundaryPoint* m_point[2];
		/// Next polygonal boundary when this is followed : 0 : in direct orientation / 1 : reverse.
		/// The boundaries turn around their left region (i.e. sorting at junctions is made in inverse trigonometric order)
		PolygonalBoundary m_next[2];
		/// Statistics of the Initial point : 0 : in direct orientation / 1 : reverse
		Point_stats m_stat[2];
		/// The Edge holding this Boundary
		ScaleClimbingEdge m_edge;

	};
	//===============================================================================================

	//===============================================================================================
	/// \brief Edge attribute for C_PolygonalBoundary-based energies
	class C_PolygonalBoundary_EdgeAttribute: public ScaleClimbingEdgeAttribute {
	public:
		friend class C_PolygonalBoundary_User;
		/// Def ctor
		C_PolygonalBoundary_EdgeAttribute() {
		}
		/// Returns the PolygonalBoundary in orientation ori
		PolygonalBoundary getPolygonalBoundary(int ori = 0) {
			return PolygonalBoundary(&m_bnd, ori);
		}
	protected:
		PhysicalPolygonalBoundary m_bnd;
	};
	//===============================================================================================

	//===============================================================================================
	/// \brief Node attribute for C_PolygonalBoundary-based energies
	class C_PolygonalBoundary_NodeAttribute: public ScaleClimbingNodeAttribute {
	public:

	};
	//===============================================================================================

	//===============================================================================================
	/// \brief User for C_PolygonalBoundary-based energies
	class C_PolygonalBoundary_User: public virtual ImageScaleClimbingUser, public virtual N_Verbose_User {
	public:

		//===============================================================================================
		/// \name Misc
		/// @{

		/// Ctor
		C_PolygonalBoundary_User() :
				m_graphical_mode(false) {
		}

		/// Set the graphical mode on/off (if on the graph is drawn during scale climbing)
		void setGraphicalMode(bool on = true) {
			m_graphical_mode = on;
		}

		/// @}
		//===============================================================================================

		//===============================================================================================
		/// \name Graph building
		/// @{

		/// \brief Returns the mode that must be used to build the graph (here 2DBoundaryTracking)
		virtual BuildGraphMode SCCB_Q_buildGraphMode() {
			return ImageScaleClimbingUser::BuildGraph2DBoundaryTracking;
		}

		/// \brief Pre-build graph : allocates the junction map
		virtual void SCCB_N_preBuildGraphNotification();

		/// \brief Sets the ign::image::segment::lgl::InterPixelBoundary associated with an Edge
		/// (called in ImageScaleClimbing::buildGraph() when the creation mode is BuildGraph2DBoundaryTracking).
		///
		/// \param bnd : the InterPixelBoundary associated with the edge e
		virtual void SCCB_C_setEdgeBoundary(Edge e, const lgl::InterPixelBoundary& bnd);

		/// \brief Post-build graph : links the boundaries at junctions and frees the junction map
		virtual void SCCB_N_postBuildGraphNotification();

		/// @}
		//===============================================================================================

		//===============================================================================================
		/// \name Graph updating
		/// @{

		/// Called before the contraction of Edge e (beginning of contract(e)).
		///
		/// Its Node from() will survive and to() will die.
		/// The user can hence update the attributes of from() in order to
		/// reflect its merging with to().
		// virtual void SCCB_C_onEdgeContraction( Edge e ) {}
		/// Called before physical deletion of Edge e
		virtual void SCCB_C_onEdgeDeletion(Edge e);
		/// Called before Edge "merged" is merged to Edge "to" ("merged" will be deleted just after)
		virtual bool SCCB_C_onEdgeMerging(Edge merged, Edge to);
		/// Called before the initial Node of Edge e is changed to new_origin
		//virtual void SCCB_C_onNodeFromChange(Edge e, Node new_origin) {}
		/// Called before Node n is physically removed from the graph
		//virtual void SCCB_C_onNodeDeletion(Node n) {}

		/// @}
		//===============================================================================================

		//===============================================================================================
		/// \name Notifications
		/// @{

		/// Called back before climbing starts (beginning of climb()) : if GraphicalMode is ON then creates a wxFrame to draw the graph
		virtual void SCCB_N_preRun(); //ClimbingNotification();
		/// Called after climbing (end of climb()) : if GraphicalMode is ON then destroys the wxFrame used to draw the graph
		virtual void SCCB_N_postRun(); //ClimbingNotification();
		/// Called before each ScaleClimbing step (beginning of step()) : if GraphicalMode is ON then draws the current graph
		virtual void SCCB_N_preStepNotification();
		/// @}
		//===============================================================================================

	protected:
		/// Junction : structure of the 4 PolygonalBoundary connected to the junction
		class Junction {
		public:
			Junction() {
			}

			inline PolygonalBoundary& operator[](int n) {
				return m_b[n];
			}

		protected:
			PolygonalBoundary m_b[4];
		};

		/// 4 channels image storing the PolygonalBoundary (oriented) starting from each of the 4 freeman directions of the image "junctions"
		/// (pixels of the dual graph). Used to link the Boundaries in inverse trigo. order (m_next).
		lgl::Array2D<Junction>* m_junction;

		/// Graphical mode
		bool m_graphical_mode;
		/// Frame used to draw the graph if GraphicalMode ON
		// wxFrame* m_frame;

	};
	//===============================================================================================

	//===============================================================================================
	/// \brief Returns the PolygonalBoundary associated with an Edge (properly oriented according to the Edge orientation)
	///
	/// NB : the Graph attributes must be of the kind C_PolygonalBoundary_?Attribute
	inline PolygonalBoundary getPolygonalBoundary(ScaleClimbingEdge e) {
		// dynamic down cast of the edge's attribute
		C_PolygonalBoundary_EdgeAttribute* pb = dynamic_cast<C_PolygonalBoundary_EdgeAttribute*>(e.getAttribute());
		if (!pb) {
			lglERROR("sxs::getPolygonalBoundary( ScaleClimbingEdge e ) : The Edge attribute is not of the kind C_PolygonalBoundary_EdgeAttribute");
			//return PolygonalBoundary::Null();
		}
		return pb->getPolygonalBoundary(e.getOrientation());
	}
	//===============================================================================================

	//===============================================================================================
	/// \brief Returns the set of PolygonalBoundary describing the boundaries of a region (Node)
	///
	/// Each element of bnd represent a connected component of the boundary of n.
	/// Each PolygonalBoundary of a connected component gives a piece of the polyline describing the connected component (its points can be iterated).
	/// NB : the Graph attributes must be of the kind C_PolygonalBoundary_?Attribute
	void getPolygonalBoundary(ScaleClimbingGraph& g, ScaleClimbingNode n, std::vector<std::vector<PolygonalBoundary> >& bnd);
	//===============================================================================================

	//===============================================================================================
	/// \brief Returns the set of PolygonalBoundary describing the boundaries of the unions of two regions (Node)
	///
	/// Each element of bnd represent a connected component of the boundary of n1 union n2.
	/// Each PolygonalBoundary of a connected component gives a piece of the polyline describing the connected component (its points can be iterated).
	/// NB : the Graph attributes must be of the kind C_PolygonalBoundary_?Attribute
	void getPolygonalBoundary(ScaleClimbingGraph& g, ScaleClimbingNode n1, ScaleClimbingNode n2, std::vector<std::vector<PolygonalBoundary> >& bnd);
	//===============================================================================================

	//===============================================================================================
	/// \brief 2D Polylines
	typedef std::vector<lgl::Point2D<lgl::F32> > Polyline2D;
	//===============================================================================================

	//===============================================================================================
	/// \brief Returns the set of Polylines describing the boundaries of a region (Node)
	///
	/// Each element of bnd is the Polyline2D describing a connected component of the boundary of n.
	/// NB : the Graph attributes must be of the kind C_PolygonalBoundary_?Attribute
	void getPolygonalBoundary(ScaleClimbingGraph& g, ScaleClimbingNode n, std::vector<Polyline2D>& bnd);
	//===============================================================================================

	//===============================================================================================
	// Inline methods that couldn't be defined before (incomplete types due to PolygonalBoundary/PhysicalPolygonalBoundary cross references)
	PolygonalBoundary PolygonalBoundary::next() const {
		return m_bnd->m_next[m_ori];
	}
	PolygonalBoundary PolygonalBoundary::prev() const {
		PolygonalBoundary p(m_bnd, 1 - m_ori);
		while (p.next().m_bnd != m_bnd) {
			p = p.next();
			p.reverseOrientation();
		}
		return p;
	}
	PolygonalBoundary PolygonalBoundary::next_around() const {
		PolygonalBoundary b(m_bnd->m_next[m_ori]);
		b.reverseOrientation();
		return b;
	}
	PolygonalBoundary PolygonalBoundary::prev_around() const {
		PolygonalBoundary p = prev();
		p.reverseOrientation();
		return p;
	}
	PolygonalBoundary::iterator PolygonalBoundary::begin() {
		return iterator(m_bnd->getFirstPoint(m_ori), m_ori);
	}
	PolygonalBoundary::iterator PolygonalBoundary::end() {
		return iterator();
	}
	PolygonalBoundary::const_iterator PolygonalBoundary::begin() const {
		return const_iterator(m_bnd->getFirstPoint(m_ori), m_ori);
	}
	PolygonalBoundary::const_iterator PolygonalBoundary::end() const {
		return const_iterator();
	}
	/// Returns the Edge holding the boundary
	ScaleClimbingEdge PolygonalBoundary::getEdge() const {
		if (m_ori == 0)
			return m_bnd->m_edge;
		else
			return m_bnd->m_edge.getReverseEdge();
	}
//===============================================================================================

#if wxUSE_GUI
//===============================================================================================
// Drawing methods (wxWi)
//===============================================================================================
/// \brief Draws the PolygonalBoundaries of the ScaleClimbingGraph G (whose attributes must be of the type C_PolygonalBoundary*)
void drawPolygonalBoundaries( const ScaleClimbingGraph& G,
		wxDC& dc, ign::image::segment::lgl::F32 zoom, bool drawPoints);
/// \brief Draws the PolygonalBoundaries of the ScaleClimbingGraph G (whose attributes must be of the type C_PolygonalBoundary*) : Node-oriented method (finds the Boundary of each Node and draws it).
/// NB : The graph is not const as a Slot must be allocated. However it is unchanged...
void drawNodesPolygonalBoundaries( ScaleClimbingGraph& G,
		wxDC& dc, ign::image::segment::lgl::F32 zoom, bool drawPoints);
/// \brief Draws the vector of PolygonalBoundary provided
void drawPolygonalBoundaries( const std::vector<PolygonalBoundary>& bnd,
		wxDC& dc, ign::image::segment::lgl::F32 zoom, bool drawPoints);

/// \brief Draws the vector of vector of PolygonalBoundary provided
inline void drawPolygonalBoundaries( const std::vector<std::vector<PolygonalBoundary> > & bnd,
		wxDC& dc, ign::image::segment::lgl::F32 zoom, bool drawPoints)
{
	for (std::vector<std::vector<PolygonalBoundary> >::const_iterator i=bnd.begin();i!=bnd.end();++i)
	drawPolygonalBoundaries((*i),dc,zoom,drawPoints);
}
#endif //#if wxUSE_GUI  
}
//===========================================================
// EO namespace sxs
//===========================================================
#endif
