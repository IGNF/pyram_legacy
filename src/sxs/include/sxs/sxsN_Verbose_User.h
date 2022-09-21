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
#ifndef __sxsN_Verbose_User_h__
#define __sxsN_Verbose_User_h__
//===========================================================
// namespace	: sxs 
// classe		: N_Verbose_User
/*! \file 
 \brief Descendant of ScaleClimbingUser which overloads the notification callbacks in order to display progress messages during the algorithm.
 */
//===========================================================
// author		: Laurent Guigues
// history		: 
//	27/07/04		creation
//===========================================================
#include "sxs/sxsGeneral.h"
#include "sxs/sxsImageScaleClimbing.h"
#include "sxs/sxsImageScaleClimbingUser.h"
#include "lgl/lglImageManip.h"

// #include "wx/stopwatch.h"

#undef M_PI
#include <cmath>

namespace sxs {

	//==================================================================================
	// if defined : compilation of N_Verbose_User's messages
	// #define __SXS_VSCU_COMPILE_MESSAGE__
	//==================================================================================
	// if defined : compilation of N_Verbose_User's warning/error messages
	// #define __SXS_VSCU_COMPILE_ERROR__
	//==================================================================================

	//==================================================================================
	// MESSAGES
	//==================================================================================
#define __SXS_VSCU_COMPILE_MESSAGE__
#if defined(__SXS_VSCU_COMPILE_MESSAGE__)
	// If messages compiled then errors are compiled
#ifndef __SXS_VSCU_COMPILE_ERROR__
#define __SXS_VSCU_COMPILE_ERROR__
#endif
	//==================================================================================
	/// Displays a simple message if verboseLevel()>=V
#define sxsVSCU_MESSAGE(V,A) if (verboseLevel()>= V ) { lglLOG ( A ); }
	/// Displays a newline tabulated message if verboseLevel()>=V
#define sxsVSCU_NLMESSAGE(V,A) if (verboseLevel()>= V ) { lglLOG ( ENDL << /*Tab().c_str() <<*/ A); FLUSH }
	//==================================================================================
#else
	//==================================================================================
#define sxsVSCU_MESSAGE(V,A)
#define sxsVSCU_NLMESSAGE(V,A)
	//==================================================================================
#endif
	//==================================================================================

	//===========================================================
	///	\brief Descendant of ImageScaleClimbingUser which overloads the notification callbacks in order to display progress messages during the algorithm.
	class N_Verbose_User: public virtual ImageScaleClimbingUser {
	public:

		//===========================================================
		/// ctor
		N_Verbose_User(lgl::I32 verbose_level = 2, lgl::BOOL display_progress = true,
				lgl::BOOL stop_on_warning = true) :
				m_verb_level(verbose_level), m_display_progress(display_progress), m_stop_on_warning(stop_on_warning) {
		}
		/// dtor
		virtual ~N_Verbose_User() {
		}
		//===========================================================

		//===========================================================
		/// returns a ref on the level of verbosity of the class
		lgl::I32& verboseLevel() {
			return m_verb_level;
		}
		/// returns a const ref on the level of verbosity of the class
		const lgl::I32& verboseLevel() const {
			return m_verb_level;
		}
		//===========================================================
		/// returns a ref on the display progress variable
		lgl::BOOL& displayProgress() {
			return m_display_progress;
		}
		/// returns a const ref on the display progress variable
		const lgl::BOOL& displayProgress() const {
			return m_display_progress;
		}
		/// returns a ref on the stop on warning variable
		lgl::BOOL& stopOnWarning() {
			return m_stop_on_warning;
		}
		/// returns a const ref on the stop on warning variable
		const lgl::BOOL& stopOnWarning() const {
			return m_stop_on_warning;
		}
		//===========================================================

		//===========================================================
		/// Memorizes the ScaleClimbing algorithm in order to access its structures (graph...)
		void setAlgorithm(ImageScaleClimbing& algo) {
			m_algo = &algo;
		}
		//===========================================================

		//===========================================================
		// Ponctual callbacks notifying the current algorithm state, progress...
		//===========================================================
		/// Called back before the algorithm runs (beginning of run())
		virtual void SCCB_N_preRun() {
			sxsVSCU_NLMESSAGE(1, "[Image Scale Climbing (c) Laurent Guigues 2004");
			TabRight();
			//m_watch.Start();
		}
		/// Called back when the algorithm has finished (end of run())
		virtual void SCCB_N_postRun() {
			TabLeft();
			//sxsVSCU_NLMESSAGE(1, "[Total time = "<<m_watch.Time()<<" ms ]"<<ENDL);
		}
		/// Called back before the computation base segmentation (beginning of computeBaseSegmentation())
		virtual void SCCB_N_preComputeBaseSegmentationNotification() {
			sxsVSCU_NLMESSAGE(2, "[Base segmenting ");
			TabRight();
		}
		/// Called back after the computation base segmentation (end of computeBaseSegmentation())
		virtual void SCCB_N_postComputeBaseSegmentationNotification() {

			if (verboseLevel() >= 3) {
				// min/max label
				double min, max;
				lgl::computeMinMax(m_algo->getBaseSegmentation(), min, max);
				lglLOG(" : min/max label = "<<min<<"/"<<max<<" ");
			}

			TabLeft();
			sxsVSCU_MESSAGE(2, " OK ]    ");
		}
		/// Called back before the graph contruction (beginning of buildGraph())
		virtual void SCCB_N_preBuildGraphNotification() {
			sxsVSCU_NLMESSAGE(2, "[Building Graph  ");
			TabRight();
		}
		/// Called back after the graph contruction (end of buildGraph())
		virtual void SCCB_N_postBuildGraphNotification() {
			sxsVSCU_MESSAGE(3, " : " <<m_algo->getGraph().getNodes().size()<<" nodes / " <<m_algo->getGraph().getEdges().size()<<" edges");
			TabLeft();
			sxsVSCU_MESSAGE(2, " OK ]    ");
		}
		/// Called back before the ScaleSets's Base contruction (beginning of buildBase())
		virtual void SCCB_N_preBuildBaseNotification() {
			sxsVSCU_NLMESSAGE(2, "[Building Base   ");
			TabRight();
		}
		/// Called back after the ScaleSets's Base contruction (end of buildBase())
		virtual void SCCB_N_postBuildBaseNotification() {
			TabLeft();
			sxsVSCU_MESSAGE(2, " OK ]    ");
		}
		/// Called back before the Heap construction (beginning of buildHeap())
		virtual void SCCB_N_preBuildHeapNotification() {
			sxsVSCU_NLMESSAGE(2, "[Building Heap   ");
			TabRight();
		}
		/// Called back after the Heap construction (end of buildHeap())
		virtual void SCCB_N_postBuildHeapNotification() {
			TabLeft();
			sxsVSCU_MESSAGE(2, " OK ]    ");
		}
		/// Called back before climbing starts (beginning of climb())
		virtual void SCCB_N_preClimbingNotification() {
			sxsVSCU_NLMESSAGE(2, "[Scale Climbing  ");
			TabRight();
		}
		/// Called back after climbing (end of climb())
		virtual void SCCB_N_postClimbingNotification() {
			sxsVSCU_MESSAGE(3,
					" : " <<m_algo->getScaleSets().size()<<" sets / "<< "root soa = "<<m_algo->getScaleSets().getRoot()->scaleOfAppearance());
			TabLeft();
			sxsVSCU_MESSAGE(2, " OK ]    ");
		}
		/// Called back before post processing starts (beginning of postProcess())
		virtual void SCCB_N_prePostprocessingNotification() {
			sxsVSCU_NLMESSAGE(2, "[Post Treatment  ");
			TabRight();
		}
		/// Called back after post processing starts (end of postProcess())
		virtual void SCCB_N_postPostprocessingNotification() {
			TabLeft();
			sxsVSCU_MESSAGE(2, " OK ]    ");
		}
		/// Called back before each ScaleClimbing step (beginning of step())
		//virtual void SCCB_N_preStepNotification() { }
		/// Called back after each ScaleClimbing step (end of step())
		//virtual void SCCB_N_postStepNotification() { }
		/// Called back at the beginning of each functionalDynamicProgrammingStep(e)
		//virtual void SCCB_N_preFDP(Edge e) { }
		/// Called back at the end of each functionalDynamicProgrammingStep(e)
		//virtual void SCCB_N_postFDP(Edge e) { }
		//===========================================================
		/// Gives a progress indicator for the current task
		virtual bool SCCB_N_progressIndicator(int stage, int number_of_stages) {
			if (!m_display_progress)
				return true;
			//

			static int exp = -1;
			int prec = 3;
			int p = (int) (stage * pow(10.0, prec) * 1. / number_of_stages);
			//std::cout << "DEBUG " << std::string(p) << std:endl;

			if (p == exp)
				return true;
			exp = p;
			float s = p * 100. / pow(10.0, prec);
			char fmt[128];
			sprintf(fmt, "%%%i.%if%%%%", prec < 4 ? prec : prec + 1, prec > 3 ? prec - 3 : 0);
			char ch[128];

			sprintf(ch, fmt, s);
			sxsVSCU_MESSAGE(2, ch);
			for (int i = 0; i < (prec > 3 ? prec + 2 : prec + 1); ++i)
				sxsVSCU_MESSAGE(2, "\b");
			/*
				return true;

				if (display.count() == 0) {
				display.restart(number_of_stages);
				}
				++display;
				*/
			return true;
		}
		//===========================================================

		//===========================================================
		// Warning/Error callbacks
		//===========================================================
		/// Warning : algorithm can continue but might give strange results.
		/// If the CB returns TRUE then continues else stops.
		virtual lgl::BOOL SCCB_warning(int number, lgl::String& description) {
			sxsWARNING ( description )
			return !m_stop_on_warning;
		}
		/// Fatal error : algorithm cannot continue (in fact stops just after).
		virtual void SCCB_error(const lgl::String& description) {
		sxsERROR ( description )
	}
	//===========================================================

protected:
	//==================================================================================
	/// Level of verbosity
	lgl::I32 m_verb_level;
	/// Display progress indicator ?
	lgl::BOOL m_display_progress;
	/// Stop on warning ?
	lgl::BOOL m_stop_on_warning;
	/// Memorizes the ScaleClimbing algorithm in order to access its structures (graph...)
	ImageScaleClimbing* m_algo;

	/// Stop watch for timing
	//wxStopWatch m_watch;
	//==================================================================================
	// Messages tab
	//==================================================================================
	static lgl::String& Tab() {
#ifndef LGL_WITH_BOOST
		lgl::String _TABCh(std::string(""));
#else
		static lgl::String _TABCh("");
#endif
		return _TABCh;
	}
#if defined(__SXS_VSCU_COMPILE_MESSAGE__)
	static void TabRight() {
#ifndef LGL_WITH_BOOST
		Tab() += lgl::String(std::string(" "));
#else
		Tab() += lgl::String(" ");
#endif

	}
	static void TabLeft() {
		Tab().resize(Tab().size() - 1);
	}
#else
	static void TabRight() {}
	static void TabLeft() {}
#endif

	};
//===========================================================
// EO class N_Verbose_User
//===========================================================

}

//===========================================================
// EOF
//===========================================================
#endif
