/* 

lGl : A C++ library needed by the multiscale image segmentation library SxS
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
	\brief Includes all the headers of lgl.
*/
//===========================================================
// namespace	: lgl
//  includes all the headers of the library
//===========================================================
// author		: Laurent Guigues
// history		: 
//	11/07/04		creation
//===========================================================
// General inclusions, preprocessor definitions, and so on
#include "lgl/lglGeneral.h"
// little/big endian machine handler
#include "lgl/lglMachine.h"
// general templates for unary/binary/ternary functions
#include "lgl/lglFunction.h"

// pseudo-random numbers generators
#include "lgl/lglRandom.h"

// 2D points 
#include "lgl/lglPoint2D.h"
// 0 to 4D points 
#include "lgl/lglPoint4D.h"
// abstract collections of objets 
#include "lgl/lglCollection.h"
// vectors (linear algebra objects inherited from std::vector STL objects)
#include "lgl/lglVector.h"
// abstract arrays 
#include "lgl/lglArray.h"
// 2D arrays (inherited from BinaryFunction and Array)
#include "lgl/lglArray2D.h"
// Matrices (inherited from Array2D)
#include "lgl/lglMatrix.h"

// Bitwise operations and bit allocation/desallocation
#include "lgl/lglBitManager.h"

// Priority queues implemented as binary trees allowing fast element location in the queue (for priority change)
#include "lgl/lglIndexedHeap.h"

// Arbitrary (and dynamical) size arrays allowing random indices (sparsity) while keeping a constant time random access   
// (basically it is a efficient implementation of a STL map<int,T>).
// Handled by a 4 levels dynamic tree of arity 256.
#include "lgl/lglSparseArray.h"

// Abstract image definition
#include "lgl/lglImage.h"
// 2d/3d Images stored in RAM (inherited from Image)
#include "lgl/lglImageRam.h"
// Various functions for image manipulation
#include "lgl/lglImageManip.h"
// Image loading/saving methods
#include "lgl/lglImageIO.h"
// Handler for the Analyze medical image format 
#include "lgl/lglAnalyzeHeader.h"

/// IMAGE ALGORITHMS
/// Basics : connected components, local optima
#include "lgl/lglImageBasicAlgorithms.h"
// Image partitioning using the generalized Voronoi tesselation approach of Arbelaez and Cohen
#include "lgl/lglVoronoiImageTesselation.h"
// Boundary tracker
#include "lgl/lglInterPixelBoundaryTracker.h"

// A node/edge + edge linking-based attributed graph structure implementation (template attributes)
#include "lgl/lglGraph.h"
// Graphs whose nodes are image sites and edges are adjacency relationships between neighboring image sites 
#include "lgl/lglPixelGraph.h"
// Methods for building region adjacency graphs (RAG) from label images
#include "lgl/lglLabelImageToGraph.h"


// CLASSES/METHODS USING wxWidgets
// Color images (3 channels byte images) based on wxImage (inherits wxImage,ImageInOut)
// #include "ign/image/segment/lgl/lglwxImageRGB.h"
// A wxFrame-based class implementing a 3D image viewer (slicing...)
// #include "ign/image/segment/lgl/lglwxImage3DViewer.h"
// A log window (based on wxLogWindow)  
// #include "ign/image/segment/lgl/lglwxLogWindow.h"
// A basic frame for drawing a RGBImage
// #include "ign/image/segment/lgl/lglwxImageFrame.h"


// INTERFACE WITH IGN/MATIS's "NOYAU" LIBRARY
// one must define __NOYAU__
// #include "ign/image/segment/lgl/lglNoyau.h"
