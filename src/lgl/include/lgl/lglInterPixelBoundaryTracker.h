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
 \brief Classes for the tracking of the inter-pixel boundaries between the 4-connected flat zones of an image
 */
#ifndef __lglInterPixelBoundaryTracker_h__
#define __lglInterPixelBoundaryTracker_h__
//===========================================================
// namespace	: lgl
// classes	: InterPixelBoundaryTracker
//===========================================================
// author		: Laurent Guigues
// history		: 
// 02/11/04 : creation
//===========================================================
#include "lgl/lglGeneral.h"
#include "lgl/lglImageRam.h"
#include "lgl/lglPoint2D.h"


namespace lgl {

	//==============================================================================================================
	/// \brief An inter-pixel boundary element defined by a inter-pixel initial position and a direction.
	///
	/// The inter-pixel position (0,0) corresponds to the upper-left corner of the pixel (0,0) of the image.
	/// The inter-pixel position (c+1,l+1) corresponds to the lower-right corner of the pixel (c,l) of the image.
	/// Directions correspond to the 4 Freeman codes for inter-pixel edges (N/S/E/W).
	class Edgel {
	public:
		/// Ctor
		Edgel(IPoint2D p = IPoint2D(0, 0), I8 d = E) :
				m_p(p), m_d(d) {
		}

		/// Returns the position of the Edgel
		inline const IPoint2D& getPosition() const {
			return m_p;
		}
		/// Returns the direction of the Edgel
		inline I8 getDirection() const {
			return m_d;
		}

		/// Sets the Edgel position and direction
		inline void set(IPoint2D p, I8 d) {
			m_p = p;
			m_d = d;
		}
		/// Steps to the next Edgel in the current direction
		inline void stepForward() {
			m_p += m_D[m_d];
		}
		/// Steps to the previous Edgel in the current direction
		inline void stepBackward() {
			m_p += m_D[(m_d + 2) % 4];
		}
		/// Turns right
		inline void turnRight() {
			(m_d == 0) ? m_d = 3 : m_d--;
		}
		/// Turns left
		inline void turnLeft() {
			(m_d == 3) ? m_d = 0 : m_d++;
		}
		/// Turns back (same position, opposite direction)
		inline void turnBack() {
			(m_d < 2) ? m_d += 2 : m_d -= 2;
		}
		/// Reverses (forwarded position, opposite direction)
		inline void reverse() {
			stepForward();
			turnBack();
		}

		/// Returns the ImageSite corresponding to the pixel on the left of the Edgel
		inline ImageSite getLeftSite() const {
			return ImageSite(0, m_p(0) + m_L[m_d](0), m_p(1) + m_L[m_d](1));
		}
		/// Returns the ImageSite corresponding to the pixel on the left of the Edgel
		inline ImageSite getRightSite() const {
			return ImageSite(0, m_p(0) + m_R[m_d](0), m_p(1) + m_R[m_d](1));
		}
		/// Returns the IPoint2D corresponding to the pixel on the left of the Edgel
		inline IPoint2D getLeftPoint() const {
			return m_p + m_L[m_d];
		}
		/// Returns the IPoint2D corresponding to the pixel on the left of the Edgel
		inline IPoint2D getRightPoint() const {
			return m_p + m_R[m_d];
		}

		/// Returns the "center" of the Edgel
		inline FPoint2D getCenter() const {
			return FPoint2D(m_p(0) + 0.5 * m_D[m_d](0), m_p(1) + 0.5 * m_D[m_d](1));
		}

		/// Operator ==
		inline bool operator==(const Edgel& e) const {
			return (m_p == e.m_p) && (m_d == e.m_d);
		}

		/// Direction codes
		static const I8 E, S, W, N;
		/// Static data initialization
		static void init_static_data();

	protected:
		IPoint2D m_p;
		UI8 m_d;

		/// Displacement vectors corresponding to directions
		static IPoint2D m_D[4];
		///  Relative positions of left / right pixels corresponding to directions
		static IPoint2D m_L[4];
		static IPoint2D m_R[4];
	};
	// EO class Edgel
	//==============================================================================================================

	//==============================================================================================================
	/// \brief A vector of Edgel describing the boundary between two regions of an image (different left/right values)
	class InterPixelBoundary: public std::vector<Edgel> {
	public:
		/// Default ctor
		InterPixelBoundary() :
				m_l(-1), m_r(-1) {
		}

		/// Returns the value of the image on the left of the boundary
		inline double getLeftValue() const {
			return m_l;
		}
		/// Returns the value of the image on the right of the boundary
		inline double getRightValue() const {
			return m_r;
		}
		/// Sets the value of the image on the left of the boundary
		inline void setLeftValue(double v) {
			m_l = v;
		}
		/// Sets the value of the image on the left of the boundary
		inline void setRightValue(double v) {
			m_r = v;
		}

	protected:

		/// left and right value
		double m_l, m_r;

	};
	// EO class InterPixelBoundary
	//==============================================================================================================

	//==============================================================================================================
	/// \brief Tracker of the inter-pixel boundaries between the 4-connected flat zones of an image
	class InterPixelBoundaryTracker {
	public:

		/// \brief Constructor
		///
		/// \param im : input image
		/// \param track_image_frame set to true indicates that the tracker must follow the frame of the image
		/// \param extern_value gives the value of the image outside its domain (for image frame tracking)
		InterPixelBoundaryTracker(const ImageRam& im, bool track_image_frame = false, double extern_value = -1);

		/// \brief Tracks the boundaries of the image and stores them in bnd
		void track(std::vector<InterPixelBoundary>& bnd);

		/// Starts the tracking at the Edgel e
		inline void start(const Edgel& e) {
			m_ini = m_cur = e;
			m_left_value = getLeftValue();
			m_right_value = getRightValue();
		}
		/// Restarts the tracking at the current position
		inline void restartHere() {
			m_ini = m_cur;
			m_left_value = getLeftValue();
			m_right_value = getRightValue();
		}

		/// Steps along the current boundary. Returns false if end
		inline bool step();
		/// "Rewinds" to the beginning of the boundary
		inline void rewind();

		/// Returns true iff the Point is inside the image domain
		inline bool is_inside(const IPoint2D& p) {
			return (p(0) >= 0) && (p(0) < m_nc) && (p(1) >= 0) && (p(1) < m_nl);
		}

		/// Returns the value of the image on the left of the current Edgel
		inline double getLeftValue() {
			ImageSite p = m_cur.getLeftSite();
			if (m_im->is_inside(p))
				return m_im->get(p);
			else
				return m_extern_value;
		}
		/// Returns the value of the image on the right of the current Edgel
		inline double getRightValue() {
			ImageSite p = m_cur.getRightSite();
			if (m_im->is_inside(p))
				return m_im->get(p);
			else
				return m_extern_value;
		}

	private:
		/// The image
		const ImageRam* m_im;
		/// Track the image frame ?
		bool m_track_image_frame;
		/// Value outside the image
		double m_extern_value;
		/// Initial and current Edgel
		Edgel m_ini, m_cur;
		/// Image size
		I32 m_nc, m_nl;
		/// Current left and right image values
		double m_left_value, m_right_value;

	};

///////////////////////////////////////////////////////////////////////
	/** Algo de suivi des frontieres inter-regions d'une image de labels :
	 avec appel de la methode Add(const InterPixelBoundaryTracker<T>& t) de A a chaque segment de frontiere.

		Renvoie un InterPixelBoundarySet avec les conventions :
		- L'entrée dans la PILE se fait par le label de la région gauche
		- Le label de la région gauche est toujours plus petit que celui de la droite
		- Les portions déconnectées de frontieres entre deux regions forment autant d'entree dans la deuxieme pile
		*/
	void findInterPixelBoundaries(const ImageRam& l, std::vector<InterPixelBoundary>);

}
// EO namespace lgl

#endif
// EOF

