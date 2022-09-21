#include <ignImageWatershed.h>

using namespace ign::image;

namespace ign {

	namespace image {


		//////////////////////////////////////////////////////////////////////////////
		// Outils pour segmentation s'appuyant
		// sur des images de directions de Freeman (ou graphe de pixels)
		//////////////////////////////////////////////////////////////////////////////
		const int V8Cx[9] = { 1, 1, 0, -1, -1, -1, 0, 1, 0 };
		const int V8Cy[9] = { 0, 1, 1, 1, 0, -1, -1, -1, 0 };
		const unsigned char bmask[9] = { 1, 2, 4, 8, 16, 32, 64, 128, 0 };

		void pxlgraph_symetrise(BufferImage<unsigned char>& im) {
			unsigned char bitm, pxl, pxl1;
			int i = 0, j;

			int nc = static_cast<int>(im.numCols());
			int nl = static_cast<int>(im.numLines());
			for (j = 1; j < nl; j++) {
				// Corps de l'image
				for (i = 1; i < nc - 1; i++) {
					pxl = im(i, j);
					// NE
					bitm = (bmask[7] & pxl) | ((bmask[3] & im(i + 1, j - 1)) << 4);
					pxl1 = bitm;
					bitm = bitm >> 4;
					im(i + 1, j - 1) = im(i + 1, j - 1) | bitm;
					// N
					bitm = (bmask[6] & pxl) | ((bmask[2] & im(i, j - 1)) << 4);
					pxl1 = pxl1 | bitm;
					bitm = bitm >> 4;
					im(i, j - 1) = im(i, j - 1) | bitm;
					// NO
					bitm = (bmask[5] & pxl) | ((bmask[1] & im(i - 1, j - 1)) << 4);
					pxl1 = pxl1 | bitm;
					bitm = bitm >> 4;
					im(i - 1, j - 1) = im(i - 1, j - 1) | bitm;
					// O
					bitm = (bmask[4] & pxl) | ((bmask[0] & im(i - 1, j)) << 4);
					im(i, j) = im(i, j) | pxl1 | bitm;
					bitm = bitm >> 4;
					im(i - 1, j) = im(i - 1, j) | bitm;
				}
			}

			if (nc > 1)
				for (j = 1; j < nl; j++) {
					// 1ere colonne
					pxl = im(0, j);
					// NE
					bitm = (bmask[7] & pxl) | ((bmask[3] & im(1, j - 1)) << 4);
					pxl1 = bitm;
					bitm = bitm >> 4;
					im(1, j - 1) = im(1, j - 1) | bitm;
					// N
					bitm = (bmask[6] & pxl) | ((bmask[2] & im(0, j - 1)) << 4);
					im(0, j) = im(0, j) | pxl1;
					bitm = bitm >> 4;
					im(0, j - 1) = im(0, j - 1) | bitm;
					// derniere colonne
					pxl = im(nc - 1, j);
					// N
					bitm = (bmask[6] & pxl) | ((bmask[2] & im(nc - 1, j - 1)) << 4);
					pxl1 = bitm;
					bitm = bitm >> 4;
					im(nc - 1, j - 1) = im(nc - 1, j - 1) | bitm;
					// NO
					bitm = (bmask[5] & pxl) | ((bmask[1] & im(nc - 2, j - 1)) << 4);
					pxl1 = pxl1 | bitm;
					bitm = bitm >> 4;
					im(nc - 2, j - 1) = im(nc - 2, j - 1) | bitm;
					// O
					bitm = (bmask[4] & pxl) | ((bmask[0] & im(nc - 2, j)) << 4);
					im(i, j) = im(i, j) | pxl1 | bitm;
					bitm = bitm >> 4;
					im(nc - 2, j) = im(nc - 2, j) | bitm;
				}
			// 1ere ligne
			if (nl)
				for (i = 1; i < nc; i++) {
					pxl = im(i, 0);
					// O
					bitm = (bmask[4] & pxl) | ((bmask[0] & im(i - 1, 0)) << 4);
					im(i, 0) = im(i, 0) | bitm;
					bitm = bitm >> 4;
					im(i - 1, 0) = im(i - 1, 0) | bitm;
				}
		}

		void pxlgraph_connected_components(const BufferImage<unsigned char> &im, BufferImage<int> &ims, int &nbreg) {
			size_t nc, nl;
			nc = im.numCols();
			nl = im.numLines();
			size_t i;
			unsigned char pxl;
			ims.initialize(static_cast<int>(nc), static_cast<int>(nl), 1, -1);

			std::vector<std::pair<int, int> > tas;
			std::pair<int, int> p, pv;

			unsigned int cg = 0, lg = 0;
			unsigned int regno = 0;
			while (lg < nl) {
				// recherche du prochain germe
				while ((lg < nl) && (ims(cg, lg) != -1)) {
					cg++;
					if (cg >= nc) {
						lg++;
						cg = 0;
					}
				}
				if (lg < nl) {
					// germe dans le tas
					p.first = cg;
					p.second = lg;
					tas.push_back(p);
					// croissance
					while (tas.size() > 0) {
						p = tas.back();
						tas.pop_back();
						ims(p.first, p.second) = regno;
						pxl = im(p.first, p.second);
						for (i = 0; i < 8; i++) {
							if (bmask[i] & pxl) {
								pv.first = p.first + V8Cx[i];
								pv.second = p.second + V8Cy[i];

								if (im.isIn(pv.first, pv.second, 0))
									if (ims(pv.first, pv.second) == -1)
										tas.push_back(pv);
							}
						}
					}
					regno++;
				}
			}
			nbreg = regno;
		}

		double round(double x) {
			return floor(x + 0.5);
		}

		int intRound(double x) {
			return int(round(x));
		}

		int _value_8v_(const BufferImage<float>& im, int i, int j, int k, float precision = 10000) {
			float v = im(i + V8Cx[k], j + V8Cy[k]);
			int iv = intRound(v * precision);
			return (iv * 10 + k);
		}

		void Watershed(const BufferImage<float> &im, BufferImage<unsigned char> &imdir, BufferImage<int> &bassins, int &nbbass, float p) {

			imdir.initialize(static_cast<int>(im.numCols()), static_cast<int>(im.numLines()), static_cast<int>(im.numBands()), 0);
			bassins.initialize(static_cast<int>(im.numCols()), static_cast<int>(im.numLines()), static_cast<int>(im.numBands()), 0);

			size_t nl = im.numLines();
			size_t nc = im.numCols();
			size_t i, j, k;
			int min;
			int imin;
			int val;

			// directions de plus grande pente
			for (i = 1; i < nc - 1; i++) {
				// 1ere ligne
				min = static_cast<int>(_value_8v_(im, i, 0, 8, p));
				imin = 8;
				for (k = 0; k < 5; k++) {
					val = _value_8v_(im, i, 0, k, p);
					if (val < min) {
						min = val;
						imin = static_cast<int>(k);
					}
				}
				imdir(i, 0) += static_cast<int>(bmask[imin]);

				// corps
				for (j = 1; j < nl - 1; j++) {
					min = static_cast<int>(_value_8v_(im, i, j, 8, p));
					imin = 8;
					for (k = 0; k < 8; k++) {
						val = static_cast<int>(_value_8v_(im, i, j, k, p));
						if (val < min) {
							min = val;
							imin = k;
						}
					}
					imdir(i, j) += static_cast<int>(bmask[imin]);
				}

				// derniere ligne
				min = static_cast<int>(_value_8v_(im, i, nl - 1, 8, p)); //(i+1,nl-1);
				imin = 8;
				val = static_cast<int>(_value_8v_(im, i, nl - 1, 0, p));
				if (val < min) {
					min = val;
					imin = 0;
				}
				for (k = 4; k < 8; k++) {
					val = static_cast<int>(_value_8v_(im, i, nl - 1, k, p));
					if (val < min) {
						min = val;
						imin = static_cast<int>(k);
					}
				}
				imdir(i, nl - 1) += static_cast<int>(bmask[imin]);
			}

			// 1ere et derniere colonne
			for (j = 1; j < nl - 1; j++) {
				// 1ere colonne
				min = static_cast<int>(_value_8v_(im, 0, j, 8, p));
				imin = 8;
				for (k = 0; k < 2; k++) {
					val = static_cast<int>(_value_8v_(im, 0, j, k, p));
					if (val < min) {
						min = val;
						imin = static_cast<int>(k);
					}
				}
				for (k = 6; k < 8; k++) {
					val = static_cast<int>(_value_8v_(im, 0, j, k, p));
					if (val < min) {
						min = val;
						imin = static_cast<int>(k);
					}
				}
				imdir(0, j) += bmask[imin];

				// derniere colonne
				min = static_cast<int>(_value_8v_(im, nc - 1, j, 8, p));
				imin = 8;
				for (k = 2; k < 7; k++) {
					val = _value_8v_(im, nc - 1, j, k, p);
					if (val < min) {
						min = val;
						imin = static_cast<int>(k);
					}
				}
				imdir(nc - 1, j) += bmask[imin];
			}

			// 4 coins
			// (0,0)
			min = _value_8v_(im, 0, 0, 8, p);
			imin = 8;
			for (k = 0; k < 3; k++) {
				val = _value_8v_(im, 0, 0, k, p);
				if (val < min) {
					min = val;
					imin = static_cast<int>(k);
				}
			}
			imdir(0, 0) += bmask[imin];
			// (nc-1,0)
			min = static_cast<int>(_value_8v_(im, nc - 1, 0, 8, p));
			imin = 8;
			for (k = 2; k < 5; k++) {
				val = static_cast<int>(_value_8v_(im, nc - 1, 0, k, p));
				if (val < min) {
					min = val;
					imin = static_cast<int>(k);
				}
			}
			imdir(nc - 1, 0) += static_cast<int>(bmask[imin]);
			// (0,nl-1)
			min = static_cast<int>(_value_8v_(im, 0, nl - 1, 8, p));
			imin = 8;
			val = static_cast<int>(_value_8v_(im, 0, nl - 1, 0, p));
			if (val < min) {
				min = val;
				imin = 0;
			}
			for (k = 6; k < 8; k++) {
				val = static_cast<int>(_value_8v_(im, 0, nl - 1, k, p));
				if (val < min) {
					min = val;
					imin = k;
				}
			}
			imdir(0, nl - 1) += bmask[imin];
			// (nc-1,nl-1)
			min = static_cast<int>(_value_8v_(im, nc - 1, nl - 1, 8, p));
			imin = 8;
			for (k = 4; k < 7; k++) {
				val = static_cast<int>(_value_8v_(im, nc - 1, nl - 1, k, p));
				if (val < min) {
					min = val;
					imin = static_cast<int>(k);
				}
			}
			imdir(nc - 1, nl - 1) += static_cast<int>(bmask[imin]);

			// Symetrisation
			pxlgraph_symetrise(imdir);
			pxlgraph_connected_components(imdir, bassins, nbbass);
		}

	}
}
