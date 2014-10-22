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

//distance transformation
#include "DGtal/geometry/volumes/distance/DistanceTransformation.h"

//board 2D
#include "DGtal/io/boards/Board2D.h"
#include <DGtal/io/colormaps/HueShadeColormap.h>

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
  typedef GaussDigitizer<Z2i::Space,Flower> Gauss;
  Gauss dig;  
  dig.attach( flower );
  dig.init( flower.getLowerBound()+Z2i::Vector(-1,-1),
            flower.getUpperBound()+Z2i::Vector(1,1), h ); 

  
  ///////////////// second part: display ////////////////////////////
  //board
  DGtal::Board2D board;

  //We get the digitzed object digital domain
  typedef Gauss::Domain Domain;
  Domain domain= dig.getDomain();

  //we display it
  board << domain;
  trace.info()<< "Domain = "<< domain<<std::endl;
  
  //for each arrow, draw the corresponding straight segment
  for(Domain::ConstIterator it = domain.begin() ; it != domain.end();
       ++it)
    {
      Z2i::Point P = *it;

      //if P belongs to the digitization
      if (dig(P))
        board << P;
    }
  //export
  board.saveEPS("shape-init.eps");  


  ///////////////// third part: DT /////////////////////////

  //Euclidean Metric distance transformation computed on the domain
  //'domain'
  //DT labels all points in the domain  with the distacne to the closest
  // point not belonging to 'dig'
  typedef DistanceTransformation<Z2i::Space, Gauss, Z2i::L2Metric> DT;

  DT dt(domain, dig, Z2i::l2Metric);

  //We display the DT using colormap
  double maxval = 0.0;
  double minval = 0.0;
  for(DT::Domain::ConstIterator it = dt.domain().begin() ; it != dt.domain().end(); ++it)
    {
      // (*it) is a pointn dt(*it) is the DT value at (*it)
      if ( dt(*it)>maxval )  maxval = dt(*it);
    }
  trace.info() << "Max DT value= "<<maxval<<std::endl;

  //we create the colormap
  typedef HueShadeColorMap<double, 2> Colormap;
  Colormap cmap(minval,maxval);

  
  board.clear();
  for(DT::Domain::ConstIterator it = dt.domain().begin() ; it != dt.domain().end(); ++it)
    {
      DT::Point p = *it;
      double value = dt(*it);
      board <<  CustomStyle( p.className(), new CustomColors( cmap(value) , cmap(value) ) )
            << p;
    }
  
  //Export again
  board.saveEPS("shape-DT.eps");


  return 0;
}

///////////////////////////////////////////////////////////////////////////////
