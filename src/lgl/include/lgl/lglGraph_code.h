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
	\brief Code of the Graph class and related classes
*/
//====================================================
// Graph
//====================================================


//====================================================
template <class NA, class EA>
Graph<NA,EA>::Graph() 
  :
  m_node(NULL,TRUE),
     m_edge(NULL,TRUE),
     m_nbn(0),
     m_nbe(0)
{
}
//====================================================


//====================================================
template <class NA, class EA>
Graph<NA,EA>::~Graph() 
{
  clear();
}
//====================================================



//====================================================
template <class NA, class EA>
void Graph<NA,EA>::clear() 
{
  typename lgl::SparseArray<pPhNode>::iterator itn;
  for (itn=m_node.begin(); itn!=m_node.end(); ++itn) {
    delete (*itn);
    *itn = NULL;
  }
  
  typename lgl::SparseArray<pPhEdge>::iterator ite;
  for (ite=m_edge.begin(); ite!=m_edge.end(); ++ite) { 
    delete (*ite);
    *ite = NULL;
  }
  
  m_node.freeUnusedMemory(FALSE);
  m_edge.freeUnusedMemory(FALSE);
  
  m_nbn = m_nbe = 0;
}
//====================================================




//====================================================
template <class NA, class EA>
typename Graph<NA,EA>::Node Graph<NA,EA>::addNode(Index i) 
{
	typename lgl::SparseArray<pPhNode>::iterator it = m_node.begin(i);
	// if Index already occupied => returns Null Node
	if ((*it)!=NULL) return Node();
	(*it) = new PhNode(i);
	m_nbn++;
	return Node((*it));
}
//====================================================


//====================================================
template <class NA, class EA>
typename Graph<NA,EA>::Node Graph<NA,EA>::addNode(Index i, const NA & a) 
{
	typename lgl::SparseArray<pPhNode>::iterator it = m_node.begin(i);
//	pPhNode& n = m_node[i];
	// if Index already occupied => returns Null Node
	if ((*it)!=NULL) return Node();
	(*it) = new PhNode(i,a);
	m_nbn++;
	return Node((*it));
}
//====================================================




//====================================================
template <class NA, class EA>
typename Graph<NA,EA>::Edge Graph<NA,EA>::addEdge(Node n1, Node n2, Index i) 
{
	// if Null end nodes => returns Null Edge
	if ( (n1==Node::Null()) || (n2==Node::Null()) ) return Edge();
	// starts an edge iteration on index i 
	typename lgl::SparseArray<pPhEdge>::iterator it = m_edge.begin(i);
	// if Index already occupied => returns Null Edge 
	if ((*it)!=NULL) return Edge();
	(*it) = new PhEdge ( n1.m_phnode, n2.m_phnode, i );
	m_nbe++;
	return Edge((*it));
}
//====================================================


//====================================================
template <class NA, class EA>
typename Graph<NA,EA>::Edge Graph<NA,EA>::addEdge(Node n1, Node n2, Index i, const EA & a) 
{
	// if Null end nodes => returns Null Edge
	if ( (n1==Node::Null()) || (n2==Node::Null()) ) return Edge();
	// starts an edge iteration on index i 
	typename lgl::SparseArray<pPhEdge>::iterator it = m_edge.begin(i);
	// if Index already occupied => returns Null Edge 
	if ((*it)!=NULL) return Edge();
	(*it) = new PhEdge ( n1.m_phnode, n2.m_phnode, i, a);
	m_nbe++;
	return Edge((*it));
}
//====================================================


//====================================================
template <class NA, class EA>
void Graph<NA,EA>::removeNode(Node n)
{
  //	lglASSERT ( n.m_phnode != NULL );
  if (n.m_phnode == NULL) return;
  // remove edges
  typename Node::iterator i;
  for (i=n.begin();i!=n.end();++i) {
    removeEdge(*i);
  }
  // remove from array
  m_node[n.getIndex()] = NULL;
  // delete physical node
  delete n.m_phnode;
  //
  m_nbn--;	
}
//====================================================


//====================================================
template <class NA, class EA>
void Graph<NA,EA>::removeEdge(Edge e)
{
  //lglASSERT ( e.m_phedge != NULL );
  if (e.m_phedge == NULL) return;
  // removes the edge from its end nodes lists
  // from Node
  // manual iteration until find e
  Edge* i = &(e.m_phedge->m_node[0]->m_first_edge);
  while (i->m_phedge != e.m_phedge) {
    i = &(i->m_phedge->m_next_edge[i->m_ori]);
  }
  // unlink 
  (*i) = i->m_phedge->m_next_edge[i->m_ori];
  // to Node
  // manual iteration until find e
  i = &(e.m_phedge->m_node[1]->m_first_edge);
  while (i->m_phedge != e.m_phedge) {
    i = &(i->m_phedge->m_next_edge[i->m_ori]);
  }
  // unlink 
  (*i) = i->m_phedge->m_next_edge[i->m_ori];
  //
  // remove from array
  m_edge[e.getIndex()] = NULL;
  // delete physical edge
  delete e.m_phedge;
  //
  m_nbe--;	
}
//====================================================



//====================================================
template <class NA, class EA>
void Graph<NA,EA>::changeNodeFrom(Edge e, Node n)
{
	lglASSERT ( e.m_phedge != NULL );
	lglASSERT ( n.m_phnode != NULL );
	
	//=================================================
	// unlink from the old node from list
	// manual iteration until find e
	Edge* i = &(e.m_phedge->m_node[e.m_ori]->m_first_edge);
	while (i->m_phedge != e.m_phedge) {
		i = &(i->m_phedge->m_next_edge[i->m_ori]);
	}
	// unlink 
	(*i) = i->m_phedge->m_next_edge[i->m_ori];
	//=================================================
	// insert into the new node from list
	// if (n==e.getFinalNode()) : loop => already in the list (in opposite orientation)
	if (n!=e.getFinalNode()) {
		e.m_phedge->m_next_edge[e.m_ori] = n.m_phnode->m_first_edge;
		n.m_phnode->m_first_edge = e;
	}
	// pointer on node from
	e.m_phedge->m_node[e.m_ori] = n.m_phnode;
}
//====================================================




//====================================================
template <class NA, class EA>
void Graph<NA,EA>::changeNodeTo(Edge e, Node n)
{
	lglASSERT ( e.m_phedge != NULL );
	lglASSERT ( n.m_phnode != NULL );
	
	//=================================================
	// unlink from the old node from list
	// manual iteration until find e
	Edge* i = &(e.m_phedge->m_node[1-e.m_ori]->m_first_edge);
	while (i->m_phedge != e.m_phedge) {
		i = &(i->m_phedge->m_next_edge[i->m_ori]);
	}
	// unlink 
	(*i) = i->m_phedge->m_next_edge[i->m_ori];
	//=================================================
	// insert into the new node to list
	// if (n==e.getInitialNode()) : loop => already in the list (in opposite orientation)
	if (n!=e.getInitialNode()) {
		e.m_phedge->m_next_edge[1-e.m_ori] = n.m_phnode->m_first_edge;
		n.m_phnode->m_first_edge = e;
	}
	// pointer on node from
	e.m_phedge->m_node[1-e.m_ori] = n.m_phnode;
}
//====================================================




//====================================================
// PhysicalGraphNode
//====================================================


//====================================================
template <class NA, class EA>
PhysicalGraphNode<NA,EA>::PhysicalGraphNode(Index i) 
:
m_index(i),
m_attr(),
m_mark(0),
m_first_edge()
{}
//====================================================


//====================================================
template <class NA, class EA>
PhysicalGraphNode<NA,EA>::PhysicalGraphNode(Index i, const NA& a) 
:
m_index(i),
m_attr(a),
m_mark(0),
m_first_edge()
{}
//====================================================
 
//====================================================
// PhysicalGraphEdge
//====================================================


//====================================================
template <class NA, class EA>
PhysicalGraphEdge<NA,EA>::PhysicalGraphEdge(pPhNode n1, pPhNode n2, Index i) 
:
m_index(i),
m_attr(),
m_mark(0)
{
	// pointer on node from and node to 
	m_node[0] = n1;
	m_node[1] = n2;
	// front insertion into from node's edge list
	m_next_edge[0] = n1->m_first_edge; 
	n1->m_first_edge = Edge(this,0);
	// front insertion into to node's edge list
	if (n1!=n2) {
		m_next_edge[1] = n2->m_first_edge; 
		n2->m_first_edge = Edge(this,1);
	}
	else {
		// loops : must not be inserted twice !
		m_next_edge[1] = m_next_edge[0];
	}
}
//====================================================


//====================================================
template <class NA, class EA>
PhysicalGraphEdge<NA,EA>::PhysicalGraphEdge(pPhNode n1, pPhNode n2, Index i, const EA& a) 
:
m_index(i),
m_attr(a),
m_mark(0)
{
	// pointer on node from and node to 
	m_node[0] = n1;
	m_node[1] = n2;
	// front insertion into from node's edge list
	m_next_edge[0] = n1->m_first_edge; 
	n1->m_first_edge = Edge(this,0);
	// front insertion into to node's edge list
	if (n1!=n2) {
		m_next_edge[1] = n2->m_first_edge; 
		n2->m_first_edge = Edge(this,1);
	}
	else {
		// loops : must not be inserted twice !
		m_next_edge[1] = m_next_edge[0];
	}
}
//====================================================




