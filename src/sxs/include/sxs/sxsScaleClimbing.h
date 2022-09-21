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
#ifndef __sxsScaleClimbing_h__
#define __sxsScaleClimbing_h__
//===========================================================
// namespace	: sxs
// class		: ScaleClimbing
/*! \file 
 \brief Main algorithmic class for the computation of a ScaleSets.
 */
//===========================================================
// author		: Laurent Guigues
// history		: 
//	12/07/04		creation
//===========================================================
//===========================================================
#include <limits>
#include "lgl/lglIndexedHeap.h"

#include "sxs/sxsGeneral.h"
#include "sxs/sxsScaleClimbingGraph.h"
#include "sxs/sxsScaleClimbingUser.h"
#include "sxs/sxsScaleSets.h"


namespace sxs {

	//===========================================================
	/// Class holding the parameters of the ScaleClimbing algorithm.
	///
	/// \sa ScaleClimbing
	class ScaleClimbingParameters {
	protected:
		/// Must the graph be handled as a simple graph (no multi-edges) ?
		lgl::BOOL m_simple_graph;
		/// Maximum scale for climbing
		lgl::F32 m_maximum_scale;
		/// Minimum persistence to retain a set in the result
		lgl::F32 m_minimum_persistence;

	public:
		/// Ctor
		ScaleClimbingParameters();
		/// Dtor (is only made virtual to get a polymorphic class which can be dynamically typed (see dynamic downcast in ImageScaleClimbing::setParameters())
		virtual ~ScaleClimbingParameters() {
		}
		/// Returns a reference to a static instance of the class initialized with default values
		//			static ScaleClimbingParameters& static_instance() { static ScaleClimbingParameters p; return p;}

		/// Must the graph be [build and] handled as a simple graph (no multi-edges) ?
		lgl::BOOL getSimpleGraph() const {
			return m_simple_graph;
		}
		/// Maximum scale for climbing
		lgl::F32 getMaximumScale() const {
			return m_maximum_scale;
		}
		/// Minimum persistence to retain a set in the result
		lgl::F32 getMinimumPersistence() const {
			return m_minimum_persistence;
		}

		/// Sets wether the graph must be [build and] handled as a simple graph (no multi-edges) ?
		void setSimpleGraph(lgl::BOOL b = lgl::TRUE) {
			m_simple_graph = b;
		}
		/// Sets the maximum scale for climbing
		void setMaximumScale(lgl::F32 s = (std::numeric_limits<lgl::F32>::max)()) {
			m_maximum_scale = s;
		}
		/// Sets the minimum persistence to retain a set in the result
		void setMinimumPersistence(lgl::F32 p = 1) {
			m_minimum_persistence = p;
		}
	};
	//===========================================================

	//===========================================================
	/// The ScaleClimbing algorithm class.
	///
	/// Builds a ScaleSets by the scale climbing algorithm.
	/// Starts from an initial Graph which represents a region adjacency graph of a fine partition (built by the user).
	/// The energy which is to be minimized is evaluated
	/// via callbacks to a descendant of a ScaleClimbingUser.
	/// Other callbacks concern algorithm controls, structure updates and progress notification.
	///
	/// \sa ign::image::segment::lgl::Graph, ScaleSets, ScaleClimbingUser
	///
	class ScaleClimbing {
	public:
		//===========================================================
		///\name Types definitions
		///
		///\{
		//===========================================================
		/// \brief Minimal attribute for the Nodes of a Graph used in the ScaleClimbing algorithm
		typedef ScaleClimbingNodeAttribute NodeAttribute;
		/// \brief Minimal attribute for the Edges of the Graph used in the ScaleClimbing algorithm
		typedef ScaleClimbingEdgeAttribute EdgeAttribute;
		/// \brief Graph used
		typedef ScaleClimbingGraph Graph;
		/// \shortcut to the Nodes of a Graph
		typedef ScaleClimbingGraph::Node Node;
		/// \shortcut to the Edges of a Graph
		typedef ScaleClimbingGraph::Edge Edge;
		//===========================================================
		/// Shortcut for ScaleClimbingParameters
		//typedef ScaleClimbingParameters Parameters;
		//===========================================================
		/// Shortcut for ScaleSets::Set
		typedef ScaleSets::Set Set;
		//===========================================================
		//===========================================================
		///\}

		//===========================================================
		/// \name Construction, destruction
		/// @{
		//===========================================================
		/// Ctor
		ScaleClimbing();
		/// Dtor
		virtual ~ScaleClimbing();
		//===========================================================
		/// @}
		//===========================================================

		//===========================================================
		/// \name Input (Graph), output (ScaleSets), user (ScaleClimbingUser) and parameters setting (Parameters)
		/// @{
		//===========================================================
		/// Sets the input Graph which should have been created by dynamic allocation.
		/// The object then becomes responsible for its deletion.
		void setGraph(Graph* g);
		/// Sets the output ScaleSets
		void setScaleSets(ScaleSets& s);
		/// Sets the ScaleClimbingUser which should have been created by dynamic allocation.
		/// The object then becomes responsible for its deletion.
		void setUser(ScaleClimbingUser* u);
		/// Sets the Parameters of the algorithm which should have been created by dynamic allocation.
		/// The object then becomes responsible for its deletion.
		void setParameters(const ScaleClimbingParameters* p);
		//===========================================================
		/// Const access to the Graph
		const Graph& getGraph() const {
			return *m_graph;
		}
		/// Access to the graph
		Graph& getGraph() {
			return *m_graph;
		}
		/// Const access to the ScaleSets
		const ScaleSets& getScaleSets() const {
			return *m_scale_sets;
		}
		/// Const access to the User
		const ScaleClimbingUser& getUser() const {
			return *m_user;
		}
		/// Const access to the Parameters
		const ScaleClimbingParameters& getParameters() const {
			return *m_param;
		}
		//===========================================================
		/// @}
		//===========================================================

		//===========================================================
		/// \name Level 0 methods
		/// @{
		//===========================================================
		/// \brief Runs the whole algorithm.
		///
		/// Sequentially calls :
		/// - buildBase()
		///	- buildHeap()
		/// - climb()
		/// - postProcess()
		/// \return TRUE if normal termination
		lgl::BOOL run();
		/// @}
		//===========================================================

	protected:

		//===========================================================
		/// \name Level 1 methods
		/// @{
		//===========================================================
		/// \brief Builds the base of the ScaleSets.
		///
		/// Initializes the energies of the initial graph's Nodes.
		/// \return TRUE if normal termination
		lgl::BOOL buildBase();
		/// \brief Builds the priority queue.
		///
		/// Performs a FDP step on each initial edge and inserts it into the heap.
		/// \return TRUE if normal termination
		lgl::BOOL buildHeap();
		/// \brief Core of the algorithm : builds the ScaleSets by iterative climbing steps until the top is reached (or a stop criterion is met, such as maximum scale reached).
		///
		/// \return TRUE if normal termination
		lgl::BOOL climb();
		/// \brief Postprocessing stage : performs a top-down traversal of the hierarchy to get the Persistent Hierarchy.
		///
		/// \return TRUE if normal termination
		lgl::BOOL postProcess();
		/// \}
		//===========================================================

		//===========================================================
		/// \name Level 2 methods
		/// \{
		//===========================================================
		/// \brief Performs one climbing step.
		///
		/// \return TRUE if climbing must continue.
		/// \param normal_termination set to TRUE iff normal termination
		lgl::BOOL step(lgl::BOOL& normal_termination);
		/// \}
		//===========================================================

		//===========================================================
		/// \name Level 3 methods
		/// \{
		//===========================================================
		/// \brief Contracts the Edge e and updates all the structures (Graph and ScaleSets).
		///
		/// \param e edge to be contracted
		/// \return TRUE if normal termination
		lgl::BOOL contract(Edge e);
		//===========================================================

		//===========================================================
		/// \brief Performs a Functional Dynamic Programming (FDP) step for the set corresponding to the union of the two endnodes of the edge.
		///
		/// This is here that :
		/// - The energetical evaluations occur
		/// - The operations on PiecewiseAffineFunctions occur
		///
		/// Updates the related fields of the edge's attribute (Partial energy, Scale of appearance).
		/// \param e edge for which to compute FDP
		/// \return TRUE if normal termination.
		lgl::BOOL functionalDynamicProgrammingStep(Edge e);
		//===========================================================

		//===========================================================
		/// \brief Tests wether the edge has to be inserted in the Heap.
		///
		/// Is called after a dynamicProgrammingStep(), hence the edge's scale of appearance (soa) has been computed.
		/// In practice, returns m_user->SCCBmustConsider() &&  soa < m_param.maximumScale()
		/// \return TRUE iff the edge e has to be inserted in the Heap.
		lgl::BOOL heapInsertionAllowed(Edge e);
		//===========================================================
		/// \} // EO Level 3 methods
		//===========================================================

		//===========================================================
		/// \name Basic methods
		/// \{
		//===========================================================
		/// \brief Removes the Node from the Graph (deletes its attribute and then removes it).
		inline void removeNode(Node n) {
			delete n.getAttribute();
			m_graph->removeNode(n);
		}
		/// \brief Removes the Edge from the Graph (deletes its attribute and then removes it).
		inline void removeEdge(Edge e) {
			delete e.getAttribute();
			m_graph->removeEdge(e);
		}
		/// \brief Deletes the Attributes of all the Graph elements
		void deleteGraphAttributes();
		//===========================================================
		//===========================================================
		/// \} // EO Basic methods
		//===========================================================

		//===========================================================
		/// \name Nested Classes
		// \{
		//===========================================================

		//===========================================================
		/// \brief [SYSTEM class] Ordering function on Gr::Edge according to their attribute's scaleOfAppearance() (IndexedHeap sorting).
		class AppearanceOrder {
		public:
			AppearanceOrder() {
			}
			lgl::BOOL operator()(Edge e1, Edge e2) const {
				return (e1.getAttribute()->scaleOfAppearance() < e2.getAttribute()->scaleOfAppearance());
			}
		};
		//===========================================================

		//===========================================================
		/// \brief [SYSTEM class] Index function on Gr::Edge according to their attributes's heapIndex() (for IndexedHeap sorting).
		class EdgeIndex {
		public:
			EdgeIndex() {
			}
			lgl::I32& operator()(Edge e) const {
				return e.getAttribute()->heapIndex();
			}
		};
		//===========================================================

		// \}
		//===========================================================

		//===========================================================
		// Data
		//===========================================================
		/// The graph
		Graph* m_graph;
		/// The ScaleSets
		ScaleSets* m_scale_sets;
		/// The user
		ScaleClimbingUser* m_user;
		/// The algorithm parameters
		const ScaleClimbingParameters* m_param;
		//===========================================================

		//===========================================================
		/// Comparator for Edge sorting in the IndexedHeap
		AppearanceOrder m_compare;
		/// Heap indexer
		EdgeIndex m_index;
		/// Indexed Edge Heap for priority queue region merging
		lgl::IndexedHeap<Edge, AppearanceOrder, EdgeIndex> m_heap;
		//===========================================================

		//===========================================================
		/// A 1 bit Slot on m_graph's Nodes to mark the neigbours of a Node during contraction
		Graph::Slot m_neigb;
		//===========================================================

		//===========================================================
		// Progress indicator variables
		/// Stage of the current task
		lgl::I32 m_stage;
		/// Total number of stages for the current task
		lgl::I32 m_number_of_stages;
		//===========================================================

	private:
		/// No copy ctor allowed
		ScaleClimbing(const ScaleClimbing&); // not implemented
		/// No affectation allowed
		void operator =(const ScaleClimbing&); // not implemented

	};
//===========================================================
// EO class ScaleClimbing
//===========================================================

}

//===========================================================
// EOF
//===========================================================
#endif
