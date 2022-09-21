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
/*! \file 
 \brief Code of the methods of the class ImageScaleClimbing and of nested classes (Parameters).
 */

#include <sxs/sxsC_PolygonalBoundary.h>

#if wxUSE_GUI
#include <wx/frame.h>
#include <wx/dcclient.h>
#endif

#define GMZOOM 7


namespace sxs {

	//=================================================================================================
	/// \brief Captures the other PhysicalPolygonalBoundary (in the right orientation) and updates the graph structure.
	///
	/// \returns true iff the capture is succesfull (a common degree 2 junction has been found)
	bool PhysicalPolygonalBoundary::capture(PhysicalPolygonalBoundary* o) {
		//    std::cout << "* Capture "<< this << " <- " << o ;
		// checks wether this is connected to o
		// At the end
		PolygonalBoundary b(this, 0);
		b = b.next();
		b.test();
		// found and degree 2 junction
		if (b.m_bnd == o && b.getReverse().next().m_bnd == this) {
			//      std::cout << std::endl;
			// if indirect orientation : physical inversion
			if (b.getOrientation() == 1)
				o->physicalInversion();
			// Points connexion
			m_point[1]->m_next[0] = o->m_point[0];
			o->m_point[0]->m_next[1] = m_point[1];
			m_point[1] = o->m_point[1];
			o->m_point[0] = o->m_point[1] = 0;
			// Update next boundary
			PolygonalBoundary prev(o, 0);
			for (; prev.next().m_bnd != o; prev = prev.next_around())
				;
			//      std::cout << prev.m_bnd << ":" << prev.m_bnd->m_next[prev.m_ori].m_bnd << "->" << this << std::endl ;
			prev.m_bnd->m_next[prev.m_ori] = PolygonalBoundary(this, 1);
			//     std::cout << this << ":" << m_next[0].m_bnd << "->" << o->m_next[0].m_bnd << std::endl ;
			m_next[0] = o->m_next[0];
			o->m_next[0] = PolygonalBoundary(o, 1);
			o->m_next[1] = PolygonalBoundary(o, 0);
			// Stats ... TO DO when usefull

			prev.test();
			prev = PolygonalBoundary(this, 0);
			prev.test();
			prev = PolygonalBoundary(o, 0);
			prev.test();

			//      std::cout << "T1"<<std::endl;
			return true;
		} else {
			// is o connected to this at the beginning ?
			PolygonalBoundary b(this, 1);
			b = b.next();
			b.reverseOrientation();
			// found and degree 2 junction
			if (b.m_bnd == o && b.next().m_bnd == this) {
				//	std::cout << std::endl;
				// if indirect orientation : physical inversion
				if (b.getOrientation() == 1)
					o->physicalInversion();
				// Points connexion
				m_point[0]->m_next[1] = o->m_point[1];
				o->m_point[1]->m_next[0] = m_point[0];
				m_point[0] = o->m_point[0];
				o->m_point[0] = o->m_point[1] = 0;
				// Update next boundary
				PolygonalBoundary prev(o, 1);
				for (; prev.next().m_bnd != o; prev = prev.next_around())
					;
				//	std::cout << prev.m_bnd << ":" << prev.m_bnd->m_next[prev.m_ori].m_bnd << "->" << this << std::endl ;
				prev.m_bnd->m_next[prev.m_ori] = PolygonalBoundary(this, 0);
				//	std::cout << this << ":" << m_next[1].m_bnd << "->" << o->m_next[1].m_bnd << std::endl ;
				m_next[1] = o->m_next[1];
				o->m_next[0] = PolygonalBoundary(o, 1);
				o->m_next[1] = PolygonalBoundary(o, 0);
				// Stats ... TO DO when usefull
				prev.test();
				prev = PolygonalBoundary(this, 0);
				prev.test();
				prev = PolygonalBoundary(o, 0);
				prev.test();

				//	std::cout << "T2"<<std::endl;
				return true;
			}
			// No connexion found
			//      std::cout << ": NOT CONNECTED"<<std::endl;
			return false;
		}
	}
	//=================================================================================================

	//================================================================================================
	/// \brief Physically inverts the linking of the points of the boundary.
	void PhysicalPolygonalBoundary::physicalInversion() {
		//   std::cout << "* Chain inversion : "<<std::endl;
		PolygonalBoundary pb(this, 0);
		PolygonalBoundary::iterator it;
		//    for (it=pb.begin();it!=pb.end();++it) {
		//      std::cout << "("<<(*it).x()<<","<<(*it).y()<<")";
		//   }
		//   std::cout << std::endl;
		// Chain inversion
		PolygonalBoundaryPoint* p = m_point[0];
		while (p) {
			PolygonalBoundaryPoint* tmp = p->m_next[0];
			p->m_next[0] = p->m_next[1];
			p->m_next[1] = tmp;
			p = tmp;
		}
		p = m_point[0];
		m_point[0] = m_point[1];
		m_point[1] = p;
		//    std::cout << "chinv ok "<<std::endl;
		// Stats inversion
		Point_stats s = m_stat[0];
		m_stat[0] = m_stat[1];
		m_stat[1] = s;
		//    std::cout << "stats ok "<<std::endl;
		// Order inversion at junction
		PolygonalBoundary bnd(this, 0);
		PolygonalBoundary prev0 = bnd.prev();
		prev0.m_bnd->m_next[prev0.m_ori].reverseOrientation();

		bnd.reverseOrientation();
		PolygonalBoundary prev1 = bnd.prev();
		prev1.m_bnd->m_next[prev1.m_ori].reverseOrientation();
		bnd = m_next[0];
		m_next[0] = m_next[1];
		m_next[1] = bnd;

		bnd = PolygonalBoundary(this, 0);
		bnd.test();
		/*
			for (it=pb.begin();it!=pb.end();++it) {
			std::cout << "("<<(*it).x()<<","<<(*it).y()<<")";
			}
			std::cout << std::endl;
			*/
	}
	//=================================================================================================

	//================================================================================================
	bool PolygonalBoundary::test() {
		PolygonalBoundary b(*this);
		int i = 0;
		for (b = b.next_around(); b != *this && i < 5; b = b.next_around(), i++)
			;
		if (b != *this) {
			std::cout << "!!! sxs::PolygonalBoundary " << this->m_bnd << " : forward linking error (";
			b = PolygonalBoundary(*this);
			int i = 0;
			for (b = b.next_around(); b != *this && i < 5; b = b.next_around(), i++) {
				std::cout << b.m_bnd << " ";
			}
			std::cout << ")" << std::endl;
			return false;
		}
		PolygonalBoundary b0(*this);
		b0.reverseOrientation();
		b = b0;
		i = 0;
		for (b = b.next_around(); b != b0 && i < 5; b = b.next_around(), i++)
			;
		if (b != b0) {
			std::cout << "!!! sxs::PolygonalBoundary " << this->m_bnd << " : backward linking error (";
			b = b0;
			int i = 0;
			for (b = b.next_around(); b != b0 && i < 5; b = b.next_around(), i++) {
				std::cout << b.m_bnd << " ";
			}
			std::cout << ")" << std::endl;
			return false;
		}
		return true;
	}
	//=================================================================================================

	//================================================================================================
	/// \brief Pre-build graph : allocates the junction map
	void C_PolygonalBoundary_User::SCCB_N_preBuildGraphNotification() {
		N_Verbose_User::SCCB_N_preBuildGraphNotification();
		m_junction = new lgl::Array2D<Junction>(getImageScaleClimbing().getBaseSegmentation().size(2) + 1,
				getImageScaleClimbing().getBaseSegmentation().size(1) + 1);
		//    std::cout << m_junction->size(0) << "x" << m_junction->size(1) <<std::endl;
	}
	//=================================================================================================

	//=================================================================================================
	void C_PolygonalBoundary_User::SCCB_C_setEdgeBoundary(Edge e, const lgl::InterPixelBoundary& bnd) {
		// dynamic downcast
		C_PolygonalBoundary_EdgeAttribute* b = dynamic_cast<C_PolygonalBoundary_EdgeAttribute*>(e.getAttribute());
		PhysicalPolygonalBoundary& pb = b->m_bnd;
		// Add points
		lgl::InterPixelBoundary::const_iterator it;
		for (it = bnd.begin(); it != bnd.end(); ++it) {
			lgl::FPoint2D pt = (*it).getCenter();
			pb.addPoint(pt(0), pt(1));
		}
		// graph edge reference
		pb.m_edge = e;
		// junctions update
		// Initial junction
		// Position of the first Edgel
		int i = bnd.front().getPosition()(0);
		int j = bnd.front().getPosition()(1);
		// Direction of the first Edgel
		lgl::I8 dir = bnd.front().getDirection();
		(*m_junction)(j, i)[dir].m_bnd = &pb;
		(*m_junction)(j, i)[dir].m_ori = 0;
		// Final junction
		// Last Edgel
		lgl::Edgel el = bnd.back();
		// reverse
		el.reverse();
		// Position
		i = el.getPosition()(0);
		j = el.getPosition()(1);
		// Direction
		dir = el.getDirection();
		(*m_junction)(j, i)[dir].m_bnd = &pb;
		(*m_junction)(j, i)[dir].m_ori = 1;
		//
	}
	//=================================================================================================

	//=================================================================================================
	/// \brief Post-build graph : links the boundaries at junctions and frees the junction map
	void C_PolygonalBoundary_User::SCCB_N_postBuildGraphNotification() {
		//    std::cout << "* Bnd linking"<<std::endl;
		//    std::cout << m_junction->size(0) << "x" << m_junction->size(1) <<std::endl;
		// Boundary linking
		lgl::Array2D<Junction>::iterator it;
		for (it = m_junction->begin(); it != m_junction->end(); ++it) {

			int i0 = 3;
			while ((i0 >= 0) && ((*it)[i0] == PolygonalBoundary::Null()))
				i0--;
			if (i0 < 0)
				continue;
			int i1 = i0;
			while (i1 > 0) {
				int i2 = i1 - 1;
				while ((i2 >= 0) && ((*it)[i2] == PolygonalBoundary::Null())) {
					i2--;
				}
				if (i2 < 0)
					break;
				// linking
				(*it)[i1].m_bnd->m_next[1 - (*it)[i1].m_ori] = (*it)[i2];
				//	std::cout << "("<<(*it)[i1].m_bnd<<"->"<<(*it)[i2].m_bnd<<")";
				i1 = i2;
			}
			// linking last one to first one
			(*it)[i1].m_bnd->m_next[1 - (*it)[i1].m_ori] = (*it)[i0];
			//      std::cout << "f("<<(*it)[i1].m_bnd<<"->"<<(*it)[i0].m_bnd<<")";
			//      std::cout<< std::endl;
			/*
				PolygonalBoundary b=(*it)[i0],bb;
				b.reverseOrientation();
				bb = b;
				int i=0;
				for (b=b.next_around(); b!=bb && i<5; b=b.next_around(), i++);
				if (b!=bb) { std::cout << "sxs::PolygonalBoundary " << b.m_bnd << " : backward linking error "<<std::endl; }
				*/

		}

		// junction deletion
		delete m_junction;

		// test
		//    std::cout << "Testing ... ";
		ScaleClimbingGraph::EdgeSet::iterator ei;
		for (ei = getScaleClimbing().getGraph().getEdges().begin(); ei != getScaleClimbing().getGraph().getEdges().end(); ++ei) {
			C_PolygonalBoundary_EdgeAttribute* a = dynamic_cast<C_PolygonalBoundary_EdgeAttribute*>((*ei).getAttribute());
			a->getPolygonalBoundary().test();
		}
		//    std::cout << "ok"<<std::endl;

		N_Verbose_User::SCCB_N_postBuildGraphNotification();
	}
	//=================================================================================================

	//=================================================================================================
	bool C_PolygonalBoundary_User::SCCB_C_onEdgeMerging(Edge merged, Edge to) {
		//   std::cout << "* onEdgeMerging "<<std::endl;
		// dynamic downcast
		C_PolygonalBoundary_EdgeAttribute* bm = dynamic_cast<C_PolygonalBoundary_EdgeAttribute*>(merged.getAttribute());
		C_PolygonalBoundary_EdgeAttribute* bt = dynamic_cast<C_PolygonalBoundary_EdgeAttribute*>(to.getAttribute());
		PhysicalPolygonalBoundary& pbm = bm->m_bnd;
		PhysicalPolygonalBoundary& pbt = bt->m_bnd;
		bool res = pbt.capture(&pbm);
		PolygonalBoundary bnd(&pbm, 0);
		bnd.test();
		PolygonalBoundary bnd1(&pbt, 0);
		bnd1.test();
		//    if (res) std::cout << "MERGE"<<std::endl;
		//    else std::cout << " NO" << std::endl;
		return res;
	}
	//=================================================================================================

	//=================================================================================================
	void C_PolygonalBoundary_User::SCCB_C_onEdgeDeletion(Edge e) {

		// dynamic downcast
		C_PolygonalBoundary_EdgeAttribute* b = dynamic_cast<C_PolygonalBoundary_EdgeAttribute*>(e.getAttribute());

		// test
		/*
			if (m_graphical_mode) {
			wxClientDC dc(m_frame);
			dc.Clear();
			b->getPolygonalBoundary().draw(dc,3,false);
			}
			*/
		// delete
		PhysicalPolygonalBoundary& pb = b->m_bnd;

		//    std::cout << "* onEdgeDeletion (" <<&pb<<")"<< std::endl;
		// boundary points deletion
		//    std::cout << " deletePoints ";
		pb.deletePoints();
		//    std::cout << "ok " << std::endl;
		// boundary unlinking
		PolygonalBoundary bnd0(&pb, 0);
		PolygonalBoundary bnd1(&pb, 1);
		bnd0.test();

		PolygonalBoundary prev0 = bnd0.prev();
		//    std::cout << prev0.m_bnd << ":" << prev0.m_bnd->m_next[prev0.m_ori].m_bnd << "->" << bnd1.next().m_bnd << std::endl;
		prev0.m_bnd->m_next[prev0.m_ori] = bnd1.next();

		PolygonalBoundary prev1 = bnd1.prev();
		//    std::cout << prev1.m_bnd << ":" << prev1.m_bnd->m_next[prev1.m_ori].m_bnd << "->" << bnd0.next().m_bnd << std::endl;
		prev1.m_bnd->m_next[prev1.m_ori] = bnd0.next();

		pb.m_next[0] = bnd1;
		pb.m_next[1] = bnd0;
		bnd0.test();
		prev0.test();
		prev1.test();
		/*

			PolygonalBoundary bnd0(&pb,0);
			PolygonalBoundary rbnd0(&pb,1);
			PolygonalBoundary next = bnd0.next();
			PolygonalBoundary cur = next;
			cur.reverseOrientation() ;
			while (cur.next()!=rbnd0) {
			std::cout << cur.m_bnd << ","<<cur.m_ori<<"-";
			cur = cur.next(); cur.reverseOrientation(); }
			cur.m_bnd->m_next[cur.m_ori] = next;
			// undirect orientation
			next = rbnd0.next();
			cur = next;
			cur.reverseOrientation() ;
			while (cur.next()!=bnd0) { cur = cur.next(); cur.reverseOrientation(); }
			cur.m_bnd->m_next[cur.m_ori] = next;
			*/
		//    std::cout << "ok"<<std::endl;
		//
	}
	//=================================================================================================

	//=================================================================================================
	void C_PolygonalBoundary_User::SCCB_N_preRun() //ClimbingNotification()
	{
		N_Verbose_User::SCCB_N_preRun(); //ClimbingNotification();
#if wxUSE_GUI
		if (!m_graphical_mode) return;

		wxSize sz(getImageScaleClimbing().getImage/*BaseSegmentation*/().size(1)*GMZOOM,
				getImageScaleClimbing().getImage/*BaseSegmentation*/().size(2)*GMZOOM);
		m_frame = new wxFrame(0,0,wxT("graph"),wxDefaultPosition,sz);
		m_frame->Show();
#endif //#if wxUSE_GUI
	}
	//=================================================================================================
	//=================================================================================================
	void C_PolygonalBoundary_User::SCCB_N_postRun() //ClimbingNotification()
	{
		N_Verbose_User::SCCB_N_postClimbingNotification();
#if wxUSE_GUI
		if (!m_graphical_mode) return;
		m_frame->Close();
#endif //#if wxUSE_GUI  
	}
	//=================================================================================================

	//=================================================================================================
	void C_PolygonalBoundary_User::SCCB_N_preStepNotification() {
		N_Verbose_User::SCCB_N_preStepNotification();
		//    std::cout<<"* preStep"<<std::endl;

#if wxUSE_GUI
		if (!m_graphical_mode) return;

		wxStopWatch sw;
		//    long it = sw.Time();
		while (sw.Time()<0) {}

		wxClientDC dc(m_frame);
		// char i=' '; std::cin>>i;
		static int i=0;
		if (!((i++)%100)) {
			dc.Clear();
			wxPen myPen = dc.GetPen();
			myPen.SetColour(*wxBLACK);
			dc.SetPen(myPen);
			drawNodesPolygonalBoundaries(getScaleClimbing().getGraph(),dc,GMZOOM,false);
		}
#endif //#if wxUSE_GUI  
	}
	//=================================================================================================

	//===============================================================================================
	/// \brief Returns the set of PolygonalBoundary describing the boundaries of a region (Node)
	///
	/// Each element of bnd represent a connected component of the boundary of n.
	/// Each PolygonalBoundary of a connected component gives a piece of the polyline describing the connected component (its points can be iterated).
	/// NB : the Graph attributes must be of the kind C_PolygonalBoundary_?Attribute
	void getPolygonalBoundary(ScaleClimbingGraph& g, ScaleClimbingNode n, std::vector<std::vector<PolygonalBoundary> >& bnd) {
		// Allocate an Edge Slot
		ScaleClimbingGraph::Slot em;
		if (!g.allocateEdgeSlot(em)) {
			lglERROR("sxs::getPolygonalBoundary(...) : cannot allocate 1 bit Slot on the Graph !");
			return;
		}
		//      std::cout << "sxs::getPolygonalBoundary(Node)" << std::endl;
		// Iterates the incident edges to node n
		ScaleClimbingNode::iterator eit;
		for (eit = n.begin(); eit != n.end(); ++eit) {
			// Skips allready viewed edges
			if ((*eit).getMark(em) == 1)
				continue;

			//	std::cout << "Start CC : " ;

			// Starts a new vector of PolygonalBoundary
			std::vector<PolygonalBoundary> nb;
			bnd.push_back(nb);
			// Current boundary
			PolygonalBoundary cb = getPolygonalBoundary(*eit);
			// While unmarked
			while (cb.getEdge().getMark(em) == 0) {
				// std::cout << cb.getEdge().getIndex() << " ";
				// Marks the edge
				cb.getEdge().setMark(em, 1);
				bnd.back().push_back(cb);
				cb = cb.next();
			}
		}
		// Unmarks all incident edges
		for (eit = n.begin(); eit != n.end(); ++eit)
			(*eit).setMark(em, 0);
		// Deallocates Slot em
		g.freeEdgeSlot(em);
		//      std::cout << "eo getPB";
	}
	//===============================================================================================

	//===============================================================================================
	/// \brief Returns the set of PolygonalBoundary describing the boundaries of the unions of two regions (Node)
	///
	/// Each element of bnd represent a connected component of the boundary of n1 union n2.
	/// Each PolygonalBoundary of a connected component gives a piece of the polyline describing the connected component (its points can be iterated).
	/// NB : the Graph attributes must be of the kind C_PolygonalBoundary_?Attribute
	void getPolygonalBoundary(ScaleClimbingGraph& g, ScaleClimbingNode n1, ScaleClimbingNode n2,
			std::vector<std::vector<PolygonalBoundary> >& bnd) {
		// Allocate an Edge Slot
		ScaleClimbingGraph::Slot em;
		if (!g.allocateEdgeSlot(em)) {
			lglERROR("sxs::getPolygonalBoundary(...) : cannot allocate 1 bit Slot on the Graph !");
			return;
		}
		//      std::cout << "sxs::getPolygonalBoundary(Edge)" << std::endl;
		// Iterates the incident edges to node n1
		ScaleClimbingNode::iterator eit;
		for (eit = n1.begin(); eit != n1.end(); ++eit) {
			// Skips allready viewed edges
			if ((*eit).getMark(em) == 1)
				continue;
			// Skips edges whose final node is n2
			if ((*eit).getFinalNode() == n2)
				continue;
			// Starts a new vector of PolygonalBoundary
			std::vector<PolygonalBoundary> nb;
			bnd.push_back(nb);
			// Current boundary
			PolygonalBoundary cb = getPolygonalBoundary(*eit);
			// While unmarked
			while (cb.getEdge().getMark(em) == 0) {
				// Marks the edge
				cb.getEdge().setMark(em, 1);
				bnd.back().push_back(cb);
				do {
					cb = cb.next_around();
				} while ((cb.getEdge().getInitialNode() == n2) && (cb.getEdge().getMark(em) == 0));
				cb.reverseOrientation();
			}
		}
		// Iterates the incident edges to node n2
		for (eit = n2.begin(); eit != n2.end(); ++eit) {
			// Skips allready viewed edges
			if ((*eit).getMark(em) == 1)
				continue;
			// Skips edges whose final node is n1
			if ((*eit).getFinalNode() == n1)
				continue;
			// Starts a new vector of PolygonalBoundary
			std::vector<PolygonalBoundary> nb;
			bnd.push_back(nb);
			// Current boundary
			PolygonalBoundary cb = getPolygonalBoundary(*eit);
			// While unmarked
			while (cb.getEdge().getMark(em) == 0) {
				// Marks the edge
				cb.getEdge().setMark(em, 1);
				bnd.back().push_back(cb);
				do {
					cb = cb.next_around();
				} while ((cb.getEdge().getInitialNode() == n1) && (cb.getEdge().getMark(em) == 0));
				cb.reverseOrientation();
			}
		}
		// Unmarks all incident edges
		for (eit = n1.begin(); eit != n1.end(); ++eit)
			(*eit).setMark(em, 0);
		for (eit = n2.begin(); eit != n2.end(); ++eit)
			(*eit).setMark(em, 0);
		// Deallocates Slot em
		g.freeEdgeSlot(em);
	}
	//===============================================================================================

	//===============================================================================================
	/// \brief Returns the set of Polylines describing the boundaries of a region (Node)
	///
	/// Each element of bnd is the Polyline2D describing a connected component of the boundary of n.
	/// NB : the Graph attributes must be of the kind C_PolygonalBoundary_?Attribute
	void getPolygonalBoundary(ScaleClimbingGraph& g, ScaleClimbingNode n, std::vector<Polyline2D>& bnd) {
		// dynamic down cast of the edge's attribute
	}
//===============================================================================================

#if wxUSE_GUI
//===============================================================================================
// Drawing methods (wxWi)
//===============================================================================================
void PolygonalBoundary::draw( wxDC& dc, lgl::F32 zoom, bool drawPoints) const
{
	const_iterator i=begin();
	if (i==end()) return;
	wxCoord x = (wxCoord)( ((*i).x()+0.5)*zoom ) ,y = (wxCoord)( ((*i).y()+0.5)*zoom );
	wxCoord r = (wxCoord)(0.5*zoom);
	if (r==0) r=1;
	if (drawPoints) dc.DrawCircle(x,y,r);
	for (++i;i!=end();++i) {
		wxCoord x1 = (wxCoord)( ((*i).x()+0.5)*zoom ) ,y1 = (wxCoord)( ((*i).y()+0.5)*zoom );
		dc.DrawLine(x,y,x1,y1);
		if (drawPoints) dc.DrawCircle(x1,y1,r);
		x=x1;
		y=y1;
	}

}
//===============================================================================================

//===============================================================================================
void drawPolygonalBoundaries( const ScaleClimbingGraph& G, wxDC& dc, lgl::F32 zoom, bool drawPoints)
{
	ScaleClimbingGraph::EdgeSet::iterator e;
	for (e=G.getEdges().begin(); e!=G.getEdges().end(); ++e) {
		C_PolygonalBoundary_EdgeAttribute* a = dynamic_cast<C_PolygonalBoundary_EdgeAttribute*>((*e).getAttribute());
		if (a==0) {
			lglERROR("drawPolygonalBoundaries( const ScaleClimbingGraph& ...) : The Edges attribute is not a PhysicalPolygonalBoundary* ! ");
		}
		a->getPolygonalBoundary().draw(dc,zoom,drawPoints);
	}
}
//===============================================================================================

//===============================================================================================
void drawNodesPolygonalBoundaries( ScaleClimbingGraph& G, wxDC& dc, lgl::F32 zoom, bool drawPoints)
{
	ScaleClimbingGraph::NodeSet::iterator e;
	for (e=G.getNodes().begin(); e!=G.getNodes().end(); ++e) {
		std::vector<std::vector<PolygonalBoundary> > bnd;
		ScaleClimbingNode n = *e;
		getPolygonalBoundary( G, n, bnd );
		drawPolygonalBoundaries(bnd,dc,zoom,drawPoints);
	}
}
//===============================================================================================

//===============================================================================================
/// \brief Draws the PolygonalBoundaries provided
void drawPolygonalBoundaries( const std::vector<PolygonalBoundary>& bnd,
		wxDC& dc, lgl::F32 zoom, bool drawPoints)
{
	std::vector<PolygonalBoundary>::const_iterator i=bnd.begin();
	if (i==bnd.end()) return;
	PolygonalBoundary::const_iterator j=(*i).begin();
	if (j==(*i).end()) return;
	wxCoord x = (wxCoord)( ((*j).x()+0.5)*zoom ) ,y = (wxCoord)( ((*j).y()+0.5)*zoom );
	wxCoord r = (wxCoord)(0.5*zoom);
	if (r==0) r=1;
	if (drawPoints) dc.DrawCircle(x,y,r);
	++j;
	for (;;) {
		for (;j!=(*i).end();++j) {
			wxCoord x1 = (wxCoord)( ((*j).x()+0.5)*zoom ) ,y1 = (wxCoord)( ((*j).y()+0.5)*zoom );
			dc.DrawLine(x,y,x1,y1);
			if (drawPoints) dc.DrawCircle(x1,y1,r);
			x=x1;
			y=y1;
		}
		++i;
		if (i==bnd.end()) break;
		j=(*i).begin();
	}
}
//===============================================================================================
#endif //#if wxUSE_GUI
}

