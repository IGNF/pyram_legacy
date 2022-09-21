/* 

 lGl : A C++ library needed by the multiscale image segmentation library SxS
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
 \brief General unoriented graph structures
 */
#ifndef __sxsGraph_h__
#define __sxsGraph_h__
//=============================================================================
// namespace : lgl
// class     : Graph
//  Unoriented graphs (however in which the direction of travel might be important) which allow :
//  - fast iteration of incident edges (by linking)
//  - elements (node/edge) attribution (template)
//	- elements marking
//	- fast edge contraction
// author    : Laurent Guigues
// history   :
//  09/07/2004 Creation
//=============================================================================
#include "lgl/lglGeneral.h"
#include "lgl/lglBitManager.h"
#include "lgl/lglSparseArray.h"

namespace lgl {

	//===========================================================
	/// Graph
	template<class NodeAttribute, class EdgeAttribute> class Graph;
	/// Physical graph edges (fixed orientation)
	template<class NodeAttribute, class EdgeAttribute> class PhysicalGraphEdge;
	/// Logical graph edges = pointer on a PhysicalGraphEdge and orientation
	template<class NodeAttribute, class EdgeAttribute> class LogicalGraphEdge;
	/// Physical graph nodes
	template<class NodeAttribute, class EdgeAttribute> class PhysicalGraphNode;
	/// Logical graph nodes
	template<class NodeAttribute, class EdgeAttribute> class LogicalGraphNode;
	//===========================================================
	/// Iterator of the edges connected to a node
	template<class NodeAttribute, class EdgeAttribute> class IncidentEdgesIterator;
	/// Iterator of the nodes connected to an edge
	template<class NodeAttribute, class EdgeAttribute> class IncidentNodesIterator;
	//===========================================================
	/// Virtual container of the Nodes of a graph
	template<class NodeAttribute, class EdgeAttribute> class GraphNodesContainer;
	/// Iterator of a GraphNodesContainer
	template<class NodeAttribute, class EdgeAttribute> class GraphNodesContainerIterator;
	/// Virtual container of the Edges of a graph (in a given orientation)
	template<class NodeAttribute, class EdgeAttribute> class GraphEdgesContainer;
	/// Iterator of a GraphEdgesContainer
	template<class NodeAttribute, class EdgeAttribute> class GraphEdgesContainerIterator;
	//===========================================================

	//===========================================================
	/// A macro defining the common typedefs to all graph elements
#define lglGRAPH_TYPEDEFS\
	typedef Graph<NodeAttribute,EdgeAttribute>				TGraph;\
	typedef PhysicalGraphNode<NodeAttribute,EdgeAttribute>	PhNode;\
	typedef PhysicalGraphEdge<NodeAttribute,EdgeAttribute>	PhEdge;\
	typedef LogicalGraphNode<NodeAttribute,EdgeAttribute>	Node;\
	typedef LogicalGraphEdge<NodeAttribute,EdgeAttribute>	Edge;\
	typedef PhNode	*pPhNode;\
	typedef Node	*pNode;\
	typedef PhEdge	*pPhEdge;\
	typedef Edge	*pEdge;\
	typedef GraphNodesContainer<NodeAttribute,EdgeAttribute> NodeSet;\
	typedef GraphEdgesContainer<NodeAttribute,EdgeAttribute> EdgeSet;\
	typedef	I32 Index;\
	typedef	UI8 Mark;\
	typedef lgl::BitSlot Slot;\
	typedef UI8 Orientation;
	//=============================================================================

	//=============================================================================
	// class Graph
	//=============================================================================
	//=============================================================================
	/// \brief A general unoriented Graph structure.
	///
	/// They are the graphs upon which region merging based image segmentation algorithms can work efficiently.
	/// They allow :
	///		- fast iteration of incident edges (by linking)
	///		- elements (node/edge) attribution (template)
	///		- elements marking
	///		- fast edge contraction
	template<class NodeAttribute, class EdgeAttribute>
	class Graph {

	public:
		//===========================================================
		/// Common typedefs to the lglGraph classes
		lglGRAPH_TYPEDEFS
		//===========================================================
		//===========================================================
		/// Returns a constant representing Direct edge orientation
static					Orientation DirectOrientation() {return 0;}
		/// Returns a constant representing Reverse (indirect) edge orientation
		static Orientation ReverseOrientation() {return 1;}
		//===========================================================

		//===========================================================
		// Friendship
		FRIEND_CLASS GraphNodesContainer<NodeAttribute,EdgeAttribute>;
		FRIEND_CLASS GraphEdgesContainer<NodeAttribute,EdgeAttribute>;
		//===========================================================

		//===========================================================
		/// ctor
		Graph();
		/// dtor
		~Graph();
		//===========================================================

		//===========================================================
		/// Empties the Graph
		void clear();
		//===========================================================
		/// Returns the amount of memory used by the node/edge pointers arrays.
		/// Not the total amount of memory used as the node/edge structures themselves are not counted !!!
		UI32 memoryUsage(BOOL verbose) {return ( m_node.memoryUsage(verbose) + m_edge.memoryUsage(verbose));}
		//===========================================================

		//===========================================================
		/// returns a container of all the nodes
		inline NodeSet getNodes() const {return NodeSet(this);}
		/// returns a container of all the edges in orientation ori
		inline EdgeSet getEdges(Orientation o=DirectOrientation()) const {return EdgeSet(this,o);}
		//===========================================================

		//===========================================================
		/// Adds a Node to the Graph.
		/// If the Index is already occupied : returns Null Node
		Node addNode(Index i);
		/// Adds a Node to the Graph.
		/// If the Index is already occupied : returns Null Node
		Node addNode(Index i, const NodeAttribute& a);
		/// Adds an Edge from Node nf to Node nt.
		/// If any problem (Null nodes or used index) : returns Null Edge
		Edge addEdge(Node nf, Node nt, Index i);
		/// Adds an Edge from Node nf to Node nt.
		/// If any problem (Null nodes or used index) : returns Null Edge
		Edge addEdge(Node nf, Node nt, Index i, const EdgeAttribute& a);
		//===========================================================

		//===========================================================
		/// Removes the Node from the Graph and all its incident Edges
		void removeNode(Node n);
		/// Removes the Edge from the Graph
		void removeEdge(Edge n);
		//===========================================================

		//===========================================================
		/// Edge relinking : Changes the Edge's e from() Node to new_origin.
		void changeNodeFrom(Edge e, Node new_origin);
		/// Edge relinking : Changes the Edge's e to() Node to new_end.
		void changeNodeTo(Edge e, Node new_end);
		//===========================================================

		//===========================================================
		/// returns the Node of Index i
		inline Node getNode(Index i) {return Node(m_node[i]);}
		/// returns the Edge of Index i
		inline Edge getEdge(Index i, Orientation o=DirectOrientation()) {return Edge(m_edge[i],o);}
		//===========================================================

		//================================================================
		/// allocates a Node Slot of l bits
		BOOL allocateNodeSlot ( Slot& s, UI8 length = 1) {return m_node_marker.allocate(length,s);}
		/// allocates an Edge Slot of l bits
		BOOL allocateEdgeSlot ( Slot& s, UI8 length = 1) {return m_edge_marker.allocate(length,s);}
		/// frees the Node Slot s
		void freeNodeSlot ( Slot& s ) {m_node_marker.free(s);}
		/// frees the Edge Slot s
		void freeEdgeSlot ( Slot& s ) {m_edge_marker.free(s);}
		//================================================================

	protected:
		//===========================================================
		/// Array of nodes
		lgl::SparseArray<pPhNode> m_node;
		/// Array of edges
		lgl::SparseArray<pPhEdge> m_edge;
		//===========================================================
		/// Number of nodes in the graph
		Index m_nbn;
		/// Number of edges in the graph
		Index m_nbe;
		//===========================================================
		/// Bit manager for the nodes marks
		lgl::BitManager< Mark > m_node_marker;
		/// Bit manager for the edges marks
		lgl::BitManager< Mark > m_edge_marker;
		//===========================================================

	};
	//===========================================================
	// EO class Graph
	//===========================================================

	//===========================================================
	// class PhysicalGraphNode
	//===========================================================

	//===========================================================
	template<class NodeAttribute, class EdgeAttribute>
	class PhysicalGraphNode {

	public:
		//===========================================================
		/// Common typedefs to the lglGraph classes
		lglGRAPH_TYPEDEFS
		//===========================================================

		//===========================================================
		// Frienship
FRIEND_CLASS					Graph<NodeAttribute,EdgeAttribute>;
		FRIEND_CLASS PhysicalGraphEdge<NodeAttribute,EdgeAttribute>;
		FRIEND_CLASS LogicalGraphNode<NodeAttribute,EdgeAttribute>;
		//===========================================================

		//===========================================================
	protected:
		//===========================================================
		/// index of the node
		Index m_index;
		/// attribute
		NodeAttribute m_attr;
		/// marks
		Mark m_mark;
		/// first logical edge out (contains a ptr on physical edge and orientation)
		Edge m_first_edge;
		//===========================================================
		/// ctor
		PhysicalGraphNode(Index i);
		/// ctor
		PhysicalGraphNode(Index i, const NodeAttribute& a);
		//===========================================================

	};
	//===========================================================
	// EO class PhysicalGraphNode
	//===========================================================

	//===========================================================
	// class LogicalGraphNode
	//===========================================================

	//===========================================================
	template<class NodeAttribute, class EdgeAttribute>
	class LogicalGraphNode {

	public:
		//===========================================================
		/// Common typedefs to the lglGraph classes
		lglGRAPH_TYPEDEFS
		//===========================================================

		//===========================================================
		// Frienship
FRIEND_CLASS					Graph<NodeAttribute,EdgeAttribute>;
		FRIEND_CLASS GraphNodesContainerIterator<NodeAttribute,EdgeAttribute>;
		FRIEND_CLASS LogicalGraphEdge<NodeAttribute,EdgeAttribute>;
		//===========================================================

		//===========================================================
		// Iterators typedefs
		typedef IncidentEdgesIterator<NodeAttribute,EdgeAttribute> iterator;
		typedef IncidentEdgesIterator<NodeAttribute,EdgeAttribute> const_iterator;
		//===========================================================

		//===========================================================
		/// Default ctor : builds a Null Node
		LogicalGraphNode() : m_phnode(NULL) {}
		/// Copy ctor
		LogicalGraphNode(const LogicalGraphNode& n) : m_phnode(n.m_phnode) {}
		/// dtor
		~LogicalGraphNode() {}
		//===========================================================

		//===========================================================
		/// static method returning a const ref to the Null Node
		static const LogicalGraphNode & Null() {static const LogicalGraphNode n; return n;}
		//===========================================================

		//===========================================================
		/// equal operator
		inline BOOL operator == (const LogicalGraphNode& n) const {return (m_phnode==n.m_phnode);}
		/// not equal operartor
		inline BOOL operator != (const LogicalGraphNode& n) const {return (m_phnode!=n.m_phnode);}
		//===========================================================

		//===========================================================
		// Accessors
		/// returns the Node's Index
		Index getIndex() const {lglASSERT(m_phnode!=NULL); return m_phnode->m_index;}
		/// returns a ref on the Node's Attribute
		NodeAttribute& getAttribute() {lglASSERT(m_phnode!=NULL); return m_phnode->m_attr;}
		/// returns a const ref on the Node's Attribute
		const NodeAttribute& getAttribute() const {lglASSERT(m_phnode!=NULL); return m_phnode->m_attr;}
		//===========================================================

		//===========================================================
		/// incident edges iterator begin
		iterator begin() {lglASSERT(m_phnode!=NULL); return iterator(m_phnode->m_first_edge);}
		/// incident edges const iterator begin
		const_iterator begin() const {lglASSERT(m_phnode!=NULL); return const_iterator(m_phnode->m_first_edge);}
		/// incident edges iterator end
		iterator end() {lglASSERT(m_phnode!=NULL); return iterator(Edge::Null());}
		/// incident edges const iterator end
		const_iterator end() const {lglASSERT(m_phnode!=NULL); return iterator(Edge::Null());}
		//===========================================================

		//===========================================================
		// Marks management
		/// Sets the mark of Slot s to v
		inline void setMark(Slot s, Mark v) {lglASSERT(m_phnode!=NULL); lgl::BitManager<Mark>::set(m_phnode->m_mark,s,v);}
		/// Gets the value of the mark of Slot s
		inline Mark getMark(Slot s) {lglASSERT(m_phnode!=NULL); return lgl::BitManager<Mark>::get(m_phnode->m_mark,s);}
		//===========================================================

	protected:
		//===========================================================
		/// pointer on the Physical Node
		pPhNode m_phnode;
		/// ctor with a Physical Node pointer
		LogicalGraphNode( pPhNode n ) : m_phnode(n) {}
		//===========================================================
	};
	//===========================================================
	// EO class LogicalGraphNode
	//===========================================================

	//===========================================================
	// class PhysicalGraphEdge
	//===========================================================
	template<class NodeAttribute, class EdgeAttribute>
	class PhysicalGraphEdge {
	public:
		//===========================================================
		/// Common typedefs to the lglGraph classes
		lglGRAPH_TYPEDEFS
		//===========================================================

		//===========================================================
		// Friendship
FRIEND_CLASS					LogicalGraphEdge<NodeAttribute,EdgeAttribute>;
		FRIEND_CLASS Graph<NodeAttribute,EdgeAttribute>;
		FRIEND_CLASS IncidentEdgesIterator<NodeAttribute,EdgeAttribute>;
		//===========================================================

	protected:
		/// index
		Index m_index;
		/// Attribute
		EdgeAttribute m_attr;
		/// marks
		Mark m_mark;
		/// pointer on physical node from (0) and node to (1)
		pPhNode m_node[2];
		/// next Logical Edge (oriented) around node from (0) and node to (1)
		Edge m_next_edge[2];
		//===========================================================
		/// ctor
		PhysicalGraphEdge(pPhNode n1, pPhNode n2, Index i);
		/// ctor
		PhysicalGraphEdge(pPhNode n1, pPhNode n2, Index i, const EdgeAttribute& a);
		//===========================================================

	};
	//===========================================================
	// EO class PhysicalGraphEdge
	//===========================================================

	//===========================================================
	// class LogicalGraphEdge
	//===========================================================

	//===========================================================
	template<class NodeAttribute, class EdgeAttribute>
	class LogicalGraphEdge {
	public:
		//===========================================================
		/// Common typedefs to the lglGraph classes
		lglGRAPH_TYPEDEFS
		//===========================================================

		//===========================================================
		// Frienship
FRIEND_CLASS					Graph<NodeAttribute,EdgeAttribute>;
		FRIEND_CLASS PhysicalGraphEdge<NodeAttribute,EdgeAttribute>;
		FRIEND_CLASS IncidentEdgesIterator<NodeAttribute,EdgeAttribute>;
		FRIEND_CLASS GraphEdgesContainerIterator<NodeAttribute,EdgeAttribute>;
		//===========================================================

		//===========================================================
		// iterators typedefs
		typedef IncidentNodesIterator<NodeAttribute,EdgeAttribute> iterator;
		typedef IncidentNodesIterator<NodeAttribute,EdgeAttribute> const_iterator;
		//===========================================================

		//===========================================================
		/// Default ctor : builds a Null Edge
		LogicalGraphEdge() : m_phedge(NULL), m_ori(TGraph::DirectOrientation()) {}
		/// Copy ctor
		LogicalGraphEdge(const LogicalGraphEdge& e) : m_phedge(e.m_phedge), m_ori(e.m_ori) {}
		/// dtor
		~LogicalGraphEdge() {}
		//===========================================================

		//===========================================================
		/// static method returning a const ref to the Null Edge
		static const LogicalGraphEdge & Null() {static const LogicalGraphEdge e; return e;}
		//===========================================================

		//===========================================================
		/// equal operator
		inline BOOL operator == (const LogicalGraphEdge& e) const {return ((m_phedge==e.m_phedge)&&(m_ori==e.m_ori));}
		/// not equal operator
		inline BOOL operator != (const LogicalGraphEdge& e) const {return ((m_phedge!=e.m_phedge)||(m_ori!=e.m_ori));}
		//===========================================================

		//===========================================================
		// Accessors
		/// returns the Edge's Index
		Index getIndex() const {lglASSERT(m_phedge!=NULL) return m_phedge->m_index;}
		/// returns a ref on the Edge's Attribute
		EdgeAttribute& getAttribute() {lglASSERT(m_phedge!=NULL) return m_phedge->m_attr;}
		/// returns a const ref on the Edge's Attribute
		const EdgeAttribute& getAttribute() const {lglASSERT(m_phedge!=NULL) return m_phedge->m_attr;}
		//===========================================================

		//===========================================================
		/// Node from
		Node getInitialNode() const {lglASSERT(m_phedge!=NULL) return Node(m_phedge->m_node[m_ori]);}
		/// Node to
		Node getFinalNode() const {lglASSERT(m_phedge!=NULL) return Node(m_phedge->m_node[1-m_ori]);}
		//===========================================================

		//===========================================================
		/// Orientation
		Orientation getOrientation() const {return m_ori;}
		/// Reverse orientation
		void reverseOrientation() {m_ori = 1-m_ori;}
		/// returns the reversed edge
		Edge getReverseEdge() const {return Edge(m_phedge,1-m_ori);}
		//===========================================================

		//===========================================================
		/// connected nodes iterator begin
		iterator begin();
		/// connected nodes const iterator begin
		const_iterator begin() const;
		/// connected nodes iterator end
		iterator end();
		/// connected nodes const iterator end
		const_iterator end() const;
		//===========================================================

		//===========================================================
		// Marks management
		/// Sets the mark of Slot s to v
		inline void setMark(Slot s, Mark v) {lglASSERT(m_phedge!=NULL); lgl::BitManager<Mark>::set(m_phedge->m_mark,s,v);}
		/// Gets the value of the mark of Slot s
		inline Mark getMark(Slot s) {lglASSERT(m_phedge!=NULL); return lgl::BitManager<Mark>::get(m_phedge->m_mark,s);}
		//===========================================================

		//===========================================================
	protected:
		//===========================================================
		/// corresponding physical edge
		pPhEdge m_phedge;
		/// orientation
		Orientation m_ori;
		/// ctor with a Physical Edge pointer
		LogicalGraphEdge( pPhEdge e, Orientation o=TGraph::DirectOrientation() ) : m_phedge(e), m_ori(o) {}
		//===========================================================
	};
	//===========================================================
	// EO class LogicalGraphEdge
	//===========================================================

	//===========================================================
	//===========================================================
	// Stream insertion of graph elements
	//===========================================================
	//===========================================================
	template<class N, class E>
	std::ostream& operator <<(std::ostream& o, const LogicalGraphNode<N, E>& n) {
		o << "(" << n.getIndex() << ")";
		return o;
	}
	//===========================================================

	//===========================================================
	template<class N, class E>
	std::ostream& operator <<(std::ostream& o, const LogicalGraphEdge<N, E>& e) {
		o << "(" << e.getIndex() << ":" << e.getInitialNode().getIndex() << "-" << e.getFinalNode().getIndex() << ")";
		return o;
	}
	//===========================================================

	//===========================================================
	template<class N, class E>
	std::ostream& operator <<(std::ostream& o, const GraphNodesContainer<N, E>& n) {
		o << "(";
		typename GraphNodesContainer<N, E>::const_iterator i;
		for (i = n.begin(); i != n.end(); ++i)
			o << (*i);
		o << ")";
		return o;
	}
	//===========================================================

	//===========================================================
	template<class N, class E>
	std::ostream& operator <<(std::ostream& o, const GraphEdgesContainer<N, E>& n) {
		o << "(";
		typename GraphEdgesContainer<N, E>::const_iterator i;
		for (i = n.begin(); i != n.end(); ++i)
			o << (*i);
		o << ")";
		return o;
	}
	//===========================================================

	//===========================================================
	//===========================================================
	//===========================================================
	//===========================================================
	//===========================================================
	// Containers and iterators
	//===========================================================
	//===========================================================
	//===========================================================
	//===========================================================

	//===========================================================
	/// Iterator of the edges connected to a node
	template<class NodeAttribute, class EdgeAttribute>
	class IncidentEdgesIterator {
		typedef LogicalGraphEdge<NodeAttribute, EdgeAttribute> Edge;
	protected:
		Edge m_cur;
	public:
		// Edge Conflicts with
		inline IncidentEdgesIterator(const Edge& e = Edge::Null()) :
				m_cur(e) {
		}
		//
		inline Edge operator *() const {
			return m_cur;
		}
		inline IncidentEdgesIterator& operator ++() {
			m_cur = m_cur.m_phedge->m_next_edge[m_cur.m_ori];
			return *this;
		}
		inline BOOL operator !=(const IncidentEdgesIterator& i) {
			return (m_cur != i.m_cur);
		}
	};
	//===========================================================
	// EO IncidentEdgesIterator
	//===========================================================

	//===========================================================
	/// Iterator of the nodes connected to an edge
	template<class NodeAttribute, class EdgeAttribute>
	class IncidentNodesIterator {
	};
	//===========================================================
	// EO IncidentNodesIterator
	//===========================================================

	//===========================================================
	/// Virtual container of the Nodes of a graph
	template<class NodeAttribute, class EdgeAttribute>
	class GraphNodesContainer {
		FRIEND_CLASS Graph<NodeAttribute, EdgeAttribute> ;
	protected:
		const Graph<NodeAttribute, EdgeAttribute>* m_g;
		GraphNodesContainer(const Graph<NodeAttribute, EdgeAttribute>* g) :
				m_g(g) {
		}
	public:
		typedef GraphNodesContainerIterator<NodeAttribute, EdgeAttribute> iterator;
		typedef GraphNodesContainerIterator<NodeAttribute, EdgeAttribute> const_iterator;

		//===========================================================
		inline I32 size() const {
			return m_g->m_nbn;
		}
		//===========================================================
		inline iterator begin() {
			return iterator(m_g->m_node.begin());
		}
		inline const_iterator begin() const {
			return const_iterator(m_g->m_node.begin());
		}
		inline iterator end() {
			return iterator(m_g->m_node.end());
		}
		inline const_iterator end() const {
			return const_iterator(m_g->m_node.end());
		}
		//===========================================================

	};
	//===========================================================
	// EO GraphNodesContainer
	//===========================================================

	//===========================================================
	/// Iterator of a GraphNodesContainer
	template<class NodeAttribute, class EdgeAttribute>
	class GraphNodesContainerIterator {
		typedef LogicalGraphNode<NodeAttribute, EdgeAttribute> Node;
		typedef PhysicalGraphNode<NodeAttribute, EdgeAttribute>* pPhNode;FRIEND_CLASS GraphNodesContainer<NodeAttribute, EdgeAttribute> ;
	protected:
		typename lgl::SparseArray<pPhNode>::const_iterator m_i;
		GraphNodesContainerIterator(const typename lgl::SparseArray<pPhNode>::const_iterator& i) :
				m_i(i) {
		}
	public:
		GraphNodesContainerIterator() {
		}

		inline Node operator *() const {
			return Node(*m_i);
		}
		inline GraphNodesContainerIterator& operator ++() {
			++m_i;
			return *this;
		}
		inline BOOL operator !=(const GraphNodesContainerIterator& i) {
			return (m_i != i.m_i);
		}
	};
	//===========================================================
	// EO GraphNodesContainerIterator
	//===========================================================

	//===========================================================
	/// Virtual container of the Edges of a graph (in a given orientation)
	template<class NodeAttribute, class EdgeAttribute>
	class GraphEdgesContainer {
	public:
		//===========================================================
		/// Common typedefs to the lglGraph classes
		lglGRAPH_TYPEDEFS
		//===========================================================
FRIEND_CLASS					Graph<NodeAttribute,EdgeAttribute>;

		typedef GraphEdgesContainerIterator<NodeAttribute,EdgeAttribute> iterator;
		typedef GraphEdgesContainerIterator<NodeAttribute,EdgeAttribute> const_iterator;

		//===========================================================
		inline I32 size() const {return m_g->m_nbe;}
		//===========================================================
		inline iterator begin() {return iterator(m_g->m_edge.begin(),m_o);}
		inline const_iterator begin() const {return const_iterator( m_g->m_edge.begin(),m_o);}
		inline iterator end() {return iterator(m_g->m_edge.end(),m_o);}
		inline const_iterator end() const {return const_iterator(m_g->m_edge.end(),m_o);}
		//===========================================================

	protected:
		const Graph<NodeAttribute,EdgeAttribute>* m_g;
		Orientation m_o;
		GraphEdgesContainer(const Graph<NodeAttribute,EdgeAttribute>* g, Orientation o) : m_g(g), m_o(o) {}

	};
	//===========================================================
	// EO GraphEdgesContainer
	//===========================================================

	//===========================================================
	/// Iterator of a GraphEdgesContainer
	template<class NodeAttribute, class EdgeAttribute>
	class GraphEdgesContainerIterator {
	public:
		//===========================================================
		/// Common typedefs to the lglGraph classes
		lglGRAPH_TYPEDEFS
		//===========================================================
FRIEND_CLASS	GraphEdgesContainer<NodeAttribute,EdgeAttribute>;
		//===========================================================

		//===========================================================
		GraphEdgesContainerIterator() {}
		//===========================================================

		//===========================================================
		inline Edge operator * () const {return Edge(*m_i,m_o);}
		inline GraphEdgesContainerIterator& operator ++ () {++m_i; return *this;}
		inline BOOL operator != ( const GraphEdgesContainerIterator& i ) {return (m_i != i.m_i);}
		//===========================================================

	protected :
		typename lgl::SparseArray<pPhEdge>::const_iterator m_i;
		Orientation m_o;
		GraphEdgesContainerIterator(const typename lgl::SparseArray<pPhEdge>::const_iterator& i, Orientation o) : m_i(i), m_o(o) {}

	};
//===========================================================
// EO GraphEdgesContainerIterator
//===========================================================

//===========================================================

#include "lgl/lglGraph_code.h"

}
//===========================================================
// EO namespace sxs
//===========================================================

//===========================================================
// EOF
//===========================================================
#endif
