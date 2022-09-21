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
 \brief Code of the methods of the class ImageScaleSets.
 */

#include <lgl/lglImageIO.h>
#include <lgl/lglImageManip.h>
#include <lgl/lglNoyau.h>
#include <lgl/lglString.h>
#include <sxs/sxsImageScaleSets.h>
#include <sxs/macroSortieMessage.h>

#include <ignBufferImage.h>
#include <ignFilesystemUtils.h>
#include <set>

// #include <wx/gdicmn.h>
// #include <wx/log.h>


namespace sxs {
	//======================================================
	/// Loads the ImageScaleSets from an ".iss" file
	lgl::BOOL ImageScaleSets::load(const lgl::Filename& filename) {
		bool verbose = 0;
		FLUSH;
		if (verbose)
			lglWARNING(std::string("ImageScaleSets::load 1 "));
		
		lgl::Filename s(filename);
#ifndef LGL_WITH_BOOST
		s.SetExt(wxT("iss"));
		// Reads the .iss
		#if wxUSE_UNICODE
							std::ifstream f(s.GetFullPath().ToAscii().data(),PARAM_IFSTREAM_TEXT);
		#else
							std::ifstream f(s.GetFullPath().c_str(), PARAM_IFSTREAM_TEXT);
		#endif //#if wxUSE_UNICODE
#else
		s.replace_extension(".iss");
		std::ifstream f(s.generic_string().c_str());
#endif

		if (verbose)
			lglWARNING(std::string("ImageScaleSets::load 2 "));
		if (!f.good())
			return false;
		if (verbose)
			lglWARNING(std::string("ImageScaleSets::load 3 "));

		char line[256];
		f.getline(line, 256, '\n');

		f.getline(line, 256, ':');
		f.getline(line, 256, '\n');
#ifndef LGL_WITH_BOOST
		lgl::Filename imagename(wxString(line, *wxConvCurrent));
		if (verbose)  wxLogWarning(_T("ImageScaleSets::load : fullname img '%s' "), imagename.GetFullName().c_str());
		imagename.MakeAbsolute(filename.GetPath());
		if (verbose)  wxLogWarning(_T("ImageScaleSets::load : fullname absolute img '%s' "), imagename.GetFullName().c_str());
#else
		lgl::Filename imagename(line);
		if (verbose) { 
			std::string msg = lgl::string_format(std::string("ImageScaleSets::load : fullname img '%s' "), imagename.generic_string().c_str());
			lglWARNING(msg);
		}
		imagename = filename.parent_path();
		imagename/=line;
		if (verbose) { 
			std::string msg = lgl::string_format(std::string("ImageScaleSets::load : fullname absolute img '%s' "), imagename.generic_string().c_str());
			lglWARNING(msg);
		}
#endif



		f.getline(line, 256, ':');
		f.getline(line, 256, '\n');
#ifndef LGL_WITH_BOOST
		lgl::Filename basename(wxString(line, *wxConvCurrent));
		if (verbose)   wxLogWarning(_T("ImageScaleSets::load : fullname base '%s' "), basename.GetFullName().c_str());
		basename.MakeAbsolute(filename.GetPath());
		if (verbose)   wxLogWarning(_T("ImageScaleSets::load : fullname absolute base '%s' "), basename.GetFullName().c_str());


#else
		lgl::Filename basename(line);
		if (verbose)  {
			std::string msg = lgl::string_format(std::string("ImageScaleSets::load : fullname base '%s' "), basename.generic_string().c_str());
			lglWARNING(msg);

		}
		basename = filename.parent_path();
		basename/=line;
		if (verbose)  {
			std::string msg = lgl::string_format(std::string("ImageScaleSets::load : fullname absolute base '%s' "), basename.generic_string().c_str());
			lglWARNING(msg);
		}
#endif


		f.getline(line, 256, ':');
		f.getline(line, 256, '\n');

#ifndef LGL_WITH_BOOST
		lgl::Filename sxsname(wxString(line, *wxConvCurrent));
		if (verbose) wxLogWarning(_T("ImageScaleSets::load 8 sxs '%s' "), sxsname.GetPath().c_str());
		sxsname.MakeAbsolute(filename.GetPath());
		if (verbose)  wxLogWarning(_T("ImageScaleSets::load : sxs '%s' "), sxsname.GetFullName().c_str());
#else
		lgl::Filename sxsname(line);
		if (verbose) {
			std::string msg = lgl::string_format(std::string("ImageScaleSets::load 8 sxs '%s' "), sxsname.parent_path().generic_string().c_str());
			lglWARNING(msg);

		}
		sxsname = filename.parent_path();
		sxsname /= line;
		if (verbose) { 
			std::string msg = lgl::string_format(std::string("ImageScaleSets::load : sxs '%s' "), sxsname.generic_string().c_str());
			lglWARNING(msg);		
		}
#endif


		//	lglLOG("image : "<<imagename.GetFullPath()<<ENDL);
		//	lglLOG("base  : "<<basename.GetFullPath()<<ENDL);
		//	lglLOG("sxs   : "<<sxsname.GetFullPath()<<ENDL);

		//double min,max;
		// TO CHANGE TO HANDLE ANY IMAGE TYPE !
#ifndef LGL_WITH_BOOST
		if (verbose)   wxLogWarning(_T("ImageScaleSets::load imgage '%s'"), imagename.GetFullName().c_str());
#else
		if (verbose)  {
			std::string msg = lgl::string_format(std::string("ImageScaleSets::load imgage '%s'"), imagename.generic_string().c_str());
			lglWARNING(msg);		
		}
#endif
		if (!lgl::readImage(imagename, m_input))
			return false;
		//	lgl::computeMinMax(*m_input, min, max);
		//	lglLOG("image : "<<m_input->size()<<" min/max = "<<min<<"/"<<max<<ENDL);

		// TO CHANGE !
#ifndef LGL_WITH_BOOST
		if (verbose) 	wxLogWarning(_T("ImageScaleSets::load 10 base '%s'"), basename.GetFullName().c_str());
		if (verbose) 	wxLogWarning(_T("ImageScaleSets::load 10 base path '%s'"), basename.GetPath().c_str());
#else
		if (verbose) {
			std::string msg = lgl::string_format(std::string("ImageScaleSets::load 10 base '%s'"), basename.generic_string().c_str());
			lglWARNING(msg);		
		}
		if (verbose) { 
			std::string msg = lgl::string_format(std::string("ImageScaleSets::load 10 base path '%s'"), basename.parent_path().generic_string().c_str());
			lglWARNING(msg);		
		}
#endif
		bool caught = false;
		std::stringstream errorStream;

		try {
#ifndef LGL_WITH_BOOST
			std::string path( basename.GetPath().mb_str() );
			path += "/";
			path += basename.GetFullName().mb_str();
#else
			std::string path = basename.generic_string();
#endif

		if (verbose){
			std::string msg = lgl::string_format(std::string("ImageScaleSets::load loading base image '%s'  as a socle inmage"), path.c_str());
			lglWARNING(msg);
		}
		ign::image::BufferImage<int> m_base_socle(path.c_str());
		if (verbose){
			std::string msg = lgl::string_format(std::string("ImageScaleSet::load image loaded '%d'x'%d'x'%d'"), m_base_socle.numLines(), m_base_socle.numCols(), m_base_socle.numBands());
			lglWARNING(msg);
		}
		lgl::ImageRamT<int>* m_base_typed = new lgl::ImageRamT<int>();
		lgl::BufferImageToImageRamT(m_base_socle, *m_base_typed);

		m_base = m_base_typed;


		}
		IGN_CATCH_ALL_EXCEPTIONS(errorStream, caught, 0);
		if (caught) {
			std::string msg = lgl::string_format(std::string("Erreur de chargement : %s "),  errorStream.str().c_str() );
			lglWARNING(msg);
			return false;
		}

		//  if (!lgl::readImage(basename, m_base))
		//      return false;

		//	lgl::computeMinMax(*m_base, min, max);
		//	lglLOG("base  : "<<m_base->size()<<" min/max = "<<min<<"/"<<max<<ENDL);
#ifndef LGL_WITH_BOOST
		if (verbose) wxLogWarning(_T("ImageScaleSets::load 10 sxs '%s'"), sxsname.GetFullName().c_str());
#else
		if (verbose) {
			std::string msg = lgl::string_format(std::string("ImageScaleSets::load 10 sxs '%s'"), sxsname.generic_string().c_str());
			lglWARNING(msg);
		}
#endif
		if (!ScaleSets::load(sxsname))
			return false;
		//	lglLOG("sxs   : "<<this->size()<<" sets, root soa = "<<this->getRoot()->scaleOfAppearance()<<ENDL);

		if (!testBaseImageCoherence()) {
#ifndef LGL_WITH_BOOST
			lglWARNING("ImageScaleSets::load "<<filename.GetFullPath().c_str()<<" data corrupted (incoherence between base image and sets)"<<ENDL);
#else
			lglWARNING("ImageScaleSets::load "<<filename.generic_string().c_str()<<" data corrupted (incoherence between base image and sets)"<<ENDL);
#endif
			return false;
		}

		if (verbose)
			lglWARNING(std::string("ImageScaleSets::load OK"));

		return true;
	}
	//======================================================

	//======================================================
	/// Save the ImageScaleSets in an ".iss" file
#ifndef LGL_WITH_BOOST
	lgl::BOOL ImageScaleSets::save(const lgl::Filename& filename, const lgl::Filename& ori) {
		lgl::Filename issfilename(filename);
		//=========================================================
		// Creates the .iss
		issfilename.SetExt(wxT("iss"));
		issfilename.MakeAbsolute();
#if wxUSE_UNICODE
		std::ofstream f((issfilename.GetFullPath()).ToAscii().data(),PARAM_OFSTREAM_TEXT);
#else
		std::ofstream f((issfilename.GetFullPath()).c_str(), PARAM_OFSTREAM_TEXT);
#endif //#if wxUSE_UNICODE


		if (!f.good())
			return false;

		f << "sxs::ImageScaleSets - Version " << __SXS_VERSION__ << " - (c) Laurent Guigues 2004" << std::endl;

		f << "input_image       :";
		lgl::Filename imagename(filename);
		lgl::BOOL writeimage = false;
		
		if (ori.IsOk() && ori.FileExists(ori.GetFullPath())) {
			lgl::Filename o(ori);
			o.MakeRelativeTo(issfilename.GetPath());
#if wxUSE_UNICODE
			f << o.GetFullPath().ToAscii().data() << std::endl;
#else
			f << o.GetFullPath().c_str() << std::endl;
#endif //#if wxUSE_UNICODE
		} else {
			lgl::String name = filename.GetName() + wxT("_image");
			imagename.SetName(name);
			imagename.SetExt(wxT("hdr"));
			imagename.MakeRelativeTo(issfilename.GetPath());
#if wxUSE_UNICODE
			f << imagename.GetFullPath().ToAscii().data() << std::endl;
#else
			f << imagename.GetFullPath().c_str() << std::endl;
#endif //#if wxUSE_UNICODE
			writeimage = true;
		}

		f << "base_segmentation :";
		lgl::Filename basename(filename);
		lgl::String name = filename.GetName() + wxT("_base");
		basename.SetName(name);
		basename.SetExt(wxT("rle"));
		basename.MakeRelativeTo(issfilename.GetPath());
#if wxUSE_UNICODE
		f << basename.GetFullPath().ToAscii().data() << std::endl;
#else
		f << basename.GetFullPath().c_str() << std::endl;
#endif //#if wxUSE_UNICODE
		f << "scale_sets        :";
		lgl::Filename sxsname(filename);
		sxsname.SetExt(wxT("sxs"));
		sxsname.MakeRelativeTo(issfilename.GetPath());
#if wxUSE_UNICODE
		f << sxsname.GetFullPath().ToAscii().data() << std::endl;
#else
		f << sxsname.GetFullPath().c_str() << std::endl;
#endif //#if wxUSE_UNICODE
		//=========================================================
		// TO DO : save parameters, meta data, and so on ...
		//=========================================================
		f.close();
		// EO .iss creation
		//=========================================================

		//=========================================================
		// If writeimage==true saves the input image
		if (writeimage) {
			imagename.MakeAbsolute(issfilename.GetPath());
			if (!lgl::writeImageAnalyze(imagename, *m_input))
				return false;
		}
		//=========================================================
		// Saves the base segmentation
		basename.MakeAbsolute(issfilename.GetPath());
		{
			ign::image::BufferImage<int> segbase;
			lgl::ImageRamToBufferImage(*m_base, segbase);

#if wxUSE_UNICODE
			segbase.save(basename.GetFullPath().ToAscii().data());
#else
			segbase.save(basename.GetFullPath().c_str());

#endif //#if wxUSE_UNICODE
		}
		//    if (!lgl::writeImageAnalyze(basename,*m_base))
		//      return false;
		//=========================================================
		// Saves the ScaleSets : calls the ScaleSets::save method
		sxsname.MakeAbsolute(issfilename.GetPath());
		if (!ScaleSets::save(sxsname))
			return false;
		//=========================================================

		return true;
	}
	
#else
	
	lgl::BOOL ImageScaleSets::save(const lgl::Filename& filename, const lgl::Filename& ori) {

		std::cout<<"ImageScaleSets save "<<std::endl;
		std::cout<<"filename  "<<filename.generic_string().c_str()<<std::endl;

		lgl::Filename issfilename(filename);
		//=========================================================
		// Creates the .iss

		issfilename.replace_extension(".iss");
		std::ofstream f(issfilename.generic_string().c_str(), PARAM_OFSTREAM_TEXT);
		
		if (!f.good())	return false;
		
		issfilename = boost::filesystem::canonical(issfilename);

		std::cout<<"issfilename canonical  "<<issfilename.generic_string().c_str()<<std::endl;

		f << "sxs::ImageScaleSets - Version " << __SXS_VERSION__ << " - (c) Laurent Guigues 2004" << std::endl;
		
		f << "input_image       :";
		lgl::Filename imagename(filename);
		lgl::BOOL writeimage = false;
		
		if (boost::filesystem::exists(ori)) {
			lgl::Filename o(ori);
			o = ign::filesystem::make_relative(o, issfilename.parent_path() );


			f << o.generic_string().c_str() << std::endl;
		} else {
			lgl::String name = filename.stem().generic_string() + "_image.hdr";
			imagename = imagename.parent_path();
			imagename/=name;
			// imagename = ign::filesystem::make_relative(imagename, issfilename.parent_path() );
			imagename = ign::filesystem::make_relative(issfilename.parent_path(), imagename  );
			f << imagename.generic_string().c_str() << std::endl;
			writeimage = true;
		}
		
		f << "base_segmentation :";
		lgl::Filename basename(filename);
		lgl::String name = filename.stem().generic_string() + "_base.tif";
		basename = imagename.parent_path();
		basename/=name;
		basename = ign::filesystem::make_relative(issfilename.parent_path(), basename);
		f << basename.generic_string().c_str() << std::endl;
		
		f << "scale_sets        :";
		lgl::Filename sxsname(filename);
		sxsname.replace_extension(".sxs");
		sxsname = ign::filesystem::make_relative(issfilename.parent_path(), sxsname );
		f << sxsname.generic_string().c_str() << std::endl;
		//=========================================================
		// TO DO : save parameters, meta data, and so on ...
		//=========================================================
		f.close();
		// EO .iss creation
		//=========================================================
		
		//=========================================================
		// If writeimage==true saves the input image
		if (writeimage) {
			std::string name = imagename.filename().generic_string();
			imagename = issfilename.parent_path();
			imagename /= name;
			// imagename = boost::filesystem::canonical(imagename);
			std::cout<<"imagename canonical  "<<imagename.generic_string().c_str()<<std::endl;
			if (!lgl::writeImageAnalyze(imagename, *m_input))
				return false;
		}
		//=========================================================
		// Saves the base segmentation
		{
			std::string name = basename.filename().generic_string();
			basename = issfilename.parent_path();
			basename /= name;
			// basename = boost::filesystem::canonical(basename);
		}
		{
			ign::image::BufferImage<int> segbase;
			lgl::ImageRamToBufferImage(*m_base, segbase);
			std::cout<<"basename   "<<basename.generic_string().c_str()<<std::endl;

			segbase.save(basename.generic_string().c_str());
			
		}
		//    if (!lgl::writeImageAnalyze(basename,*m_base))
		//      return false;
		//=========================================================
		// Saves the ScaleSets : calls the ScaleSets::save method
		//sxsname.MakeAbsolute(issfilename.GetPath());
		{
			std::string name = sxsname.filename().generic_string();
			sxsname = issfilename.parent_path();
			sxsname /= name;
			// sxsname = boost::filesystem::canonical(sxsname);
		}
		std::cout<<"sxsname   "<<sxsname.generic_string().c_str()<<std::endl;

		if (!ScaleSets::save(sxsname))
			return false;
		//=========================================================
		return true;
	}
#endif
	
	
	
	//======================================================

	//================================================================

	void ImageScaleSets::getSection(lgl::F32 scale, lgl::pImageRam& out) {

#ifdef __SXS_SXS_TEST__
		if (!verifySlots()) lglERROR("Slots NOK !");
		if (!testBaseImageCoherence()) {lglERROR("Base Image to Base Sets mapping NOK !");}
#endif

		// maps labels (indices) to the section sets indices
		std::map<int, int> lmap;
		// marks indices already seen
		Slot seen;
		if (!allocateSlot(seen)) {
			lglERROR("ImageScaleSets::getSection(scale,image_out) : unable to allocate a 1 bit Slot !");
			return;
		}
		//
		for (m_base->start(); !m_base->stop(); ++(*m_base)) {
			int l = (int) (*(*m_base));
			Set* b = operator[](l);
			if (!b) {
				lglERROR("ImageScaleSets::getSection(scale,image_out) : incoherence between label image and ScaleSets indices");
			}
			// already seen
			if (b->getMark(seen) == 1) {
				// verify that is mapped
				if ((*(lmap.find(l))).first != l) {
					lglWARNING("Label "<<l<<" seen but not mapped !!"<<std::endl);
				}
				continue;
			}
			b->setMark(seen, 1);
			// index of its maximal ancestor of lower scale than scale
			int lmax = b->getMaximalAncestor(scale)->index();
			// map insertion
			lmap.insert(std::pair<int, int>(l, lmax));
		}
		// unmarks the mapped elements
		std::map<int, int>::const_iterator mit;
		std::set<int> sec;
		for (mit = lmap.begin(); mit != lmap.end(); ++mit) {
			operator[]((*mit).first)->setMark(seen, 0);
			sec.insert((*mit).second);
		}
		// frees the Slot seen
		freeSlot(seen);
		// image out construction
		lgl::map(*m_base, lmap, out);

#ifdef __SXS_SXS_TEST__
		if (!verifySlots()) {lglERROR("Slots NOK !");}
		if (!testBaseImageCoherence()) {lglERROR("Base Image to Base Sets mapping NOK !");}
#endif

	}
	//================================================================

	//================================================================

	void ImageScaleSets::getSection(lgl::ImageRam& in, lgl::F32 scale, lgl::pImageRam& out) {
		//	lglLOG("ImageScaleSets::getSection(in,"<<scale<<")"<<ENDL);

#ifdef __SXS_SXS_TEST__
		if (!verifySlots()) {lglERROR("Slots NOK !");}
		if (!testBaseImageCoherence()) {lglERROR("Base Image to Base Sets mapping NOK !");}
#endif

		// maps labels (indices) to the section sets indices
		std::map<int, int> lmap;
		// marks indices already seen
		Slot seen;
		if (!allocateSlot(seen)) {
			lglERROR("ImageScaleSets::getSection(image_in,scale,image_out) : unable to allocate a 1 bit Slot !");
			return;
		}
		//
		for (in.start(); !in.stop(); ++in) {
			int l = (int) (*in);
			Set* b = operator[](l);
			if (!b) {
				lglERROR("ImageScaleSets::getSection(scale,image_out) : incoherence between label image and ScaleSets indices");
			}
			// already seen
			if (b->getMark(seen) == 1) {
				// verify that is mapped
				if ((*(lmap.find(l))).first != l) {
					lglWARNING("Label "<<l<<" seen but not mapped !!"<<std::endl);
				}
				continue;
			}
			b->setMark(seen, 1);
			// index of its maximal ancestor of lower scale than scale
			int lmax = b->getMaximalAncestor(scale)->index();
			// map insertion
			lmap.insert(std::pair<int, int>(l, lmax));
		}
		// unmarks the mapped elements
		std::map<int, int>::const_iterator mit;
		std::set<int> sec;
		for (mit = lmap.begin(); mit != lmap.end(); ++mit) {
			operator[]((*mit).first)->setMark(seen, 0);
			sec.insert((*mit).second);
		}
		// frees the Slot seen
		freeSlot(seen);
		// image out construction
		lgl::map(in, lmap, out);

#ifdef __SXS_SXS_TEST__
		if (!verifySlots()) {lglERROR("Slots NOK !");}
		if (!testBaseImageCoherence()) {lglERROR("Base Image to Base Sets mapping NOK !");}
#endif

	}
	//================================================================

	//================================================================
	/// Deletes the sets whose persistence is lower than a given threshold.
	/// Performs a top-down traversal to compute the scale of disappearance of the sets
	/// and simultaneously removes the sets of too low persistence.

	void ImageScaleSets::removeNonPersistentSets(lgl::F32 minimum_persistence) {
		if (!verifySlots()) {
			lglERROR("Slots NOK !");
		}
		if (!testBaseImageCoherence()) {
			lglERROR("BaseSegmentation to Base Sets mapping NOK !");
		}

		// slot allocation
		lgl::BitSlot m;
		if (!allocateSlot(m, 1))
			return;
		// recursive marking and non persistent base mapping
		std::map<int, int> npbase;
		m_rec_mark_non_persistent_and_map_npbase(*m_list.rbegin(), m, (std::numeric_limits<lgl::F32>::max)(), minimum_persistence,
				(*m_list.rbegin())->index(), npbase);

		// maps the old base to the new one
		lgl::map(*m_base, npbase, m_base);
		// prunes the marked sets
		removeMarkedSets(m);
		// slot unalloc
		freeSlot(m);

		if (!verifySlots()) {
			lglERROR("Slots NOK !");
		}
		if (!testBaseImageCoherence()) {
			lglERROR("Base Image to Base Sets mapping NOK !");
		}
	}
	//================================================================

	//================================================================
	/// Recursive marking function used in removeNonPersistentSets.
	/// Also collects non persistent base sets and maps their index to their minimal persistent ancestor (npmap)

	void ImageScaleSets::m_rec_mark_non_persistent_and_map_npbase(Set* s, const lgl::BitSlot& m, lgl::F32 scale_of_disappearance,
			lgl::F32 minimum_persistence, int pa, std::map<int, int>& npbase) {
		// non persistent
		if (scale_of_disappearance <= minimum_persistence * s->scaleOfAppearance()) {
			s->setMark(m, 1);
			// Base set ?
			if (s->isBase()) {
				// mapping
				npbase.insert(std::pair<int, int>(s->index(), pa));
			}
		}
		// persistent : becomes the current minimal persistent node of its branch
		else {
			pa = s->index();
		}
		// sod
		if (s->scaleOfAppearance() < scale_of_disappearance) {
			scale_of_disappearance = s->scaleOfAppearance();
		}
		// recursive call on childrens
		std::vector<Set*>::const_iterator i;
		for (i = s->sons().begin(); i != s->sons().end(); ++i) {
			m_rec_mark_non_persistent_and_map_npbase(*i, m, scale_of_disappearance, minimum_persistence, pa, npbase);
		}
	}
	//================================================================

	//================================================================
	/// Tests that the base image labels are in a 1 to 1 mapping with the base sets indices

	lgl::BOOL ImageScaleSets::testBaseImageCoherence() {
		lgl::BOOL ok = true;
		std::set<int> labels;
		int n = 0;
		for (m_base->start(); !m_base->stop(); ++(*m_base)) {
			int l = (int) (*(*m_base));
			//		std::cout << n << ":" << l << " ";
			n++;
			Set* s = operator[](l);
			if (!s) {
				lglWARNING("ImageScaleSets::testBaseImageCoherence() : label "<<l<<" does not corresponds to a set !");
				ok = false;
			} else if (!s->isBase()) {
				lglWARNING("ImageScaleSets::testBaseImageCoherence() : the set corresponding to label "<<l<<" is not a base set !");
				ok = false;
			}
			labels.insert(l);
		}
		// converse test
		std::list<Set*>::iterator i;
		for (i = m_list.begin(); i != m_list.end(); ++i) {
			if ((*i)->isBase()) {
				if (labels.find((*i)->index()) == labels.end()) {
					lglWARNING(
							"ImageScaleSets::testBaseImageCoherence() : the base set of label "<<(*i)->index()<<" is not in the base segmentation !");
					ok = false;
				}
			}
		}
		return ok;
	}
//================================================================

}
//=================================================================================
// EO namespace sxs
//=================================================================================
