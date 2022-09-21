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
#ifndef __sxsD_ImageCovariance_h__
#define __sxsD_ImageCovariance_h__
//===========================================================
#include <sxs/sxsD_Covariance.h>
#include <sxs/macroSortieMessage.h>
//===========================================================
// namespace	: sxs
// classes		: 
//  D_ImageCovariance_User
//		Inherited from D_Covariance_User. Gets the observations from an image.
//	D_ImageCovariance_NodeAttribute
//	D_ImageCovariance_EdgeAttribute
/*! \file 
 \brief Classes implementing fidelity energies (D) based on a variance/covariance matrix of the values of an image within the regions.
 */
//===========================================================
// author		: Laurent Guigues
// history		: 
//	25/07/04		creation
//===========================================================


namespace sxs {

	//===========================================================
	/// Minimal graph node attribution when using the energy class D_CovarianceFromImage
	typedef D_Covariance_NodeAttribute D_ImageCovariance_NodeAttribute;
	//===========================================================

	//===========================================================
	/// Minimal graph edge attribution when using the energy class D_CovarianceFromImage
	typedef D_Covariance_EdgeAttribute D_ImageCovariance_EdgeAttribute;
	//===========================================================

	//===========================================================
	/// \brief Implements a fidelity energy (D) based on a variance/covariance matrix of the values of an image within the regions.
	class D_ImageCovariance_User: public virtual D_Covariance_User {
	protected:
		const lgl::ImageIn* m_image;
	public:
		D_ImageCovariance_User() :
				m_image(0) {
		}

		//===========================================================
		/// Sets the image to be used.
		///
		/// \note MUST BE SET AFTER THE IMAGE IS ALLOCATED AS IT USES THE NUMBER OF CHANNELS OF THE IMAGE TO SET THE DIMENSION OF THE OBSERVATION SPACE !
		void setImage(const lgl::ImageIn& im) {
			m_image = &im;
			// updates the dimension of the node attributes according to the number of channels in the image
			D_Covariance_NodeAttribute::dimension() = m_image->channels();
		}
		//===========================================================

		//===========================================================
		/// Called at the insertion of an ImageSite in a Node (called in ImageScaleClimbing::buildGraph()).
		///
		/// Reads the values of the observation from the image
		virtual void SCCB_D_addSiteToNode(Node n, const lgl::ImageSite& s) {
			lgl::Vector<lgl::F32> obs(m_image->channels());
			lgl::ImageSite t(s);
			for (int i = 0; i < m_image->channels(); ++i) {
				t(0) = i;

				//SORTIEMESSAGE("Test de t : ");

				if (!m_image->is_inside(t)) {
					SORTIEMESSAGE(t);
					SORTIEMESSAGE(" n'est pas dans l'image : ");
					SORTIEMESSAGE("\n");
				}
//	  else {
				//	  SORTIEMESSAGE("OK c'est dans l'image : \n");
				obs[i] = m_image->get(t);
//	  }

			}
			D_Covariance_NodeAttribute* a = dynamic_cast<D_Covariance_NodeAttribute*>(n.getAttribute());
			a->addObservation(obs);
		}
		//===========================================================
	};
//===========================================================
// EO D_ImageCovariance_User
//===========================================================

}
//===========================================================
// EO namespace sxs
//===========================================================

#endif

