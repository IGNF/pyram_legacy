#include "lgl/lglGeneral.h"

namespace lgl {
	bool lglLOG_enabled = true;

	void enable_lglLOG(bool enable) {
		lglLOG_enabled = enable;
	}

}