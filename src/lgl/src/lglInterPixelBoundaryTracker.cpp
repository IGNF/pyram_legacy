#include "lgl/lglInterPixelBoundaryTracker.h"

namespace lgl {
	//======================================================================
	// Edgel
	// Static members
	const I8 Edgel::N = 1;
	const I8 Edgel::S = 3;
	const I8 Edgel::E = 0;
	const I8 Edgel::W = 2;
	IPoint2D Edgel::m_D[4];
	IPoint2D Edgel::m_L[4];
	IPoint2D Edgel::m_R[4];
	/// Init
	void Edgel::init_static_data() {
		static bool isinit = false;
		if (isinit)
			return;
		isinit = true;
		m_D[0] = IPoint2D(1, 0);
		m_D[1] = IPoint2D(0, -1);
		m_D[2] = IPoint2D(-1, 0);
		m_D[3] = IPoint2D(0, 1);

		m_L[0] = IPoint2D(0, -1);
		m_L[1] = IPoint2D(-1, -1);
		m_L[2] = IPoint2D(-1, 0);
		m_L[3] = IPoint2D(0, 0);

		m_R[0] = IPoint2D(0, 0);
		m_R[1] = IPoint2D(0, -1);
		m_R[2] = IPoint2D(-1, -1);
		m_R[3] = IPoint2D(-1, 0);
	}
	//======================================================================

	//========================================================================
	// InterPixelBoundaryTracker
	//========================================================================

	/// \brief Constructor
	/// 
	/// \param im : input image
	/// \param track_image_frame set to true indicates that the tracker must follow the frame of the image
	/// \param extern_value gives the value of the image outside its domain (for image frame tracking)
	InterPixelBoundaryTracker::InterPixelBoundaryTracker(const ImageRam& im, bool track_image_frame, double extern_value) :
			m_im(&im), m_track_image_frame(track_image_frame), m_extern_value(extern_value), m_left_value(extern_value), m_right_value(extern_value) {
		m_nc = im.size(1);
		m_nl = im.size(2);
		Edgel::init_static_data();
	}

	//========================================================================
	bool InterPixelBoundaryTracker::step() {
		m_cur.stepForward();
		IPoint2D left = m_cur.getLeftPoint();
		bool left_in = is_inside(left);
		if (!m_track_image_frame && !left_in)
			return false;
		IPoint2D right = m_cur.getRightPoint();
		bool right_in = is_inside(right);
		if ((!m_track_image_frame && !right_in) || (m_track_image_frame && !(left_in || right_in)))
			return false;

		double left_v = m_extern_value;
		if (left_in)
			left_v = m_im->get(ImageSite(0, left(0), left(1)));
		double right_v = m_extern_value;
		if (right_in)
			right_v = m_im->get(ImageSite(0, right(0), right(1)));

		if (left_v == m_left_value) {
			if (right_v == m_right_value) {
				if (m_cur == m_ini)
					return false;
				else
					return true;
			} else if (right_v == m_left_value) {
				m_cur.turnRight();
				if (m_cur == m_ini) {
					m_cur.turnLeft();
					return false;
				} else
					return true;
			}
		} else if ((left_v == m_right_value) && (right_v == m_right_value)) {
			m_cur.turnLeft();
			if (m_cur == m_ini) {
				m_cur.turnRight();
				return false;
			} else
				return true;
		}
		return false;
	}
	//========================================================================

	//========================================================================
	inline void InterPixelBoundaryTracker::rewind() {
		m_cur.stepForward();
		m_cur.turnBack();
		restartHere();
		while (step()) {
		}
		m_cur.turnBack();
		restartHere();
	}
	//========================================================================

	//========================================================================
	// Edgel graph : 
	// its Sites represent Edgel positions and 
	// the ith bit indicates wether the Edgel in direction i is present or not
	typedef ImageRamT<UI8> EdgelGraph;

	inline UI8 EdgelGraphGet(const EdgelGraph& g, const Edgel& p) {
		return (g(0, p.getPosition()(0), p.getPosition()(1)) & (1 << p.getDirection()));
	}
	inline void EdgelGraphSet(EdgelGraph& g, const Edgel& p) {
		g(0, p.getPosition()(0), p.getPosition()(1)) = (g(0, p.getPosition()(0), p.getPosition()(1)) | (1 << p.getDirection()));
	}
	inline void EdgelGraphReset(EdgelGraph& g, const Edgel& p) {
		g(0, p.getPosition()(0), p.getPosition()(1)) = (g(0, p.getPosition()(0), p.getPosition()(1)) & (~(1 << p.getDirection())));
	}
	//========================================================================

	void InterPixelBoundaryTracker::track(std::vector<InterPixelBoundary>& out) {

		// Graph of the Edgels already tracked
		EdgelGraph eGr(ImageSite(1, m_nc + 1, m_nl + 1));
		eGr.mset(0);

		int i, j;
		int i0 = 1, j0 = 1;
		int i1 = m_nc, j1 = m_nl;
		if (m_track_image_frame) {
			i0 = j0 = 0;
			i1++;
			j1++;
		}

		// Horizontal scan
		for (i = 0; i < m_nc; ++i) {
			for (j = j0; j < j1; ++j) {
				// left/right value
				double lv = m_extern_value;
				if (j > 0)
					lv = m_im->get(ImageSite(0, i, j - 1));
				double rv = m_extern_value;
				if (j < m_nl)
					rv = m_im->get(ImageSite(0, i, j));
				if (lv != rv) {
					Edgel ed(IPoint2D(i, j), Edgel::E);
					// Left value is the smallest
					if (rv < lv) {
						double temp = lv;
						lv = rv;
						rv = temp;
						ed.reverse();
					}
					// If Edgel not seen
					if (EdgelGraphGet(eGr, ed) == 0) {
						// New boundary
						out.push_back(InterPixelBoundary());
						InterPixelBoundary& bnd = out.back();
						bnd.setLeftValue(lv);
						bnd.setRightValue(rv);
						start(ed);
						rewind();
						do {
							bnd.push_back(m_cur);
							EdgelGraphSet(eGr, m_cur);
						} while (step());
					}
				}
			}
		}
		// Vertical scan
		for (i = i0; i < i1; ++i) {
			for (j = 0; j < m_nl; ++j) {
				// left/right value
				double lv = m_extern_value;
				if (i < m_nc)
					lv = m_im->get(ImageSite(0, i, j));
				double rv = m_extern_value;
				if (i > 0)
					rv = m_im->get(ImageSite(0, i - 1, j));
				if (lv != rv) {
					Edgel ed(IPoint2D(i, j), Edgel::S);
					// Left value is the smallest
					if (rv < lv) {
						double temp = lv;
						lv = rv;
						rv = temp;
						ed.reverse();
					}
					// If Edgel not seen
					if (EdgelGraphGet(eGr, ed) == 0) {
						// New boundary
						out.push_back(InterPixelBoundary());
						InterPixelBoundary& bnd = out.back();
						bnd.setLeftValue(lv);
						bnd.setRightValue(rv);
						start(ed);
						rewind();
						do {
							bnd.push_back(m_cur);
							EdgelGraphSet(eGr, m_cur);
						} while (step());
					}
				}
			}
		}

	}

} // EO namespace lgl
