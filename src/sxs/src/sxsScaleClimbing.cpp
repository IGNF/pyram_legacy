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
 \brief Code of the methods of the class ScaleClimbing and of its nested classes.
 */
#include <set>
#include <iterator>
#include <algorithm>
#include <sxs/sxsScaleClimbing.h>

namespace sxs {

	//=================================================================================
	ScaleClimbing::ScaleClimbing() :
			m_graph(0), m_scale_sets(0), m_user(0), m_param(0) {
	}
	//=================================================================================

	//=================================================================================
	ScaleClimbing::~ScaleClimbing() {
		if (m_graph) {
			deleteGraphAttributes();
			delete m_graph;
		}
		if (m_user)
			delete m_user;
		if (m_param)
			delete m_param;
	}
	//=================================================================================

	//=================================================================================

	void ScaleClimbing::setGraph(Graph* g) {
		if (m_graph) {
			deleteGraphAttributes();
			delete m_graph;
		}
		m_graph = g;
	}
	//=================================================================================

	//=================================================================================
	void ScaleClimbing::deleteGraphAttributes() {
		Graph::NodeSet::iterator nit;
		for (nit = m_graph->getNodes().begin(); nit != m_graph->getNodes().end(); ++nit)
			delete (*nit).getAttribute();
		Graph::EdgeSet::iterator eit;
		for (eit = m_graph->getEdges().begin(); eit != m_graph->getEdges().end(); ++eit)
			delete (*eit).getAttribute();
	}
	//=================================================================================

	//=================================================================================
	void ScaleClimbing::setScaleSets(ScaleSets& s) {
		m_scale_sets = &s;
	}
	//=================================================================================

	//=================================================================================
	void ScaleClimbing::setUser(ScaleClimbingUser* u) {
		if (m_user)
			delete m_user;
		m_user = u;
		m_user->setScaleClimbing(this);
	}
	//=================================================================================

	//=================================================================================
	void ScaleClimbing::setParameters(const ScaleClimbingParameters* p) {
		if (m_param)
			delete m_param;
		m_param = p;
	}
	//=================================================================================

	//=================================================================================
	//=================================================================================
	//=================================================================================
	lgl::BOOL ScaleClimbing::run() {
		//==========================================
		//testRunningPossible();
		//==========================================
		// CB begin
		m_user->SCCB_N_preRun();
		//==========================================
		// Base construction
		if (!buildBase())
			return false;
		// Heap construction
		if (!buildHeap())
			return false;
		// Climbing
		if (!climb())
			return false;
		// Post-processing to get the persistent hierarchy
		if (!postProcess())
			return false;
		//==========================================
		// CB end
		m_user->SCCB_N_postRun();
		//==========================================
		return true;
		//==========================================
	}
	//=================================================================================

	//=================================================================================
	lgl::BOOL ScaleClimbing::buildBase() {
		//==========================================
		// If the graph doesn't exit : return
		if (m_graph == 0) {
#ifndef LGL_WITH_BOOST
			m_user->SCCB_N_error(lgl::String(wxT("call to buildBase although the graph has not been built (NULL ptr)")));
#else
			m_user->SCCB_N_error(lgl::String("call to buildBase although the graph has not been built (NULL ptr)"));
#endif
			return false;
		}
		//==========================================

		//==========================================
		// CB pre
		m_user->SCCB_N_preBuildBaseNotification();
		//==========================================

		//==========================================
		/// if the graph must be kept simple : reserves a bit on g's Node for neigbours marking during contraction
		if (m_param->getSimpleGraph()) {
			
			if (!m_graph->allocateNodeSlot(m_neigb, 1)) {
#ifndef LGL_WITH_BOOST
				m_user->SCCB_N_error(lgl::String(wxT("unable to allocate a 1 bit slot on the graph's nodes !")));
#else
				m_user->SCCB_N_error(lgl::String("unable to allocate a 1 bit slot on the graph's nodes !"));
#endif
				return false;
			}
		}
		//==========================================

		//==========================================
		// Progress
		m_number_of_stages = m_graph->getNodes().size();
		m_stage = 0;
		//==========================================

		//==========================================
		Graph::NodeSet::iterator i;
		for (i = m_graph->getNodes().begin(); i != m_graph->getNodes().end(); ++i) {
			if (m_user->SCCB_Q_mustConsider(*i)) {
				//==========================================
				// new base Set (scale 0)
				Set* s = new Set((*i).getIndex(), 0);
				// attribute allocation
				s->setAttribute(m_user->SCCB_Q_newSetAttribute());
				// insertion
				m_scale_sets->insert(s);
				// pointed by the node
				(*i).getAttribute()->sxspSet() = (void*) s;
				//==========================================
				// CB user's attribution
				m_user->SCCB_C_onBaseSetCreation(*i, s);
				m_user->SCCB_D_onBaseSetCreation(*i, s);
				//==========================================
				// energy initialization of the node
				//	std::cout << "C_eval ";
				lgl::F32 C = m_user->SCCB_C_eval(*i);
				//	std::cout << "ok";
				lgl::F32 D = m_user->SCCB_D_eval(*i);
				//	std::cout << "ok";
				(*i).getAttribute()->ppartialEnergy() = new PiecewiseAffineFunction(AffinePiece(0, D, C));
				//	std::cout << "ok";
				//==========================================
			}
			m_stage++;
			if (!m_user->SCCB_N_progressIndicator(m_stage, m_number_of_stages))
				return false;
		}

		//==========================================

		//==========================================
		// CB post
		m_user->SCCB_N_postBuildBaseNotification();
		//==========================================

		return true;
	}
	//=================================================================================

	//=================================================================================
	lgl::BOOL ScaleClimbing::buildHeap() {
		//==========================================
		// CB pre
		m_user->SCCB_N_preBuildHeapNotification();
		//==========================================

		//==========================================
		// Progress
		m_number_of_stages = m_graph->getEdges().size();
		m_stage = 0;
		//==========================================

		//==========================================
		// Edges traversal
		Graph::EdgeSet::iterator ite;
		for (ite = m_graph->getEdges().begin(); ite != m_graph->getEdges().end(); ++ite) {
			// If edge is to be considered for merging
			if (m_user->SCCB_Q_mustConsider(*ite)) {
				// performs a dynamic programming step to compute
				// the edge's scale of appearance (soa)
				functionalDynamicProgrammingStep((*ite));
				// if heap insertion allowed => inserts the edge
				if (heapInsertionAllowed(*ite))
					m_heap.insert((*ite));
			}
			m_stage++;
			if (!m_user->SCCB_N_progressIndicator(m_stage, m_number_of_stages))
				return false;
		}
		//==========================================

		//==========================================
		// CB post
		m_user->SCCB_N_postBuildHeapNotification();
		//==========================================

		return true;
	}
	//=================================================================================

	//=================================================================================
	lgl::BOOL ScaleClimbing::climb() {
		//==========================================
		// CB pre
		m_user->SCCB_N_preClimbingNotification();
		//==========================================

		//==========================================
		// Progress
		m_number_of_stages = m_graph->getNodes().size() - 1;
		m_stage = 0;
		//==========================================

		//==========================================
		// Climbing steps iteration until must stop (false return value)
		lgl::BOOL normal_termination;
		while (step(normal_termination)) {
			m_stage++;
#ifdef __SXS_CLIMBING_TESTS__
			if ( ! (((int) (m_stage*10000./m_number_of_stages) ) % 100 ) ) {
				lglLOG( "         testing\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" );
				// tests the existence of loops (should not exist !)
				if (GraphHasLoops(*m_graph)) {
					m_user->SCCB_N_warning(0,lgl::String("Graph has loops !!"));
					//				std::cout << "Graph has loops !!";
				}
				// tests the existence of multiple edges (should not exist !)
				if ( (m_param->getSimpleGraph()) && (!GraphIsSimple(*m_graph)) ) {
					m_user->SCCB_N_warning(0,lgl::String("Graph has loops !!"));
					//				std::cout << "Graph is not simple !!";
				}
			}
#endif
			if (!m_user->SCCB_N_progressIndicator(m_stage, m_number_of_stages))
				return false;
		}
		if (!normal_termination)
			return false;
		//==========================================

		//==========================================
		// CB post
		m_user->SCCB_N_postClimbingNotification();
		//==========================================

		return true;
	}
	//=================================================================================

	//=================================================================================
	lgl::BOOL ScaleClimbing::postProcess() {
		//==========================================
		// CB pre
		m_user->SCCB_N_prePostprocessingNotification();
		//==========================================

		// [RTS]: if climbing was stopped before its normal end, there may be
		// several "roots" in the hierarchy. We now join them all as children of
		// a new root node which is given an arbitrary scale of apparition
		std::set<sxs::ScaleSets::Set *> setexists;     // set of Set* in the hierarchy
		std::set<sxs::ScaleSets::Set *> ischild;       // set of Set* who are a child of another Set
		double max_soa = -1;
		bool max_soa_set = false; // maximum found scale of apparition
		std::list<sxs::ScaleSets::Set *>::const_iterator li;
		for (li = m_scale_sets->Sets().begin(); li != m_scale_sets->Sets().end(); ++li) {
			sxs::ScaleSets::Set * elset = *li;
			if (elset == NULL)
				continue;
			setexists.insert(elset);
			if (elset->scaleOfAppearance() > m_param->getMaximumScale() * 0.1)
				elset->scaleOfAppearance() = m_param->getMaximumScale() * 0.1;
			if (!max_soa_set) {
				max_soa = (double) (elset->scaleOfAppearance());
				max_soa_set = true;
			}
			max_soa = std::max(max_soa, (double) (elset->scaleOfAppearance()));
			const std::vector<sxs::ScaleSets::Set *> & fills = elset->sons();
			for (unsigned int i = 0; i < fills.size(); ++i)
				ischild.insert(fills[i]);
		}
		std::set<sxs::ScaleSets::Set *> setroots;      // root_nodes = all_nodes - child_nodes
		std::set_difference(setexists.begin(), setexists.end(), ischild.begin(), ischild.end(), std::inserter(setroots, setroots.begin()));

		if (setroots.size() > 1) { // more than one root
			// merging; new scale of apparition = 2.0 * max(scale_of_apparition)
			Set* s = new Set(m_scale_sets->firstFreeIndex(), 2.0 * max_soa + 10.0);
			s->setAttribute(m_user->SCCB_Q_newSetAttribute());
			for (std::set<sxs::ScaleSets::Set *>::iterator i = setroots.begin(); i != setroots.end(); ++i)
				s->sons().push_back(*i);
			m_scale_sets->insert(s);
		}

		//==========================================
		// Progress
		m_number_of_stages = 0;
		m_stage = 0;
		//==========================================

		//==========================================
		// Non persistent sets removal
		m_scale_sets->removeNonPersistentSets(m_param->getMinimumPersistence());
		//==========================================

		//==========================================
		/// frees the Slot m_neigb
		if (m_param->getSimpleGraph()) {
			m_graph->freeNodeSlot(m_neigb);
		}
		//==========================================

		//==========================================
		// CB post
		m_user->SCCB_N_postPostprocessingNotification();
		//==========================================

		return true;
	}
	//=================================================================================

	//=================================================================================
	lgl::BOOL ScaleClimbing::step(lgl::BOOL& normal_termination) {
		normal_termination = true;

		//std::cout << "step()\n" << std::flush;
		//std::cout << "heap has " << m_heap.size() << " edges\n" << std::flush;

		//==========================================
		// Stop if no possible fusion left
		if (m_heap.size() == 0)
			return false;
		//==========================================

		// if (m_heap.size()>0 && m_heap.size()<20) { std::cout << "\nstep() 1: " << std::flush;  std::cout << "heap top is " << m_heap.top() << "\n" << std::flush; std::cout << "Partial heap dump: " << m_heap.size() << "   ";  for (int i=0; i<std::min(m_heap.size(), 10); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "...  "; for (int i=std::max(10, m_heap.size()-10); i<m_heap.size(); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "done\n" << std::flush; }

		//==========================================
		// CB pre
		m_user->SCCB_N_preStepNotification();
		//==========================================

		//==========================================
		/// removes the least scale of appearance edge (representing the union of its end nodes)
		Edge e = m_heap.remove_top();

		// if (m_heap.size()>0 && m_heap.size()<20) { std::cout << "\nstep() 2: top has been removed" << std::flush;  std::cout << "heap top is " << m_heap.top() << "\n" << std::flush; std::cout << "Partial heap dump: " << m_heap.size() << "   ";  for (int i=0; i<std::min(m_heap.size(), 10); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "...  "; for (int i=std::max(10, m_heap.size()-10); i<m_heap.size(); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "done\n" << "top was " << e << "\n" << std::flush; }
		//==========================================

		//==========================================
		/// builds the new Set
		Set* s = new Set(m_scale_sets->firstFreeIndex(), e.getAttribute()->scaleOfAppearance());
		// attribute allocation
		s->setAttribute(m_user->SCCB_Q_newSetAttribute());
		// sons
		s->sons().push_back((Set*) e.getInitialNode().getAttribute()->sxspSet());
		s->sons().push_back((Set*) e.getFinalNode().getAttribute()->sxspSet());
		// insertion
		m_scale_sets->insert(s);
		//==========================================
		// CB user's attribution
		m_user->SCCB_C_onSetCreation(e, s);
		m_user->SCCB_D_onSetCreation(e, s);
		//==========================================

		//==========================================
		// memo initial node (survivor)
		Node survivor = e.getInitialNode();
		// Transfers e's partial energy to the survivor
		delete survivor.getAttribute()->ppartialEnergy();
		survivor.getAttribute()->ppartialEnergy() = e.getAttribute()->ppartialEnergy();
		e.getAttribute()->ppartialEnergy() = NULL;
		//==========================================

		//==========================================
		/// contracts the graph
		if (!contract(e)) {
			normal_termination = false;
			return false;
		}
		//==========================================

		//==========================================
		// The new set is pointed by the surviving node as its representative
		survivor.getAttribute()->sxspSet() = (void*) s;
		//==========================================

		//==========================================
		// CB post
		m_user->SCCB_N_postStepNotification();
		//==========================================

		//==========================================
		// CB : continue climbing ?
		return m_user->SCCB_Q_mustContinueClimbing();
		//==========================================

	}
	//=================================================================================

	//=================================================================================
	lgl::BOOL ScaleClimbing::contract(Edge e) {
		// if (m_heap.size()>0 && m_heap.size()<20) { std::cout << "\ncontract() 1: start   " << std::flush;  std::cout << "heap top is " << m_heap.top() << "\n" << std::flush; std::cout << "Partial heap dump: "<< m_heap.size() << "   ";  for (int i=0; i<std::min(m_heap.size(), 10); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "...  "; for (int i=std::max(10, m_heap.size()-10); i<m_heap.size(); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "done\n" << std::flush; }

		//==============================================================
		// CB
		m_user->SCCB_C_onEdgeContraction(e);
		m_user->SCCB_D_onEdgeContraction(e);
		//==============================================================

		//==============================================================
		// Surviving node
		Node survivor = e.getInitialNode();
		// dead one
		Node dead = e.getFinalNode();
		//====================================================================================
		// survivor/dead node edges iterators
		Node::iterator sed, ded;
		//====================================================================================

		//==============================================================
		// If the graph must be kept simple : marks the survivor's neigbours
		if (m_param->getSimpleGraph()) {
			for (sed = survivor.begin(); sed != survivor.end(); ++sed) {
				(*sed).getFinalNode().setMark(m_neigb, 1);
			}
		}
		//====================================================================================

		// if (m_heap.size()>0 && m_heap.size()<20) { std::cout << "\ncontract() 22: e=" << e << " idx=" << m_index(e) << " survivor=" << survivor << " dead=" << dead << "\n" << std::flush; }

		//==============================================================
		// re-links the dead's edges to the survivor
		// if is a dead-survivor edge : delete
		// if the graph must be kept simple : merges the multiple edges.
		for (ded = dead.begin(); ded != dead.end();) {
			Edge e = (*ded);

			// if (m_heap.size()>0 && m_heap.size()<20) { std::cout << "\ncontract() 8: " << std::flush;  std::cout << "heap top is " << m_heap.top() << "\n" << std::flush; std::cout << "Partial heap dump: "<< m_heap.size() << "   ";  for (int i=0; i<std::min(m_heap.size(), 10); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "...  "; for (int i=std::max(10, m_heap.size()-10); i<m_heap.size(); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "done\n" << "e=" << e << "   idx e=" << m_index(e) << "\n" << std::flush; }

			++ded;
			// edge between dead and survivor must be deleted
			if (e.getFinalNode() == survivor) {
				//	lglLOG ( "-- DEAD-SURVIVOR EDGE --"<<std::endl );
				// CB before deletion
				//	lglLOG ( "SCCB_C_onEdgeDeletion "<<e<<std::endl );
				m_user->SCCB_C_onEdgeDeletion(e);
				m_user->SCCB_D_onEdgeDeletion(e);
				// if !SimpleGraph() the edge can still be in the heap => remove
				if (m_index(e) >= 0) {

					// if (m_heap.size()>0 && m_heap.size()<20) { std::cout << "\ncontract() 20: about to remove from heap " << e << " idx=" << m_index(e) << "\n" << std::flush;  std::cout << "heap top is " << m_heap.top() << "\n" << std::flush; std::cout << "Partial heap dump: "<< m_heap.size() << "   ";  for (int i=0; i<std::min(m_heap.size(), 10); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "...  "; for (int i=std::max(10, m_heap.size()-10); i<m_heap.size(); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "done\n" << std::flush; }

					{
						int index = 0;
						for (; index < m_heap.size(); ++index)
							if (m_heap[index].getIndex() == e.getIndex()) {
								m_heap.remove(index);
								break;
							}
						// std::cout << "found index=" << index << "\n";
					}
					//m_heap.remove( m_index(e) );

					// if (m_heap.size()>0 && m_heap.size()<20) { std::cout << "\ncontract() 24: " << std::flush;  std::cout << "heap top is " << m_heap.top() << "\n" << std::flush; std::cout << "Partial heap dump: "<< m_heap.size() << "   ";  for (int i=0; i<std::min(m_heap.size(), 10); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "...  "; for (int i=std::max(10, m_heap.size()-10); i<m_heap.size(); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "done\n" << std::flush; }
				}

				// edge deletion
				// if (m_heap.size()>0 && m_heap.size()<20) { std::cout << "e=" << e << "   idx e=" << m_index(e) << "\n" << std::flush; }

				removeEdge(e);

				// if (m_heap.size()>0 && m_heap.size()<20) { std::cout << "contract() 10   after removeEdge, e=" << e << " idx=" << m_index(e) << ": " << std::flush;  std::cout << "heap top is " << m_heap.top() << "\n" << std::flush; std::cout << "Partial heap dump: "<< m_heap.size() << "   ";  for (int i=0; i<std::min(m_heap.size(), 10); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "...  "; for (int i=std::max(10, m_heap.size()-10); i<m_heap.size(); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "done\n" << std::flush; }
			}
			// if SimpleGraph() and multiple edge => merge
			else if ((m_param->getSimpleGraph()) && (e.getFinalNode().getMark(m_neigb) == 1)) {
				// Search for the edge starting from survivor and ending at the same neigbour
				for (sed = survivor.begin(); (*sed).getFinalNode() != e.getFinalNode(); ++sed)
					;
				Edge me = (*sed);

				//if (m_heap.size()>0 && m_heap.size()<20) { std::cout << "me=" << me << "  idx me=" << m_index(me) << "\n" << std::flush; }
				//if (m_heap.size()>0 && m_heap.size()<20) { std::cout << "e=" << e << "   idx e=" << m_index(e) << "\n" << std::flush; }

				// CB merge e to me ?
				//	lglLOG ( "-- MULTIPLE EDGE --"<<std::endl );
				//	lglLOG ( "SCCB_C_onEdgeMerging "<<e<<" -> "<<me<<std::endl );
				if (m_user->SCCB_C_onEdgeMerging(e, me) && m_user->SCCB_D_onEdgeMerging(e, me)) {
					// delete e .m_bnd
					// CB before deletion
					//	  lglLOG ( "SCCB_C_onEdgeDeletion "<<e<<std::endl );
					m_user->SCCB_C_onEdgeDeletion(e);
					m_user->SCCB_D_onEdgeDeletion(e);
					//	  lglLOG ( "remove heap... " );
					// if is in the heap => remove
					if (m_index(e) >= 0) {

						//if (m_heap.size()>0 && m_heap.size()<20) { std::cout << "contract() 11 about to remove from heap e=" << e << " idx=" << m_index(e) << ": " << std::flush;  std::cout << "heap top is " << m_heap.top() << "\n" << std::flush; std::cout << "Partial heap dump: "<< m_heap.size() << "   ";  for (int i=0; i<std::min(m_heap.size(), 10); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "...  "; for (int i=std::max(10, m_heap.size()-10); i<m_heap.size(); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "done\n" << std::flush; }

						{
							int index = 0;
							for (; index < m_heap.size(); ++index)
								if (m_heap[index].getIndex() == e.getIndex()) {
									m_heap.remove(index);
									break;
								}
							//std::cout << "found index=" << index << "\n";
						}
						//m_heap.remove( m_index(e) );

						//if (m_heap.size()>0 && m_heap.size()<20) { std::cout << "contract() 11b after removing from heap e=" << e << " idx=" << m_index(e) << ": " << std::flush;  std::cout << "heap top is " << m_heap.top() << "\n" << std::flush; std::cout << "Partial heap dump: "<< m_heap.size() << "   ";  for (int i=0; i<std::min(m_heap.size(), 10); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "...  "; for (int i=std::max(10, m_heap.size()-10); i<m_heap.size(); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "done\n" << std::flush; }

					}

					//	  lglLOG ( "graph del... " );
					// graph deletion
					//if (m_heap.size()>0 && m_heap.size()<20) { std::cout << "e=" << e << "   idx e=" << m_index(e) << "\n" << std::flush; }

					removeEdge(e);

					//if (m_heap.size()>0 && m_heap.size()<20) { std::cout << "contract() 12   after removeEdge, e=" << e << " idx=" << m_index(e) << ": " << std::flush;  std::cout << "heap top is " << m_heap.top() << "\n" << std::flush; std::cout << "Partial heap dump: "<< m_heap.size() << "   ";  for (int i=0; i<std::min(m_heap.size(), 10); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "...  "; for (int i=std::max(10, m_heap.size()-10); i<m_heap.size(); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "done\n" << std::flush; }

					//	  lglLOG ( "ok"<<std::endl );
				}
				// else the edge must be reconnected to the survivor
				else {
					// CB
					//	  lglLOG ( "SCCB_C_onNodeFromChange... " );
					//if (m_heap.size()>0 && m_heap.size()<20) { std::cout << "contract() 13: " << std::flush;  std::cout << "heap top is " << m_heap.top() << "\n" << std::flush; std::cout << "Partial heap dump: ";  for (int i=0; i<std::min(m_heap.size(), 10); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "...  "; for (int i=std::max(10, m_heap.size()-10); i<m_heap.size(); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "done\n" << std::flush; }
					//if (m_heap.size()>0 && m_heap.size()<20) { std::cout << "e=" << e << "   idx e=" << m_index(e) << "\n" << std::flush; }

					m_user->SCCB_C_onNodeFromChange(e, survivor);
					m_user->SCCB_D_onNodeFromChange(e, survivor);
					// graph operation
					m_graph->changeNodeFrom(e, survivor);
					//	  lglLOG ( "ok"<<std::endl );
					//if (m_heap.size()>0 && m_heap.size()<20) { std::cout << "contract() 14: " << std::flush;  std::cout << "heap top is " << m_heap.top() << "\n" << std::flush; std::cout << "Partial heap dump: ";  for (int i=0; i<std::min(m_heap.size(), 10); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "...  "; for (int i=std::max(10, m_heap.size()-10); i<m_heap.size(); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "done\n" << std::flush; }

				}
			}
			// else the edge must be reconnected to the survivor
			else {
				// CB
				//if (m_heap.size()>0 && m_heap.size()<20) { std::cout << "contract() 15: " << std::flush;  std::cout << "heap top is " << m_heap.top() << "\n" << std::flush; std::cout << "Partial heap dump: ";  for (int i=0; i<std::min(m_heap.size(), 10); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "...  "; for (int i=std::max(10, m_heap.size()-10); i<m_heap.size(); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "done\n" << std::flush; }

				m_user->SCCB_C_onNodeFromChange(e, survivor);
				m_user->SCCB_D_onNodeFromChange(e, survivor);
				// graph operation
				m_graph->changeNodeFrom(e, survivor);

				// if (m_heap.size()>0 && m_heap.size()<20) { std::cout << "contract() 16: " << std::flush;  std::cout << "heap top is " << m_heap.top() << "\n" << std::flush; std::cout << "Partial heap dump: ";  for (int i=0; i<std::min(m_heap.size(), 10); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "...  "; for (int i=std::max(10, m_heap.size()-10); i<m_heap.size(); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "done\n" << std::flush; }

			}
		}
		//====================================================================================

		// if (m_heap.size()>0 && m_heap.size()<20) { std::cout << "contract() 3: " << std::flush;  std::cout << "heap top is " << m_heap.top() << "\n" << std::flush; std::cout << "Partial heap dump: ";  for (int i=0; i<std::min(m_heap.size(), 10); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "...  "; for (int i=std::max(10, m_heap.size()-10); i<m_heap.size(); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "done\n" << std::flush; }

		//====================================================================================
		// deletion of the dead node
		// CB
		// if (m_heap.size()>0 && m_heap.size()<20) { std::cout << "dead=" << dead << "\n" << std::flush; }

		m_user->SCCB_C_onNodeDeletion(dead);
		m_user->SCCB_D_onNodeDeletion(dead);
		// graph operation (+attribute deletion)
		removeNode(dead);
		//====================================================================================

		// if (m_heap.size()>0 && m_heap.size()<20) { std::cout << "contract() 4 after removeNode " << dead << ": " << std::flush;  std::cout << "heap top is " << m_heap.top() << "\n" << std::flush; std::cout << "Partial heap dump: ";  for (int i=0; i<std::min(m_heap.size(), 10); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "...  "; for (int i=std::max(10, m_heap.size()-10); i<m_heap.size(); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "done\n" << std::flush; }

		//====================================================================================
		// Updates priorities of the survivor's edges and re-sorts the heap
		for (sed = survivor.begin(); sed != survivor.end(); ++sed) {

			// if (m_heap.size()>0 && m_heap.size()<20) { std::cout << "contract() 7: sed=" << *sed << " idx=" << m_index(*sed) << "\n" << std::flush;  std::cout << "heap top is " << m_heap.top() << "\n" << std::flush; std::cout << "Partial heap dump: ";  for (int i=0; i<std::min(m_heap.size(), 10); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "...  "; for (int i=std::max(10, m_heap.size()-10); i<m_heap.size(); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "done\n" << std::flush; }

			// by the way : resets neigb
			if (m_param->getSimpleGraph())
				(*sed).getFinalNode().setMark(m_neigb, 0);
			// if the edge is to be considered
			if (m_user->SCCB_Q_mustConsider(*sed)) {
				// memo old soa
				lgl::F32 osoa = (*sed).getAttribute()->scaleOfAppearance();
				// Updates priority
				functionalDynamicProgrammingStep(*sed);
				// if heap insertion allowed
				if (heapInsertionAllowed(*sed)) {
					// if is already in the heap : re-sort
					if (m_index(*sed) >= 0) {
						if ((*sed).getAttribute()->scaleOfAppearance() < osoa)
							m_heap.upsort(m_index(*sed));
						else
							m_heap.downsort(m_index(*sed));

						// if (m_heap.size()>0 && m_heap.size()<20) { std::cout << "contract() 7b after resorting: " << std::flush;  std::cout << "heap top is " << m_heap.top() << "\n" << std::flush; std::cout << "Partial heap dump: ";  for (int i=0; i<std::min(m_heap.size(), 10); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "...  "; for (int i=std::max(10, m_heap.size()-10); i<m_heap.size(); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "done\n" << std::flush; }

					}
					// else : insert
					else {
						m_heap.insert(*sed);

						// 	    std::cout << "inserting edge (1); " << *sed << "   "
						// 		      << "heap has " << m_heap.size() << " edges\n" << std::flush;
						// 	    if (m_heap.size()>0) {
						// 	      std::cout << "heap top is " << m_heap.top() << "\n" << std::flush;
						// 	      std::cout << "Partial heap dump: ";
						// 	      for (int i=0; i<std::min(m_heap.size(), 10); ++i)
						// 		std::cout << "[" << i << "]" << m_heap[i] << "  ";
						// 	      std::cout << "...  ";
						// 	      for (int i=std::max(10, m_heap.size()-10); i<m_heap.size(); ++i)
						// 		std::cout << "[" << i << "]" << m_heap[i] << "  ";
						// 	      std::cout << "done\n" << std::flush;
						// 	    }
					}
				}
				// else if was in the heap : remove
				else if (m_index(*sed) >= 0) {
					//if (m_heap.size()>0 && m_heap.size()<20) { std::cout << "contract() 7c about to remove " << *sed << " idx=" << m_index(*sed) << ": " << std::flush;  std::cout << "heap top is " << m_heap.top() << "\n" << std::flush; std::cout << "Partial heap dump: ";  for (int i=0; i<std::min(m_heap.size(), 10); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "...  "; for (int i=std::max(10, m_heap.size()-10); i<m_heap.size(); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "done\n" << std::flush; }

					{
						int index = 0;
						for (; index < m_heap.size(); ++index)
							if (m_heap[index].getIndex() == (*sed).getIndex()) {
								m_heap.remove(index);
								break;
							}
						//std::cout << "found index=" << index << "\n";
					}
					//m_heap.remove( m_index(*sed) );

					// 	  std::cout << "removed non-considered edge (1); " << *sed << "   "
					// 		    << "heap has " << m_heap.size() << " edges\n" << std::flush;
					// 	  if (m_heap.size()>0) {
					// 	    std::cout << "heap top is " << m_heap.top() << "\n" << std::flush;
					// 	    std::cout << "Partial heap dump: ";
					// 	    for (int i=0; i<std::min(m_heap.size(), 10); ++i)
					// 	      std::cout << "[" << i << "]" << m_heap[i] << "  ";
					// 	    std::cout << "...  ";
					// 	    for (int i=std::max(10, m_heap.size()-10); i<m_heap.size(); ++i)
					// 	      std::cout << "[" << i << "]" << m_heap[i] << "  ";
					// 	    std::cout << "done\n" << std::flush;
					// 	  }
				}
			}
			// else if was in the heap : remove [ajoute ROGER TRIAS 2005-08-23]
			else if (m_index(*sed) >= 0) {
				//if (m_heap.size()>0 && m_heap.size()<20) { std::cout << "contract() 5: " << std::flush;  std::cout << "heap top is " << m_heap.top() << "\n" << std::flush; std::cout << "Partial heap dump: ";  for (int i=0; i<std::min(m_heap.size(), 10); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "...  "; for (int i=std::max(10, m_heap.size()-10); i<m_heap.size(); ++i) std::cout << "[" << i << "]" << m_heap[i] << "  "; std::cout << "done\n" << std::flush; }

				{
					int index = 0;
					for (; index < m_heap.size(); ++index)
						if (m_heap[index].getIndex() == (*sed).getIndex()) {
							m_heap.remove(index);
							break;
						}
					//std::cout << "found index=" << index << "\n";
				}
				//m_heap.remove( m_index(*sed) );

				// 	std::cout << "removing non-considered edge (2); " << *sed << "    "
				// 		  << "heap has " << m_heap.size() << " edges\n" << std::flush;
				// 	if (m_heap.size()>0) {
				// 	  std::cout << "heap top is " << m_heap.top() << "\n" << std::flush;
				// 	  std::cout << "Partial heap dump: ";
				// 	  for (int i=0; i<std::min(m_heap.size(), 10); ++i)
				// 	    std::cout << "[" << i << "]" << m_heap[i] << "  ";
				// 	  std::cout << "...  ";
				// 	  for (int i=std::max(10, m_heap.size()-10); i<m_heap.size(); ++i)
				// 	    std::cout << "[" << i << "]" << m_heap[i] << "  ";
				// 	  std::cout << "done\n" << std::flush;
				// 	}
			}
		}
		//====================================================================================
		// HAPPY END
		//     if (m_heap.size()>0) {
		//       std::cout << "at end of contract(): heap top is " << m_heap.top() << "\n" << std::flush;
		//       std::cout << "heap top is " << m_heap.top() << "\n" << std::flush;
		//       std::cout << "Partial heap dump: ";
		//       for (int i=0; i<std::min(m_heap.size(), 10); ++i)
		// 	std::cout << "[" << i << "]" << m_heap[i] << "  ";
		//       std::cout << "...  ";
		//       for (int i=std::max(10, m_heap.size()-10); i<m_heap.size(); ++i)
		// 	std::cout << "[" << i << "]" << m_heap[i] << "  ";
		//       std::cout << "done\n" << std::flush;
		//     }

		return true;
		//====================================================================================
	}
	//====================================================================================

	//=================================================================================
	lgl::BOOL ScaleClimbing::functionalDynamicProgrammingStep(Edge e) {
		lglASSERT ( e != Edge::Null() );
		//================================================================
		// CB pre
		m_user->SCCB_N_preFDP(e);
		//================================================================

		//================================================================
		// Energies of the union of e's endnodes
		lgl::F32 C = m_user->SCCB_C_eval(e);
		lgl::F32 D = m_user->SCCB_D_eval(e);
		//================================================================

		//================================================================
		//
		if (e.getAttribute()->ppartialEnergy() != NULL) {
			delete e.getAttribute()->ppartialEnergy();
			e.getAttribute()->ppartialEnergy() = NULL;
		}

		// partial energy initialized to the from() node's one
		lglASSERT ( e.getInitialNode().getAttribute()->ppartialEnergy() != NULL );
		e.getAttribute()->ppartialEnergy() = new PiecewiseAffineFunction(e.getInitialNode().getAttribute()->partialEnergy());

		// sum with the to() node's partial energy
		e.getAttribute()->partialEnergy() += e.getFinalNode().getAttribute()->partialEnergy();

		// infimum with the self energy of the union
		if (!e.getAttribute()->partialEnergy().infimum(AffinePiece(0, D, C), e.getAttribute()->scaleOfAppearance())) {
#ifndef LGL_WITH_BOOST
			lgl::String s = lgl::String(wxT("over-additive regularizer"));
#else
			lgl::String s("over-additive regularizer");
#endif
			if (m_user->SCCB_N_warning(1, s) == false) {
				return false;
			}
		}
		//================================================================

		//std::cout << "*** Edge partial E = "<<e.getAttribute()->partialEnergy()<<std::endl;
		//================================================================
		// Negatives soa get mapped to zero
		if (e.getAttribute()->scaleOfAppearance() < 0)
			e.getAttribute()->scaleOfAppearance() = 0;
		//================================================================

		//================================================================
		// CB post
		m_user->SCCB_N_postFDP(e);
		//================================================================

		//================================================================
		return true;
		//================================================================

	}
	//=================================================================================

	//=================================================================================
	lgl::BOOL ScaleClimbing::heapInsertionAllowed(Edge e) {
		return ((m_user->SCCB_Q_mustConsider(e)) && (e.getAttribute()->scaleOfAppearance() <= m_param->getMaximumScale()));
	}
	//=================================================================================

	//=================================================================================
	//=================================================================================
	//=================================================================================
	/// Parameters of the ScaleClimbing algorithm
	//=================================================================================
	//=================================================================================
	//=================================================================================

	//=================================================================================
	ScaleClimbingParameters::ScaleClimbingParameters() :
			m_simple_graph(true), m_maximum_scale((std::numeric_limits<lgl::F32>::max)()), m_minimum_persistence(1) {
	}
//=================================================================================

}
//=================================================================================
// EO namespace sxs
//=================================================================================