#include <limits>
#include "lgl/lglImageManip.h"

#if wxUSE_GUI
#include <wx/log.h>
#endif

namespace lgl {

	//========================================================================================
	template<class T>
	void TcomputeMinMax(T, const ImageRam& rin, double& min, double& max) {
		// downcast
		const ImageRamT<T>* in = dynamic_cast<const ImageRamT<T>*>(&rin);
		if (!in)
			return;

		min = (std::numeric_limits<double>::max)();
		max = (std::numeric_limits<double>::min)();

		typename ImageRamT<T>::const_iterator it;
		for (it = in->begin(); it != in->end(); ++it) {
			if (*it < min)
				min = *it;
			if (*it > max)
				max = *it;
		}

	}
	//========================================================================================

	//========================================================================================
	void computeMinMax(const ImageRam& in, double& min, double& max) {
		lglSwitchOnTypeFunctionCall4(in.type(), TcomputeMinMax, in, min, max);
	}
	//========================================================================================

	//========================================================================================
	template<class T>
	void TaverageChannels(T, const ImageRam& rin, pImageRam& rout) {
		// downcast
		const ImageRamT<T>* in = dynamic_cast<const ImageRamT<T>*>(&rin);
		if (!in)
			return;

		ImageRamT<T>* out(0);
		if (!rout)
			out = new ImageRamT<T>;
		else {
			out = dynamic_cast<ImageRamT<T>*>(rout);
			if (!out)
				out = new ImageRamT<T>;
		}
		rout = out;
		ImageSite s(in->size());
		int nc = s(0);
		s(0) = 1;
		out->resize(s);

		typename ImageRamT<T>::const_iterator iit;
		typename ImageRamT<T>::iterator oit;
		for (iit = in->begin(), oit = out->begin(); oit != out->end(); ++oit) {
			double a = 0;
			for (int c = 0; c < nc; ++c, ++iit) {
				a += *iit;
			}
			*oit = (T) (a / (double) nc);
		}

	}

	//========================================================================================
	/// \brief Averages the channels of in producing a monochannel image out (out can be in)
	void averageChannels(const ImageRam& in, pImageRam& out) {
		lglSwitchOnTypeFunctionCall3(in.type(), TaverageChannels, in, out);
	}
	//========================================================================================

	//========================================================================================
	template<class T>
	void TverticalFlip(T, const ImageRam& rin, pImageRam& rout) {
		// downcast
		const ImageRamT<T>* in = dynamic_cast<const ImageRamT<T>*>(&rin);
		if (!in)
			return;

		ImageRamT<T>* out = dynamic_cast<ImageRamT<T>*>(rout);
		if (!out)
			out = new ImageRamT<T>;
		rout = out;
		out->resize(in->size());

		long line_size = in->size(0) * in->size(1);
		const T* pi = &((*in)(0, 0, 0));
		T* po = &((*out)(0, 0, in->size(2) - 1));
		for (int i = 0; i < in->size(2); i++) {
			memcpy(po, pi, line_size * sizeof(T));
			pi += line_size;
			po -= line_size;
		}
	}
	//========================================================================================

	//========================================================================================
	/// \brief Vertically flips the image in (out cannot be in !)
	void verticalFlip(const ImageRam& in, pImageRam& out) {
		lglSwitchOnTypeFunctionCall3(in.type(), TverticalFlip, in, out);
	}
	//========================================================================================

	//========================================================================================
	//========================================================================================
	//========================================================================================
	/// \brief Produces an image out whose values are the average values of in on the regions of same value of lab.
	///
	/// out can be in.
	template<class T, class L>
	void TLaverageOnRegions(L, T, const ImageRam& rin, const ImageRam& rlab, pImageRam& rout) {
		// downcast
		const ImageRamT<T>* in = dynamic_cast<const ImageRamT<T>*>(&rin);
		if (!in)
			return;
		const ImageRamT<L>* lab = dynamic_cast<const ImageRamT<L>*>(&rlab);
		if (!lab)
			return;

		if (!rout)
			rout = new ImageRamT<T>;
		rout->resize(in->size());
		ImageRamT<T>* out = dynamic_cast<ImageRamT<T>*>(rout);

		//
		int nc = in->size(0);
		int maxl = 0;
		typename ImageRamT<L>::const_iterator lit;
		for (lit = lab->begin(); lit != lab->end(); ++lit) {
			if ((*lit) > (L) maxl)
				maxl = (int) *lit;
		}
		maxl++;
		int* card = new int[maxl];
		memset(card, 0, maxl * sizeof(int));
		double* som = new double[maxl * nc];
		memset(som, 0, maxl * nc * sizeof(double));

		typename ImageRamT<T>::const_iterator iit = in->begin();
		for (lit = lab->begin(); lit != lab->end(); ++lit) {
			int l = (int) *lit;
			card[l]++;
			for (int c = 0; c < nc; ++c) {
				som[l * nc + c] += (double) *iit;
				++iit;
			}
		}

		out->resize(in->size());
		typename ImageRamT<T>::iterator oit = out->begin();
		for (lit = lab->begin(); lit != lab->end(); ++lit) {
			int l = (int) *lit;
			for (int c = 0; c < nc; ++c) {
				*oit = (T) (som[l * nc + c] / card[l]);
				++oit;
			}
		}

		delete[] card;
		delete[] som;
	}
	//========================================================================================
	template<class T>
	void TaverageOnRegions(T, const ImageRam& in, const ImageRam& lab, pImageRam& out) {
		lglSwitchOnTypeFunctionCall5(lab.type(), TLaverageOnRegions, (T )(0), in, lab, out);
	}
	//========================================================================================
	void averageOnRegions(const ImageRam& in, const ImageRam& lab, pImageRam& out) {
		lglSwitchOnTypeFunctionCall4(in.type(), TaverageOnRegions, in, lab, out);
	}
	//========================================================================================
	//========================================================================================
	//========================================================================================

	//========================================================================================
	/// \brief Subsamples the image in by a factor f. Subsampling is made by bloc average.
	template<class T>
	void Tsubsample(T, const ImageRam& rin, int f, pImageRam& rout) {
		// downcast
		const ImageRamT<T>* in = dynamic_cast<const ImageRamT<T>*>(&rin);
		if (!in)
			return;

		if (!rout)
			rout = new ImageRamT<T>;
		ImageRamT<T>* out = dynamic_cast<ImageRamT<T>*>(rout);
		if (!out) {
			lglERROR("subsample : incompatible in/out image types");
		}

		ImageSite s(in->size());
		s(1) /= f;
		s(2) /= f;
		s(3) /= f;
		rout->resize(s);

		ImageRamT<double> tmp(s);
		tmp.mset(0);
		int i, j, k, ii, jj, kk;
		int zs = s(3);
		if (zs == 0)
			zs++;
		for (k = 0; k < zs; ++k) {
			for (kk = 0; kk < f; kk++) {
				for (j = 0; j < s(2); ++j) {
					for (jj = 0; jj < f; ++jj) {
						double* pt = &(tmp(0, 0, j, k));
						const T* pi = &((*in)(0, 0, j * f + jj, k * f + kk));
						for (i = 0; i < s(1); ++i) {
							for (ii = 0; ii < f; ++ii) {
								*pt += *pi;
								++pi;
							}
							++pt;
						}
					}
				}
			}
		}
		//
		double nbs = pow((double) f, (int) in->dimension());
		double* pt = &(tmp(0, 0, 0, 0));
		T* po = &((*out)(0, 0, 0, 0));
		for (i = 0; i < out->numberOfSites(); ++i) {
			*po = (T) ((*pt) / nbs);
			++po;
			++pt;
		}
	}
	//========================================================================================
	/// \brief Subsamples the image in by a factor f. Subsampling is made by bloc average.
	void subsample(const ImageRam& in, int f, pImageRam& out) {
		lglSwitchOnTypeFunctionCall4(in.type(), Tsubsample, in, f, out);
	}
//========================================================================================

#if wxUSE_GUI

	//========================================================================================
	/// \brief Converts an ImageRGB to a ImageRamT<unsigned char>.
	void ImageRGBToImageRam ( const ImageRGB& in, ImageRamT<unsigned char>& out )
	{
		out.resize( in.size() );
		memcpy(out.getData(),in.getData(),in.size(1)*in.size(2)*3);
	}
	//========================================================================================

	//======================================================================================
	BOOL ImageToImageRGB ( const ImageIn& in,
			ImageRGB& out,
			Palette& pal)
	{
		ImageSite s = in.size();
		if (s.dimension()!=3) return FALSE;
		s(0) = 3;
		if (!out.resize(s)) return FALSE;

		std::vector<double> v(in.size(0));
		ImageSite p(0,0,0);
		ImageRGB::iterator j = out.begin();
		for (p(2)=0;p(2)<s(2);p(2)++) {
			for (p(1)=0;p(1)<s(1);p(1)++) {
				for (p(0)=0;p(0)<in.size(0);p(0)++) {
					v[p(0)] = in.get(p);
				}
				RGBpoint p = pal(v);
				(*j) = p.r; ++j;
				(*j) = p.g; ++j;
				(*j) = p.b; ++j;
			}
		}
		return TRUE;
	}
	//======================================================================================

	//========================================================================================
	/// \brief Draws the contours (change of value) of in with the color color.
	///
	/// \param zoom gives the scaling factor
	/// \note If out is already at the good size then its contour-free values are kept.
	///			Usefull to superimpose the contours on a background image.
	template <class L>
	void TdrawContours( L, const ImageRam& rlab, RGBpoint color, int zoom, ImageRGB& out )
	{
		// dynamic downcast of input image
		const ImageRamT<L>* lab = dynamic_cast<const ImageRamT<L>*>(&rlab);
		if (!lab) return;

		// tests that in is a 2D image
		if (lab->size().dimension()!=3) return;
		// output dimensions
		ImageSite s(3, zoom*lab->size(1), zoom*lab->size(2) );

		//wxLogMessage(wxT("drawContours : %d*%d vs %d*%d"),s(1),s(2),out.size(1),out.size(2));
		wxLogMessage(wxT("drawContours : %d"),zoom);
		if (!out.resize(s)) return;
		//
		long line_size = 3*out.size(1);
		unsigned char** po = new unsigned char*[zoom];

		//
		int i,j,z;
		for (j=0;j<lab->size(2)-1;j++) {
			po[0] = &out(0,zoom-1,j*zoom);
			for (z=1;z<zoom;++z) po[z] = po[z-1] + line_size;
			unsigned char* llo = po[zoom-1] - 3*(zoom-1);
			const L* pl0 = &((*lab)(0,0,j));
			const L* pl1 = pl0+1;
			const L* pl2 = &((*lab)(0,0,j+1));
			for (i=0;i<lab->size(1)-1;i++) {
				if ( (*pl0)!=(*pl1) ) {
					for (z=0;z<zoom;++z) {
						*(po[z]) = color.r; (po[z])++;
						*(po[z]) = color.g; (po[z])++;
						*(po[z]) = color.b; (po[z])++;
						po[z] += 3*(zoom-1);
					}
				}
				else {
					for (z=0;z<zoom;++z) po[z] +=3*zoom;
				}
				if ( (*pl0) != (*pl2) ) {
					for (z=0;z<zoom;++z) {
						*llo = color.r; llo++;
						*llo = color.g; llo++;
						*llo = color.b; llo++;
					}
				}
				else {
					for (z=0;z<zoom;++z) llo +=3;
				}
				++pl0;
				++pl1;
				++pl2;
			}
			// last pixel of the line
			if ((*pl0) != (*pl2)) {
				for (z=0;z<zoom;++z) {
					*llo = color.r; llo++;
					*llo = color.g; llo++;
					*llo = color.b; llo++;
				}
			}
		}
		// last line
		po[0] = &out(0,zoom-1,j*zoom);
		for (z=1;z<zoom;++z) po[z] = po[z-1] + line_size;
		const L* pl0 = &((*lab)(0,0,j));
		const L* pl1 = pl0+1;
		for (i=0;i<lab->size(1)-1;i++) {
			if ( (*pl0)!=(*pl1) ) {
				for (z=0;z<zoom;++z) {
					*(po[z]) = color.r; (po[z])++;
					*(po[z]) = color.g; (po[z])++;
					*(po[z]) = color.b; (po[z])++;
					po[z] += 3*(zoom-1);
				}
			}
			else {
				for (z=0;z<zoom;++z) po[z] +=3*zoom;
			}
			++pl0;
			++pl1;
		}
		delete[] po;
	}
	//========================================================================================
	void drawContours( const ImageRam& in, RGBpoint color, int zoom, ImageRGB& out ) {
		lglSwitchOnTypeFunctionCall5 (in.type(), TdrawContours, in, color, zoom, out)
	}

	/*
		//======================================================================================
		/// Same as ImageToImageRGB but overlays the contours of a label image
		BOOL MonoChannelImageAndLabelsToImageRGB ( const ImageIn& in,
		const ImageIn& lab,
		ImageRGB& out,  
		Palette& pal,
		RGBpoint contours_color,
		FPoint map)
		{
		// test that in is a 2D image
		if (in.size().dimension()!=3) return FALSE;
		// test that the map is correct
		if ( ! (
		( (map(0)!=0) ^ (map(1)!=0) ) &&
		( (map(2)!=0) ^ (map(3)!=0) )
		)
		) return FALSE;
		// output dimensions
		ImageSite s(3, abs(in.size(1)*map(0)+in.size(2)*map(1)) , abs(in.size(1)*map(2)+in.size(2)*map(3)) );
		if (!out.resize(s)) return FALSE;

		// inverse mapping
		FPoint imap(0,0,0,0);
		// axis inversions
		double xoff = 0, yoff = 0;
		if (map(0)!=0) {
		imap(0) = 1./map(0);
		if (map(0)<0) xoff = in.size(1)+imap(0);
		}
		else { // map(1)!=0
		imap(1) = 1./map(1);
		if (map(1)<0) xoff = in.size(2)+imap(1);
		}
		if (map(2)!=0) {
		imap(2) = 1./map(2);
		if (map(2)<0) yoff = in.size(1)+imap(2);
		}
		else { // map(3)!=0
		imap(3) = 1./map(3);
		if (map(3)<0) yoff = in.size(2)+imap(3);
		}
		//
		std::vector<double> v(in.size(0));
		ImageSite pt(0,0,0),qt(0,0,0);
		ImageRGB::iterator o = out.begin();
		double i,j;
		for (j=0;j<s(2);j+=1) {
		for (i=0;i<s(1);i+=1) {
		// inverse mapping
		pt(0) = 0;
		pt(1) = floor(xoff + imap(0)*i + imap(1)*j);
		pt(2) = floor(yoff + imap(2)*i + imap(3)*j);
		double lab0 = lab.get(pt);
		// Is it a contour point ?
		BOOL contour = false;

		// Horizontal contour
		if (i>0) {
		// q is Site i-1,j
		qt(1) = floor(xoff + imap(0)*(i-1) + imap(1)*j );
		qt(2) = floor(yoff + imap(2)*(i-1) + imap(3)*j );
		double lab1 = lab.get(qt);
		if (lab1!=lab0) {
		// horizontal contour
		(*o) = contours_color.r; ++o;
		(*o) = contours_color.g; ++o;
		(*o) = contours_color.b; ++o;
		contour = true;
		}
		}
		if ((!contour)&&(j>0)) {
		// q is Site i,j-1
		qt(1) = floor(xoff + imap(0)*i + imap(1)*(j-1));
		qt(2) = floor(yoff + imap(2)*i + imap(3)*(j-1));
		double lab1 = lab.get(qt);
		if (lab1!=lab0) {
		// vertical contour
		(*o) = contours_color.r; ++o;
		(*o) = contours_color.g; ++o;
		(*o) = contours_color.b; ++o;
		contour = true;
		}
		}
		if ((!contour)&&(i>0)&&(j>0)) {
		// q is Site i-1,j-1
		qt(1) = floor(xoff + imap(0)*(i-1) + imap(1)*(j-1));
		qt(2) = floor(yoff + imap(2)*(i-1) + imap(3)*(j-1));
		double lab1 = lab.get(qt);
		if (lab1!=lab0) {
		// vertical contour
		(*o) = contours_color.r; ++o;
		(*o) = contours_color.g; ++o;
		(*o) = contours_color.b; ++o;
		contour = true;
		}
		}
		if (!contour) {
		RGBpoint c = pal(in.get(pt));
		(*o) = c.r; ++o;
		(*o) = c.g; ++o;
		(*o) = c.b; ++o;
		}
		}
		}
		//wxLogMessage("ok");

		return TRUE;
		}
		//======================================================================================

		*/

	//======================================================================================
	template <class T>
	void TImageToImageRGB ( T, const ImageRam& rin,
			ImageRGB& out,
			const Palette& pal,
			int zoom)
	{
		// dynamic downcast
		const ImageRamT<T>* in = dynamic_cast<const ImageRamT<T>*>(&rin);
		if (!in) return;

		// test that in is a 2D image
		if (in->dimension()!=2) return;
		// output dimensions
		ImageSite s(3, zoom*in->size(1), zoom*in->size(2) );
		if (!out.resize(s)) return;

		//
		long line_size = 3*out.size(1);
		unsigned char* po;
		int i,j,z,c;
		for (j=0;j<in->size(2);j++) {
			const T* pi = &((*in)(0,0,j));
			po = &out(0,0,j*zoom);
			unsigned char* co = po;
			for (i=0;i<in->size(1);i++) {
				// builds the image values vector
				std::vector<double> vec(in->size(0));
				for (c=0;c<in->size(0);++c) {vec[c] = *pi; pi++;}
				// RGB mapping
				RGBpoint col = pal(vec);
				// output
				for (z=0;z<zoom;++z) {
					(*co) = col.r; ++co;
					(*co) = col.g; ++co;
					(*co) = col.b; ++co;
				}
			}
			// line copy
			unsigned char* po2 = po + line_size;
			for (z=0;z<zoom-1;++z) {
				memcpy(po2,po,line_size);
				po2 += line_size;
			}
		}
	}
	//======================================================================================

	//======================================================================================
	void ImageToImageRGB ( const ImageRam& in,
			ImageRGB& out,
			const Palette& pal,
			int zoom)
	{
		lglSwitchOnTypeFunctionCall5( in.type(), TImageToImageRGB, in, out, pal, zoom );
	}
	//======================================================================================

	//======================================================================================
	/// Same as ImageToImageRGB but overlays the contours of a label image
	template <class T, class L>
	void TLImageAndLabelsToImageRGB ( T, L, const ImageRam& rin,
			const ImageRam& rlab,
			ImageRGB& out,
			const Palette& pal,
			RGBpoint contours_color,
			int zoom)
	{
		// dynamic downcast
		const ImageRamT<T>* in = dynamic_cast<const ImageRamT<T>*>(&rin);
		const ImageRamT<L>* lab = dynamic_cast<const ImageRamT<L>*>(&rlab);

		// test that in is a 2D image
		if (in->dimension()!=2) return;
		// output dimensions
		ImageSite s(3, zoom*in->size(1), zoom*in->size(2) );
		if (!out.resize(s)) return;

		//
		long line_size = 3*out.size(1);
		unsigned char* po;
		int i,j,z,c;
		for (j=0;j<in->size(2)-1;j++) {
			po = &out(0,0,j*zoom);
			unsigned char* co = po;
			unsigned char* llo = co + (zoom-1)*line_size;
			const T* pi = &((*in)(0,0,j));
			const L* pl0 = &((*lab)(0,0,j));
			const L* pl1 = pl0+1;
			const L* pl2 = &((*lab)(0,0,j+1));
			for (i=0;i<in->size(1)-1;i++) {
				// builds the image values vector
				std::vector<double> vec(in->size(0));
				for (c=0;c<in->size(0);++c) {vec[c] = *pi; pi++;}
				// RGB mapping
				RGBpoint col = pal(vec);

				if ((*pl0)!=(*pl2)) {
					for (z=0;z<zoom-1;++z) {
						(*llo) = contours_color.r; (*co) = col.r; ++co; ++llo;
						(*llo) = contours_color.g; (*co) = col.g; ++co; ++llo;
						(*llo) = contours_color.b; (*co) = col.b; ++co; ++llo;
					}
				}
				else {
					for (z=0;z<zoom-1;++z) {
						(*llo) = (*co) = col.r; ++co; ++llo;
						(*llo) = (*co) = col.g; ++co; ++llo;
						(*llo) = (*co) = col.b; ++co; ++llo;
					}
				}
				// last point of the zoomed pixel
				RGBpoint llcol = col;
				if ( ((*pl0) != (*pl2)) || ((*pl0) != (*pl1)) ) llcol = contours_color;
				if ((*pl0) != (*pl1)) col = contours_color;
				(*llo) = llcol.r; (*co) = col.r; ++co; ++llo;
				(*llo) = llcol.g; (*co) = col.g; ++co; ++llo;
				(*llo) = llcol.b; (*co) = col.b; ++co; ++llo;
				++pl0;
				++pl1;
				++pl2;
			}
			// last pixel of the line
			// builds the image values vector
			std::vector<double> vec(in->size(0));
			for (c=0;c<in->size(0);++c) {vec[c] = *pi; pi++;}
			// RGB mapping
			RGBpoint col = pal(vec);

			RGBpoint llcol = col;
			if ((*pl0) != (*pl2)) llcol = contours_color;
			for (z=0;z<zoom;++z) {
				(*llo) = llcol.r; (*co) = col.r; ++co; ++llo;
				(*llo) = llcol.g; (*co) = col.g; ++co; ++llo;
				(*llo) = llcol.b; (*co) = col.b; ++co; ++llo;
			}
			// line copy
			unsigned char* po2 = po + line_size;
			for (z=0;z<zoom-2;++z) {
				memcpy(po2,po,line_size);
				po2 += line_size;
			}
		}
		// last line
		const T* pi = &((*in)(0,0,j));
		po = &out(0,0,j*zoom);
		const L* pl0 = &((*lab)(0,0,j));
		const L* pl1 = pl0+1;
		unsigned char* co = po;
		for (i=0;i<in->size(1)-1;i++) {
			// builds the image values vector
			std::vector<double> vec(in->size(0));
			for (c=0;c<in->size(0);++c) {vec[c] = *pi; pi++;}
			// RGB mapping
			RGBpoint col = pal(vec);

			for (z=0;z<zoom-1;++z) {
				(*co) = col.r; ++co;
				(*co) = col.g; ++co;
				(*co) = col.b; ++co;
			}
			// last point of the zoomed pxl
			if ((*pl0) != (*pl1)) col = contours_color;
			(*co) = col.r; ++co;
			(*co) = col.g; ++co;
			(*co) = col.b; ++co;
			++pl0;
			++pl1;
		}
		// last pixel of the line
		// builds the image values vector
		std::vector<double> vec(in->size(0));
		for (c=0;c<in->size(0);++c) {vec[c] = *pi; pi++;}
		// RGB mapping
		RGBpoint col = pal(vec);

		for (z=0;z<zoom;++z) {
			(*co) = col.r; ++co;
			(*co) = col.g; ++co;
			(*co) = col.b; ++co;
		}

		// line copy
		unsigned char* po2 = po + line_size;
		for (z=0;z<zoom-1;++z) {
			memcpy(po2,po,line_size);
			po2 += line_size;
		}
	}
	//======================================================================================

	//======================================================================================
	/// Same as ImageToImageRGB but overlays the contours of a label image
	template <class L>
	void LImageAndLabelsToImageRGB ( L, const ImageRam& in,
			const ImageRam& lab,
			ImageRGB& out,
			const Palette& pal,
			RGBpoint contours_color,
			int zoom)
	{
		lglSwitchOnTypeFunctionCall8( in.type(), TLImageAndLabelsToImageRGB, (L)0, in, lab, out, pal, contours_color, zoom );
	}
	//======================================================================================
	/// Same as ImageToImageRGB but overlays the contours of a label image
	void ImageAndLabelsToImageRGB ( const ImageRam& in,
			const ImageRam& lab,
			ImageRGB& out,
			const Palette& pal,
			RGBpoint contours_color,
			int zoom)
	{
		lglSwitchOnTypeFunctionCall7( lab.type(), LImageAndLabelsToImageRGB, in, lab, out, pal, contours_color, zoom );
	}
	//======================================================================================

	//======================================================================================
	template <class T>
	void TMonoChannelImageToImageRGB ( T, const ImageRam& rin,
			ImageRGB& out,
			const Palette& pal,
			int zoom)
	{
		// dynamic downcast
		const ImageRamT<T>* in = dynamic_cast<const ImageRamT<T>*>(&rin);

		// test that in is a 2D image
		if (in->dimension()!=2) return;
		// output dimensions
		ImageSite s(3, zoom*in->size(1), zoom*in->size(2) );
		if (!out.resize(s)) return;

		//
		long line_size = 3*out.size(1);
		unsigned char* po;
		int i,j,z;
		for (j=0;j<in->size(2);j++) {
			const T* pi = &((*in)(0,0,j));
			po = &out(0,0,j*zoom);
			unsigned char* co = po;
			for (i=0;i<in->size(1);i++) {
				RGBpoint col = pal(*pi);
				for (z=0;z<zoom;++z) {
					(*co) = col.r; ++co;
					(*co) = col.g; ++co;
					(*co) = col.b; ++co;
				}
				++pi;
			}
			// line copy
			unsigned char* po2 = po + line_size;
			for (z=0;z<zoom-1;++z) {
				memcpy(po2,po,line_size);
				po2 += line_size;
			}
		}
	}
	//======================================================================================

	//======================================================================================
	/// Same as ImageToImageRGB but overlays the contours of a label image
	void MonoChannelImageToImageRGB ( const ImageRam& in,
			ImageRGB& out,
			const Palette& pal,
			int zoom)
	{
		lglSwitchOnTypeFunctionCall5( in.type(), TMonoChannelImageToImageRGB, in, out, pal, zoom );
	}
	//======================================================================================

	//======================================================================================
	/// Same as ImageToImageRGB but overlays the contours of a label image
	template <class T, class L>
	void TLMonoChannelImageAndLabelsToImageRGB ( T, L, const ImageRam& rin,
			const ImageRam& rlab,
			ImageRGB& out,
			const Palette& pal,
			RGBpoint contours_color,
			int zoom)
	{
		// dynamic downcast
		const ImageRamT<T>* in = dynamic_cast<const ImageRamT<T>*>(&rin);
		const ImageRamT<L>* lab = dynamic_cast<const ImageRamT<L>*>(&rlab);

		// test that in is a 2D image
		if (in->dimension()!=2) return;
		// output dimensions
		ImageSite s(3, zoom*in->size(1), zoom*in->size(2) );
		if (!out.resize(s)) return;

		//
		long line_size = 3*out.size(1);
		unsigned char* po;
		int i,j,z;
		for (j=0;j<in->size(2)-1;j++) {
			po = &out(0,0,j*zoom);
			unsigned char* co = po;
			unsigned char* llo = co + (zoom-1)*line_size;
			const T* pi = &((*in)(0,0,j));
			const L* pl0 = &((*lab)(0,0,j));
			const L* pl1 = pl0+1;
			const L* pl2 = &((*lab)(0,0,j+1));
			for (i=0;i<in->size(1)-1;i++) {
				RGBpoint col = pal(*pi);
				if ((*pl0)!=(*pl2)) {
					for (z=0;z<zoom-1;++z) {
						(*llo) = contours_color.r; (*co) = col.r; ++co; ++llo;
						(*llo) = contours_color.g; (*co) = col.g; ++co; ++llo;
						(*llo) = contours_color.b; (*co) = col.b; ++co; ++llo;
					}
				}
				else {
					for (z=0;z<zoom-1;++z) {
						(*llo) = (*co) = col.r; ++co; ++llo;
						(*llo) = (*co) = col.g; ++co; ++llo;
						(*llo) = (*co) = col.b; ++co; ++llo;
					}
				}
				// last point of the zoomed pixel
				RGBpoint llcol = col;
				if ( ((*pl0) != (*pl2)) || ((*pl0) != (*pl1)) ) llcol = contours_color;
				if ((*pl0) != (*pl1)) col = contours_color;
				(*llo) = llcol.r; (*co) = col.r; ++co; ++llo;
				(*llo) = llcol.g; (*co) = col.g; ++co; ++llo;
				(*llo) = llcol.b; (*co) = col.b; ++co; ++llo;
				++pi;
				++pl0;
				++pl1;
				++pl2;
			}
			// last pixel of the line
			RGBpoint col = pal(*pi);
			RGBpoint llcol = col;
			if ((*pl0) != (*pl2)) llcol = contours_color;
			for (z=0;z<zoom;++z) {
				(*llo) = llcol.r; (*co) = col.r; ++co; ++llo;
				(*llo) = llcol.g; (*co) = col.g; ++co; ++llo;
				(*llo) = llcol.b; (*co) = col.b; ++co; ++llo;
			}
			// line copy
			unsigned char* po2 = po + line_size;
			for (z=0;z<zoom-2;++z) {
				memcpy(po2,po,line_size);
				po2 += line_size;
			}
		}
		// last line
		const T* pi = &((*in)(0,0,j));
		po = &out(0,0,j*zoom);
		const L* pl0 = &((*lab)(0,0,j));
		const L* pl1 = pl0+1;
		unsigned char* co = po;
		for (i=0;i<in->size(1)-1;i++) {
			RGBpoint col = pal(*pi);
			for (z=0;z<zoom-1;++z) {
				(*co) = col.r; ++co;
				(*co) = col.g; ++co;
				(*co) = col.b; ++co;
			}
			// last point of the zoomed pxl
			if ((*pl0) != (*pl1)) col = contours_color;
			(*co) = col.r; ++co;
			(*co) = col.g; ++co;
			(*co) = col.b; ++co;
			++pi;
			++pl0;
			++pl1;
		}
		// last pixel of the line
		RGBpoint col = pal(*pi);
		for (z=0;z<zoom;++z) {
			(*co) = col.r; ++co;
			(*co) = col.g; ++co;
			(*co) = col.b; ++co;
		}

		// line copy
		unsigned char* po2 = po + line_size;
		for (z=0;z<zoom-1;++z) {
			memcpy(po2,po,line_size);
			po2 += line_size;
		}
	}
	//======================================================================================

	//======================================================================================
	/// Same as ImageToImageRGB but overlays the contours of a label image
	template <class L>
	void LMonoChannelImageAndLabelsToImageRGB ( L, const ImageRam& in,
			const ImageRam& lab,
			ImageRGB& out,
			const Palette& pal,
			RGBpoint contours_color,
			int zoom)
	{
		lglSwitchOnTypeFunctionCall8( in.type(), TLMonoChannelImageAndLabelsToImageRGB, (L)0, in, lab, out, pal, contours_color, zoom );
	}
	//======================================================================================
	/// Same as ImageToImageRGB but overlays the contours of a label image
	void MonoChannelImageAndLabelsToImageRGB ( const ImageRam& in,
			const ImageRam& lab,
			ImageRGB& out,
			const Palette& pal,
			RGBpoint contours_color,
			int zoom)
	{
		lglSwitchOnTypeFunctionCall7( lab.type(), LMonoChannelImageAndLabelsToImageRGB, in, lab, out, pal, contours_color, zoom );
	}
	//======================================================================================

	/*
		//======================================================================================
		BOOL ImageToImageRGB ( const ImageIn& in,
		ImageRGB& out,
		Palette& pal,
		FPoint map )
		{
		// test that in is a 2D image
		if (in.size().dimension()!=3) return FALSE;
		// test that the map is correct
		if ( ! (
		( (map(0)!=0) ^ (map(1)!=0) ) &&
		( (map(2)!=0) ^ (map(3)!=0) )
		)
		) return FALSE;
		// output dimensions
		ImageSite s(3, abs(in.size(1)*map(0)+in.size(2)*map(1)) , abs(in.size(1)*map(2)+in.size(2)*map(3)) );

		if (!out.resize(s)) return FALSE;

		// inverse mapping
		FPoint imap(0,0,0,0);
		// axis inversions
		int xoff = 0, yoff = 0;
		if (map(0)!=0) {
		imap(0) = 1./map(0);
		if (map(0)<0) xoff = in.size(1)-1;
		}
		else { // map(1)!=0
		imap(1) = 1./map(1);
		if (map(1)<0) xoff = in.size(2)-1;
		}
		if (map(2)!=0) {
		imap(2) = 1./map(2);
		if (map(2)<0) yoff = in.size(1)-1;
		}
		else { // map(3)!=0
		imap(3) = 1./map(3);
		if (map(3)<0) yoff = in.size(2)-1;
		}

		//wxLogMessage("transfo");
		//
		std::vector<double> v(in.size(0));
		ImageSite p(0,0,0);
		ImageRGB::iterator o = out.begin();
		double i,j;
		for (j=0;j<s(2);j++) {
		for (i=0;i<s(1);i++) {
		// inverse mapping
		p(1) = xoff + imap(0)*i + imap(1)*j;
		p(2) = yoff + imap(2)*i + imap(3)*j;
		// build the vector of values for the different channels
		for (p(0)=0;p(0)<in.size(0);p(0)++) {
		v[p(0)] = in.get(p);
		}
		RGBpoint c = pal(v);
		(*o) = c.r; ++o;
		(*o) = c.g; ++o;
		(*o) = c.b; ++o;
		}
		}
		//wxLogMessage("ok");

		return TRUE;
		}
		//======================================================================================




		//======================================================================================
		/// Same as ImageToImageRGB but overlays the contours of a label image
		BOOL ImageAndLabelsToImageRGB ( const ImageIn& in,
		const ImageIn& lab,
		ImageRGB& out,
		Palette& pal,
		RGBpoint contours_color,
		FPoint map)
		{
		//		if (in.size(0)==1) return MonoChannelImageAndLabelsToImageRGB(in,lab,out,pal,contours_color,map);
		// test that in is a 2D image
		if (in.dimension()!=2) return FALSE;
		// test that the map is correct
		if ( ! (
		( (map(0)!=0) ^ (map(1)!=0) ) &&
		( (map(2)!=0) ^ (map(3)!=0) )
		)
		) return FALSE;
		// output dimensions
		ImageSite s(3, abs(in.size(1)*map(0)+in.size(2)*map(1)) , abs(in.size(1)*map(2)+in.size(2)*map(3)) );

		if (!out.resize(s)) return FALSE;

		// inverse mapping
		FPoint imap(0,0,0,0);
		// axis inversions
		double xoff = 0, yoff = 0;
		if (map(0)!=0) {
		imap(0) = 1./map(0);
		if (map(0)<0) xoff = in.size(1)+imap(0);
		}
		else { // map(1)!=0
		imap(1) = 1./map(1);
		if (map(1)<0) xoff = in.size(2)+imap(1);
		}
		if (map(2)!=0) {
		imap(2) = 1./map(2);
		if (map(2)<0) yoff = in.size(1)+imap(2);
		}
		else { // map(3)!=0
		imap(3) = 1./map(3);
		if (map(3)<0) yoff = in.size(2)+imap(3);
		}
		//
		std::vector<double> v(in.size(0));
		ImageSite pt(0,0,0),qt(0,0,0);
		ImageRGB::iterator o = out.begin();
		double i,j;
		for (j=0;j<s(2);j+=1) {
		for (i=0;i<s(1);i+=1) {
		// inverse mapping
		pt(0) = 0;
		pt(1) = floor(xoff + imap(0)*i + imap(1)*j);
		pt(2) = floor(yoff + imap(2)*i + imap(3)*j);
		double lab0 = lab.get(pt);
		// Is it a contour point ?
		BOOL contour = false;
		// Horizontal contour
		if (i>0) {
		// q is Site i-1,j
		qt(1) = floor(xoff + imap(0)*(i-1) + imap(1)*j );
		qt(2) = floor(yoff + imap(2)*(i-1) + imap(3)*j );
		double lab1 = lab.get(qt);
		if (lab1!=lab0) {
		// horizontal contour
		(*o) = contours_color.r; ++o;
		(*o) = contours_color.g; ++o;
		(*o) = contours_color.b; ++o;
		contour = true;
		}
		}
		if ((!contour)&&(j>0)) {
		// q is Site i,j-1
		qt(1) = floor(xoff + imap(0)*i + imap(1)*(j-1));
		qt(2) = floor(yoff + imap(2)*i + imap(3)*(j-1));
		double lab1 = lab.get(qt);
		if (lab1!=lab0) {
		// vertical contour
		(*o) = contours_color.r; ++o;
		(*o) = contours_color.g; ++o;
		(*o) = contours_color.b; ++o;
		contour = true;
		}
		}
		if ((!contour)&&(i>0)&&(j>0)) {
		// q is Site i-1,j-1
		qt(1) = floor(xoff + imap(0)*(i-1) + imap(1)*(j-1));
		qt(2) = floor(yoff + imap(2)*(i-1) + imap(3)*(j-1));
		double lab1 = lab.get(qt);
		if (lab1!=lab0) {
		// vertical contour
		(*o) = contours_color.r; ++o;
		(*o) = contours_color.g; ++o;
		(*o) = contours_color.b; ++o;
		contour = true;
		}
		}
		if (!contour) {
		// build the vector of values for the different channels
		for (pt(0)=0;pt(0)<in.size(0);pt(0)++) {
		v[pt(0)] = in.get(pt);
		}
		RGBpoint c = pal(v);
		(*o) = c.r; ++o;
		(*o) = c.g; ++o;
		(*o) = c.b; ++o;
		}
		}
		}
		//wxLogMessage("ok");

		return TRUE;
		}

		//======================================================================================
		*/
#endif /// #if wxUSE_GUI

}
//============================================
// EO namespace lgl
//============================================
