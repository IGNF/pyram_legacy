
namespace lgl {
/// Builds a Region Adjacency Graph (RAG) of type G based on the Neighborhood N
/// from a Label Image of type I. 
/// The graph is a simple graph, i.e. encodes topological adjacency.
/// The graph's nodes indices correspond to the labels of the regions. 
/// If exterior_node is set to TRUE, builds a virtual node corresponding to the exterior domain of the image 
/// and connects all boundary regions to it.
template <class I, class G>
LabelImageToRAG ( const I& image, Neighborhood& N, G& graph, BOOL exterior_node )
{
	// Nodes creation/count
	int lmax = 0;
	typename I::const_iterator it;
	for (it=image.begin();it!=image.end();++it) {
		// If the index is already occupied : does nothing (returns NullNode)
		graph.addNode(*it);
		if (*it>lmax) lmax=*it;
	}

	// Edges creation
	ImageBloc b = image.domain();
	for (b.begin();!b.end();++b) {
		int lf = image.iget(*b);
		for (N.begin();!N.end();++N) {
			ImageSite V = *b + *N;
			if (!image.is_inside(V)) continue;
			int lt = image.iget(V);
			if (lf==lt) continue;
			if (lf>lt) {int tmp=lf; lf=lt; lt=tmp;}
			typename G::Node nf = graph.getNode(lf);
			typename G::Node nt = graph.getNode(lt);
			graph.addEdge(nf,nt,lf*lmax+lt);
		}
	}

}

}
//===========================================================
// EO namespace lgl
// EOF
//===========================================================
