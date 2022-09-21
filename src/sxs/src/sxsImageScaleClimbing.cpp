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
 \brief Code of the methods of the class ImageScaleClimbing and of nested classes (Parameters).
 */

#include <sxs/sxsImageScaleClimbing.h>
//#include "../../lgl/include/lglType.h"
#include <lgl/lglImageRam.h>
#include <lgl/lglVoronoiImageTesselation.h>
#include <lgl/lglInterPixelBoundaryTracker.h>
#include <sxs/macroSortieMessage.h>


namespace sxs {
	//===========================================================
	/// Parameters of the ImageScaleClimbing algorithm
	ImageScaleClimbingParameters::ImageScaleClimbingParameters() :
			m_image_neighborhood(lgl::Neighborhood::d2_4n), m_masking_enabled(false), m_masking_label(0) {
	}

	void ImageScaleClimbingParameters::enable_masking(bool enable, int label) {
		m_masking_enabled = enable;
		m_masking_label = label;
	}

	//=================================================================================

	ImageScaleClimbing::ImageScaleClimbing() :
			m_image(0), m_base(0), m_iuser(0), m_iparam(0) {
	}
	//=================================================================================

	//=================================================================================

	ImageScaleClimbing::~ImageScaleClimbing() {
		//	if (m_must_delete_graph && (m_graph!=0)) delete m_graph;
	}
	//=================================================================================

	//=================================================================================
	void ImageScaleClimbing::setParameters(const ScaleClimbingParameters* p) {
		m_param = p;
		// dynamic downcast
		m_iparam = dynamic_cast<const ImageScaleClimbingParameters*>(p);
		if (!m_iparam) {
		sxsERROR("ImageScaleClimbing::setParameters : the ScaleClimbingParameters given is NOT an ImageScaleClimbingParameters (dynamic downcast failed)")}
}
//=================================================================================

//=================================================================================
void ImageScaleClimbing::setImage(lgl::ImageRam* i) {
	m_image = i;
}
//=================================================================================

//=================================================================================
void ImageScaleClimbing::setBaseSegmentation(lgl::ImageRam* b) {
	m_base = b;
}
//=================================================================================

//=================================================================================
void ImageScaleClimbing::setUser(ScaleClimbingUser* u) {
	m_user = u;
	m_user->setScaleClimbing(this);
	m_iuser = dynamic_cast<ImageScaleClimbingUser*>(u);
	if (!m_iuser) {
		sxsERROR("ImageScaleClimbing::setUser : the ScaleClimbingUser given is NOT an ImageScaleClimbingUser (dynamic downcast failed)")
	}
}
//=================================================================================

//=================================================================================
void ImageScaleClimbing::setScaleSets(ScaleSets& u) {
	m_scale_sets = &u;
	m_iscale_sets = dynamic_cast<ImageScaleSets*>(&u);
	if (!m_iscale_sets) {
		sxsERROR("ImageScaleClimbing::setScaleSets : the ScaleSets given is NOT an ImageScaleSets (dynamic downcast failed)")}
		//
	}
	//=================================================================================

	//=================================================================================
	lgl::BOOL
	ImageScaleClimbing::run()
	{
		//==========================================
		//testRunningPossible();
		//==========================================
		// CB begin
		m_user->SCCB_N_preRun();
		//==========================================
		m_iscale_sets->setInputImage(m_image);
		// Base segmentation
		if (!computeBaseSegmentation())
			return false;
		// gives it to the ImageScaleSets
		m_iscale_sets->setBaseImage(m_base);
		// Build graph
		if (!buildGraph())
			return false;
		// Same as ScaleClimbing :
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
	// Template method used to compute oversegmentation from any input image type
	// called via the macro lglSwitchOnTypeFunctionCall4
	template<class T>
	void TcomputeBaseSegmentation(T, ImageScaleClimbing* isc, lgl::ImageRam* in, lgl::ImageRamT<int>* out) {
		lgl::ImageRamT<T>* Tin = dynamic_cast<lgl::ImageRamT<T>*>(in);

		// Today : only local optima generalized voronoi partition
		// To do : other methods
		int m_oversegmentation_size;
		const ImageScaleClimbingParameters* iscp = dynamic_cast<const ImageScaleClimbingParameters*>(&isc->getParameters());
		lgl::Neighborhood neigh = iscp->getNeighborhood();
		lgl::LocalOptimaVoronoiTesselation(*Tin, neigh, *out, m_oversegmentation_size);

	}
	//=================================================================================

	//=================================================================================

	lgl::BOOL
	ImageScaleClimbing::computeBaseSegmentation()
	{
		if (m_base)
			return true;
		else if (!m_image) {
			// Error : no input
#ifndef LGL_WITH_BOOST
			m_iuser->SCCB_N_error(lgl::String(wxT("No input (neither image nor base segmentation)")));
#else
			m_iuser->SCCB_N_error(lgl::String("No input (neither image nor base segmentation)"));
#endif
			return false;
		}

		//==========================================
		// CB pre
		m_iuser->SCCB_N_preComputeBaseSegmentationNotification();
		//==========================================

		lgl::ImageRamT<int>* m_seg = new lgl::ImageRamT<int>;

		// Switches on m_image->type() to call the
		// template function TcomputeBaseSegmentation on the type of the values of m_image.
		// TcomputeBaseSegmentation will downcast m_image into an appropriate RamImage<T>
		int t = m_image->type();
		lglSwitchOnTypeFunctionCall4(t, TcomputeBaseSegmentation, this, m_image, m_seg);
		m_base = m_seg;

		//==========================================
		// CB post
		m_iuser->SCCB_N_postComputeBaseSegmentationNotification();
		//==========================================

		return true;
	}
	//=================================================================================

	//=================================================================================
	lgl::BOOL
	ImageScaleClimbing::buildGraph()
	{

		if (!m_graph) {
			m_graph = new Graph;
		} else {
			m_graph->clear();
		}

		//==========================================
		// CB pre
		m_iuser->SCCB_N_preBuildGraphNotification();
		//==========================================

		int bgm = m_iuser->SCCB_Q_buildGraphMode();

		//=====================================================================================
		if (bgm == ImageScaleClimbingUser::BuildGraphBasic) {
			//==========================================
			// Edgel / Edgel construction
			//==========================================
			int stage_step = 1000;
			m_number_of_stages = 2 * m_base->numberOfSites();
			m_stage = 0;

			// Nodes creation/count
			int lmax = 0;
			lgl::ImageBloc b = m_base->domain();

//	  SORTIEMESSAGE("BUILD GRAPH : ");
//	  SORTIEMESSAGE(m_base->size());
//	  SORTIEMESSAGE(" - ");
//	  SORTIEMESSAGE(m_base->domain());
			for (b.start(); !b.stop(); ++b) {
				//
				int l = (int) (m_base->get(*b));

				// ignore pixel if masking is enabled and label must be masked
				if (m_iparam->is_masking_enabled() && l == m_iparam->get_masking_label()) {
					m_stage++;
					continue;
				}

				// If the index is not already occupied :
				// effective Node creation and user's callback
				Node n = m_graph->addNode(l);
				if (n != Node::Null()) {
					n.getAttribute() = m_iuser->SCCB_Q_newNodeAttribute();
					m_iuser->SCCB_C_onNodeCreation(n);
					m_iuser->SCCB_D_onNodeCreation(n);
					m_iuser->SCCB_C_addSiteToNode(n, *b);
					m_iuser->SCCB_D_addSiteToNode(n, *b);
					if (l > lmax)
						lmax = l;
				}
				// else : just callback to addSiteToNode
				else {
					m_iuser->SCCB_C_addSiteToNode(m_graph->getNode(l), *b);
					m_iuser->SCCB_D_addSiteToNode(m_graph->getNode(l), *b);
				}

				m_stage++;
				if (m_stage % stage_step == 0) {
					if (!m_iuser->SCCB_N_progressIndicator(m_stage, m_number_of_stages))
						return false;
				}
			}

			// Edges creation
			lgl::Neighborhood N = m_iparam->getNeighborhood();
			for (b.start(); !b.stop(); ++b) {
				int lf = (int) (m_base->get(*b));

				// ignore pixel if masking is enabled and label must be masked
				if (m_iparam->is_masking_enabled() && lf == m_iparam->get_masking_label()) {
					m_stage++;
					continue;
				}

				for (N.begin(); !N.end(); ++N) {
					lgl::ImageSite V = *b + *N;
					if (!m_base->is_inside(V))
						continue;
					int lt = (int) (m_base->get(V));

					// ignore pixel if masking is enabled and label must be masked
					if (m_iparam->is_masking_enabled() && lt == m_iparam->get_masking_label()) {
						continue;
					}

					if (lf == lt)
						continue;
					if (lf > lt) {
						int tmp = lf;
						lf = lt;
						lt = tmp;
					}
					Node nf = m_graph->getNode(lf);
					Node nt = m_graph->getNode(lt);
					Edge e = m_graph->addEdge(nf, nt, lf * lmax + lt);
					// if new edge : cb
					if (e != Edge::Null()) {
						e.getAttribute() = m_iuser->SCCB_Q_newEdgeAttribute();
						m_iuser->SCCB_C_onEdgeCreation(e);
						m_iuser->SCCB_D_onEdgeCreation(e);
						m_iuser->SCCB_C_addEdgelToEdge(e, *b, V);
						m_iuser->SCCB_D_addEdgelToEdge(e, *b, V);
					} else {
						m_iuser->SCCB_C_addEdgelToEdge(m_graph->getEdge(lf * lmax + lt), *b, V);
						m_iuser->SCCB_D_addEdgelToEdge(m_graph->getEdge(lf * lmax + lt), *b, V);
					}
				}
				m_stage++;
				if (!(m_stage % stage_step)) {
					if (!m_iuser->SCCB_N_progressIndicator(m_stage, m_number_of_stages))
						return false;
				}
			}

		}
		//=====================================================================================
		else if (bgm == ImageScaleClimbingUser::BuildGraph2DBoundaryTracking) {
			//==========================================
			// Boundary tracking
			//==========================================
			lgl::InterPixelBoundaryTracker tracker(*m_base);
			/// TO DO : external boundary
			///,true,-1);

			std::vector<lgl::InterPixelBoundary> bnd;
			lglLOG("track");
			tracker.track(bnd);
			lglLOG(" ok" << std::endl);

			int stage_step = 1000;
			m_number_of_stages = (lgl::I32) (m_base->numberOfSites() + bnd.size());
			m_stage = 0;

			lglLOG("nodes");
			// Nodes creation/count
			int lmax = 0;
			lgl::ImageBloc b = m_base->domain();
			for (b.start(); !b.stop(); ++b) {
				//
				int l = (int) (m_base->get(*b));
				// If the index is not already occupied :
				// effective Node creation and user's callback
				Node n = m_graph->addNode(l);
				if (n != Node::Null()) {
					n.getAttribute() = m_iuser->SCCB_Q_newNodeAttribute();
					m_iuser->SCCB_C_onNodeCreation(n);
					m_iuser->SCCB_D_onNodeCreation(n);
					m_iuser->SCCB_C_addSiteToNode(n, *b);
					m_iuser->SCCB_D_addSiteToNode(n, *b);
					if (l > lmax)
						lmax = l;
				}
				// else : just callback to addSiteToNode
				else {
					m_iuser->SCCB_C_addSiteToNode(m_graph->getNode(l), *b);
					m_iuser->SCCB_D_addSiteToNode(m_graph->getNode(l), *b);
				}

				m_stage++;
				if (m_stage % stage_step == 0) {
					if (!m_iuser->SCCB_N_progressIndicator(m_stage, m_number_of_stages))
						return false;
				}
			}
			lglLOG(" ok" << std::endl << "edges");
			// Edges creation
			int ne = 0;
			std::vector<lgl::InterPixelBoundary>::const_iterator it;
			for (it = bnd.begin(); it != bnd.end(); ++it) {
				int lf = (int) ((*it).getLeftValue());
				int lt = (int) ((*it).getRightValue());
				//	std::cout << lf <<"-"<<lt<<" ";
				Node nf = m_graph->getNode(lf);
				Node nt = m_graph->getNode(lt);
				Edge e = m_graph->addEdge(nf, nt, ne);
				ne++;
				e.getAttribute() = m_iuser->SCCB_Q_newEdgeAttribute();
				m_iuser->SCCB_C_onEdgeCreation(e);
				m_iuser->SCCB_D_onEdgeCreation(e);
				m_iuser->SCCB_C_setEdgeBoundary(e, *it);
				m_iuser->SCCB_D_setEdgeBoundary(e, *it);
				m_stage++;
				if (!(m_stage % stage_step)) {
					if (!m_iuser->SCCB_N_progressIndicator(m_stage, m_number_of_stages))
						return false;
				}
			}
			lglLOG(" ok" << std::endl);
			/*
				std::cout << "nb edg = "<<m_graph->getEdges().size()<<std::endl;
				std::cout<<"nb bnd = "<<bnd.size()<<std::endl;
				for (int i = 0;i<bnd.size();i++) {
				std::cout<<"[ ";
				for (int j=0;j<bnd[i].size();++j) {
				lgl::FPoint2D p = bnd[i][j].getCenter();
				std::cout << p(0)<<","<<p(1)<<" - ";
				}
				std::cout<<"]"<<std::endl;
				}
				*/

		}

		//==========================================
		// CB post
		m_iuser->SCCB_N_postBuildGraphNotification();
		//==========================================

		return true;
	}
//=================================================================================

}