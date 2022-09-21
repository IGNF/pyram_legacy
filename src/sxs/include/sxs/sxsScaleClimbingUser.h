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
#ifndef __sxsScaleClimbingUser_h__
#define __sxsScaleClimbingUser_h__
//===========================================================
// namespace	: sxs
// classe		: ScaleClimbingUser
//===========================================================
// author		: Laurent Guigues
// history		: 
//	14/07/04		creation
//===========================================================
#include "sxs/sxsGeneral.h"
#include "sxs/sxsScaleClimbingGraph.h"
#include "sxs/sxsScaleSets.h"


namespace sxs {

	class ScaleClimbing;

	//===========================================================
	/// \brief Abstract definition of an User of the ScaleClimbing algorithm.
	///
	/// Defines the energetical and the structure management callbacks (virtual methods)
	/// which define a particular instance of the algorithm
	/// as well as control and notification callbacks.
	/// All methods start with the tag SCCB (ScaleClimbingCallBack).
	class ScaleClimbingUser {
	public:
		/// \shortcut to the Nodes of a ScaleCimbingGraph
		typedef ScaleClimbingGraph::Node Node;
		/// \shortcut to the Edges of a ScaleCimbingGraph
		typedef ScaleClimbingGraph::Edge Edge;
		/// \shortcut to the Sets of a ScaleSets
		typedef ScaleSets::Set Set;

		//===========================================================
		///@name Basic methods

		///@{

		//===========================================================
		/// Ctor
		ScaleClimbingUser() {
		}
		/// Dtor
		virtual ~ScaleClimbingUser() {
		}
		//===========================================================

		/// Sets the ScaleClimbing object which is associated with this user (called by the ScaleClimbing object itself when the User is passed to him).
		virtual void setScaleClimbing(ScaleClimbing* s) {
			m_sc = s;
		}
		/// Returns a const ref on the ScaleClimbing object which is associated with this user (usefull to access the structures during the algorithm : Graph, ScaleSets ...)
		const ScaleClimbing& getScaleClimbing() const {
			return *m_sc;
		}
		/// Returns a ref on the ScaleClimbing object which is associated with this user (usefull to access the structures during the algorithm : Graph, ScaleSets ...)
		ScaleClimbing& getScaleClimbing() {
			return *m_sc;
		}
		///@}
		//===========================================================

		//===========================================================
		///@name Energy evaluation callbacks.
		/// The tag :
		/// - C : Refers to the complexity energy
		/// - D : Refers to the fidelity energy
		///
		/// THESE METHODS MUST BE OVERLOADED !

		///@{

		/// Returns the complexity energy value of a Node
		virtual lgl::F32 SCCB_C_eval(Node n) = 0;
		/// Returns the complexity energy value of an Edge, that is of the union of its two end Nodes
		virtual lgl::F32 SCCB_C_eval(Edge n) = 0;
		/// Returns the fidelity energy value of a Node
		virtual lgl::F32 SCCB_D_eval(Node n) = 0;
		/// Returns the fidelity energy value of an Edge, that is of the union of its two end Nodes
		virtual lgl::F32 SCCB_D_eval(Edge n) = 0;

		///@}
		//===========================================================

		//===========================================================
		///@name General query callbacks
		//===========================================================

		///@{

		/// Must the Node be considered in the algorithm (can be dynamic) ?
		virtual lgl::BOOL SCCB_Q_mustConsider(Node e) {
			return true;
		}
		/// Must the Edge be considered in the algorithm (can be dynamic) ?
		virtual lgl::BOOL SCCB_Q_mustConsider(Edge e) {
			return true;
		}
		/// Must climbing continue (called after each ScaleClimbing::step() ) ?
		virtual lgl::BOOL SCCB_Q_mustContinueClimbing() {
			return true;
		}
		///@}
		//===========================================================

		//===========================================================
		///@name Scale-sets creation related callbacks
		//===========================================================

		///@{
		/// \brief Queries the allocation of a new Set::Attribute.
		///
		/// The user must allocate a new Set::Attribute which will be given to a newly created Set (method Set::setAttribute()).
		/// If the effective class of the attribute is a descendant of Set::Attribute then the user
		/// is responsible for the allocation (operator new) of the good object type (The ScaleSets is then responsible for its deletion).
		/// If this is not done, future dynamic down casts will fail...
		virtual Set::Attribute* SCCB_Q_newSetAttribute() {
			return new Set::Attribute;
		}

		///@}
		//===========================================================

		//===========================================================
		///@name Graph management / energy update related callback

		///@{

		//===========================================================
		///@name Complexity Energy (C) updates dedicated callbacks
		//===========================================================

		///@{

		/// Called before the contraction of Edge e (beginning of contract(e)).
		///
		/// Its Node from() will survive and to() will die.
		/// The user can hence update the attributes of from() in order to
		/// reflect its merging with to().
		virtual void SCCB_C_onEdgeContraction(Edge e) {
		}
		/// Called before physical deletion of Edge e
		virtual void SCCB_C_onEdgeDeletion(Edge e) {
		}
		/// Called before Edge "merged" is merged to Edge "to". Returns true iff merging must be done ("merged" will be deleted just after)
		virtual bool SCCB_C_onEdgeMerging(Edge merged, Edge to) {
			return true;
		}
		/// Called before the initial Node of Edge e is changed to new_origin
		virtual void SCCB_C_onNodeFromChange(Edge e, Node new_origin) {
		}
		/// Called before Node n is physically removed from the graph
		virtual void SCCB_C_onNodeDeletion(Node n) {
		}
		/// \brief Called after the creation of a new Base Set corresponding to the Node n (called in ScaleClimbing::buildBase()).
		///
		/// The attribute of the Set has already been created (by a callback to Q_newSetAttribute).
		/// The user can fill the Set's attribute with custom values.
		virtual void SCCB_C_onBaseSetCreation(Node n, Set* s) {
		}
		/// \brief Called after the creation of a new Set corresponding to the Edge e contraction (called in ScaleClimbing::step() before ScaleClimbing::contract(e)).
		///
		/// The attribute of the Set has already been created (by a callback to Q_newSetAttribute).
		/// The user can thus fill the Set's attribute with custom values.
		virtual void SCCB_C_onSetCreation(Edge e, Set* s) {
		}

		///@}

		//===========================================================
		///@name Fidelity Energy (D) updates dedicated callbacks
		//===========================================================

		///@{

		/// \brief Called before the contraction of Edge e (beginning of ScaleClimbing::contract(e)).
		///
		/// Its Node from() will survive and to() will die.
		/// The user can hence update the attributes of from() in order to
		/// reflect its merging with to().
		virtual void SCCB_D_onEdgeContraction(Edge e) {
		}
		/// Called before physical deletion of Edge e
		virtual void SCCB_D_onEdgeDeletion(Edge e) {
		}
		/// Called before Edge "merged" is merged to Edge "to". Returns true iff merging must be done ("merged" will be deleted just after)
		virtual bool SCCB_D_onEdgeMerging(Edge merged, Edge to) {
			return true;
		}
		/// Called before the Edge's e from Node is changed to new_origin
		virtual void SCCB_D_onNodeFromChange(Edge e, Node new_origin) {
		}
		/// Called before Node n is physically removed from the graph
		virtual void SCCB_D_onNodeDeletion(Node n) {
		}
		/// \brief Called after the creation of a new Base Set corresponding to the Node n (called in ScaleClimbing::buildBase()).
		///
		/// The attribute of the Set has already been created (by a callback to Q_newSetAttribute).
		/// The user can fill the Set's attribute with custom values.
		virtual void SCCB_D_onBaseSetCreation(Node n, Set* s) {
		}
		/// \brief Called after the creation of a new Set corresponding to the Edge e contraction (called in ScaleClimbing::step() before ScaleClimbing::contract(e)).
		///
		/// The attribute of the Set has already been created (by a callback to Q_newSetAttribute).
		/// The user can thus fill the Set's attribute with custom values.
		virtual void SCCB_D_onSetCreation(Edge e, Set* s) {
		}

		///@}
		//===========================================================
		///@}
		//===========================================================
		//===========================================================

		//===========================================================
		///@name Notification callbacks (N)
		/// Ponctual callbacks notifying the current algorithm state, progress...

		///@{

		/// Called before the algorithm runs (beginning of run())
		virtual void SCCB_N_preRun() {
		}
		/// Called when the algorithm has finished (end of run())
		virtual void SCCB_N_postRun() {
		}
		/// Called before the ScaleSets's Base contruction (beginning of buildBase())
		virtual void SCCB_N_preBuildBaseNotification() {
		}
		/// Called after the ScaleSets's Base contruction (end of buildBase())
		virtual void SCCB_N_postBuildBaseNotification() {
		}
		/// Called before the Heap construction (beginning of buildHeap())
		virtual void SCCB_N_preBuildHeapNotification() {
		}
		/// Called after the Heap construction (end of buildHeap())
		virtual void SCCB_N_postBuildHeapNotification() {
		}
		/// Called before climbing starts (beginning of climb())
		virtual void SCCB_N_preClimbingNotification() {
		}
		/// Called after climbing (end of climb())
		virtual void SCCB_N_postClimbingNotification() {
		}
		/// Called before post processing starts (beginning of postProcess())
		virtual void SCCB_N_prePostprocessingNotification() {
		}
		/// Called after post processing starts (end of postProcess())
		virtual void SCCB_N_postPostprocessingNotification() {
		}
		/// Called before each ScaleClimbing step (beginning of step())
		virtual void SCCB_N_preStepNotification() {
		}
		/// Called after each ScaleClimbing step (end of step())
		virtual void SCCB_N_postStepNotification() {
		}
		/// Called at the beginning of each functionalDynamicProgrammingStep(e)
		virtual void SCCB_N_preFDP(Edge e) {
		}
		/// Called at the end of each functionalDynamicProgrammingStep(e)
		virtual void SCCB_N_postFDP(Edge e) {
		}
		/// \brief Gives a progress indicator for the current task (tasks correspond to level 1 methods of ScaleClimbing).
		virtual bool SCCB_N_progressIndicator(int stage, int number_of_stages) {
			return true;
		}
		///@}
		//===========================================================

		//===========================================================
		///@name Warning/Error callbacks

		///@{

		/// Warning : algorithm can continue but might give strange results.
		///
		/// If returns true then continues else stops.
		virtual lgl::BOOL SCCB_N_warning(int number, lgl::String& description) {
			return false;
		}
		/// Fatal error : algorithm cannot continue (in fact stops just after).
		virtual void SCCB_N_error(const lgl::String& comment) {
		}
		///@}
		//===========================================================

	protected:
		/// The algorithm associated with this user
		ScaleClimbing* m_sc;
	};
//===========================================================
// EO class ScaleClimbingUser
//===========================================================

}
//===========================================================
// EOF
//===========================================================
#endif
