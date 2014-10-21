///////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <DGtal/base/Common.h>
#include <DGtal/io/boards/Board2D.h>
#include <DGtal/io/colormaps/HueShadeColorMap.h>
#include <DGtal/helpers/StdDefs.h>
///////////////////////////////////////////////////////////////////////////////

using namespace std;
using namespace DGtal;
using namespace DGtal::Z2i; //We'll only consider Z² digital space on
			    //32bit integers

///////////////////////////////////////////////////////////////////////////////

int main()
{
  trace.beginBlock ( "Board examples" );

  Point p1( -3, -2 );
  Point p2( 7, 3 );
  Point p3( 0, 0 );
  Domain domain( p1, p2 );
  

  Board2D board;
  
  //We display the underlying domain
  board << domain ;

  //We display points
  board << p1 << p2 << p3;

  //Output
  board.saveSVG("test.svg");
  board.saveEPS("test.eps");
  board.saveTikZ("test.tikz");
  
  //Clear
  board.clear();
  
  //Update position + color
  p2[0] = 5; //x-coordinate
  board << domain << p1 << p3;
  
  Color red( 255, 0, 0 );
  
  //All points will be in red
  board  << CustomStyle( p2.className(), new CustomColors( red, red ) )
	 << p2;
  //Export again
  board.saveEPS("test2.eps");
 

  
  ////// Colormap example
  //Clear
  board.clear();

  ///Colormap on "doubles" using 2-cycle hueshade colormap
  typedef HueShadeColorMap<double, 2> Colormap;

  double maxval = 100.0;
  double minval = 0.0;

  //we create the colormap
  Colormap cmap(minval,maxval);

  for(unsigned int i=0 ; i < 100 ; ++i)
    {
      Point p3(i,0);
      double value = (double)i; //for this example, the value is the
                                //abscissa

      //we use the colormap to map values to colors
      board <<  CustomStyle( p3.className(), new CustomColors( cmap(value) , cmap(value) ) )
            << p3;
    }
  
  //Export again
  board.saveEPS("test-colormap.eps");

  trace.endBlock();
  return 0;
}
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

