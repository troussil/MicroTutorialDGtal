///////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <fstream>
#include <algorithm>
///////////////////////////////////////////////////////////////////////////////

#include "DGtal/base/Common.h"
#include "DGtal/helpers/StdDefs.h"

//shape and digitizer
#include "DGtal/shapes/Shapes.h"
#include "DGtal/shapes/ShapeFactory.h"
#include "DGtal/shapes/GaussDigitizer.h"

//tracking grid curve
#include "DGtal/topology/helpers/Surfaces.h"
#include "DGtal/geometry/curves/GridCurve.h"

//length estimation based on a DSS segmentation
#include "DGtal/geometry/curves/estimation/DSSLengthEstimator.h"

//board 2D
#include "DGtal/io/boards/Board2D.h"

using namespace DGtal;

//////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
  trace.info() << "Args:";
  for ( int i = 0; i < argc; ++i )
    trace.info() << " " << argv[ i ];
  trace.info() << std::endl;

  /////////////////// first part: digitization ///////////////////////
  //flower shape
  typedef Flower2D<Z2i::Space> Flower; 
  Flower2D<Z2i::Space> flower(Z2i::Point(0,0), 20, 5, 5, 0);
  
  //implicit digitization of the shape
  double h = 1.0; //grid step  
  if (argc > 1)
    h = std::atof(argv[1]); 
  GaussDigitizer<Z2i::Space,Flower> dig;  
  dig.attach( flower );
  dig.init( flower.getLowerBound()+Z2i::Vector(-1,-1),
            flower.getUpperBound()+Z2i::Vector(1,1), h ); 

  //Khalimsky space
  Z2i::KSpace ks;
  ks.init( dig.getLowerBound(), dig.getUpperBound(), true );

  //searching for one boundary element
  Z2i::SCell bel = Surfaces<Z2i::KSpace>::findABel( ks, dig, 1000 );
  //tracking
  std::vector<Z2i::Point> pointels;
  SurfelAdjacency<2> sAdj( true );
  Surfaces<Z2i::KSpace>
    ::track2DBoundaryPoints( pointels, ks, sAdj, dig, bel );
  //grid curve instanciation
  GridCurve<Z2i::KSpace> c;
  c.initFromVector( pointels );  
  trace.info() << "# " << c.size() << " linels" << std::endl; 

  ///////////////// second part: display ////////////////////////////
  //board
  DGtal::Board2D board;

  //range of arrows
  typedef GridCurve<Z2i::KSpace>::ArrowsRange ARange; 
  ARange ar = c.getArrowsRange(); 

  //for each arrow, draw the corresponding straight segment
  for (ARange::ConstIterator it = ar.begin(), itEnd = ar.end();
       it != itEnd; ++it)
    {
      Z2i::Point P = it->first; 
      Z2i::Point Q = P + it->second; 
      board.drawLine( (double) P[0]*h, (double) P[1]*h,
		     (double) Q[0]*h, (double) Q[1]*h ); 
    }

  //saving
  std::stringstream filenameStream; 
  filenameStream << "DisplayFlowerGridCurve-" << h << ".eps"; 
  board.saveEPS(filenameStream.str().c_str());  
  trace.info() << "# Display in " << filenameStream.str() << std::endl; 


  ///////////////// third part: estimation /////////////////////////
  //range of points
  typedef GridCurve<Z2i::KSpace>::PointsRange PRange; 
  PRange pr = c.getPointsRange(); 
  
  //length estimation by the DSS-based polygonalization method
  DSSLengthEstimator< PRange::ConstCirculator > DSSlength;
  DSSlength.init( h, pr.c(), pr.c() );
  double perimeter = DSSlength.eval();
  trace.info() << "# Perimeter (h=" << h << "): " << perimeter << std::endl; 
  
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
