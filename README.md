# Blend2D-contours
Blend2Dextension for extracting contours from a BLPath
------------------------------------------------------

This is a little Blend2D extension for extracting the contours of a BLPath.
With this extension, you can extract all the contours and the single curves it is made of;
Then you can compute the position of each point on these curves, ando also their
tangent and normal versors.

You are free to use the the code
    Get the source code at
    https://github.com/abu-irrational/Blend2D-contours

These extension introduces a new main class "BLX_Contours", and two inner classes: BLX_Contour and Bcurve.

BLX_Contours: a list of BLX_Contour.
  BLX_Contour: a list of connected Bcurve (Bezier-Curves) of degree 2,3,4
    Bcurve: A Bezier-curve stored in polynomial form for fast evaluation.
    
Usage:

    #include <blend2d.h>
    #include "BLX_Contours.h"
    ...
     // extract the contours from a BLPath
    BLX_Contours contours;
    contours.init(blPath);
    
     // get the numbers of contours:
    size_t nc = contours.size();
    ...
     //  get the i-th contour   ( 0 <= i <contours.size() )
    BLX_Contour contour = contours[i];
    ...
     // get the number of basic curves (strokes) on this contour
    size_t nStrokes = contour.size();    //  contours[j].size()
    ...
     // get the j-th curve on this contour ( 0 <= j < contour.size() )
    BCurve bCurve = contour[j];          //  contours[i][j]
    ...
     // get any BLPoint on bCurve for any t ( 0.0 <=t <=1.0 )
    BLPoint P = bCurve.at(t);
     // Get the tangent or the normal versor
    BLPoint T = bCurve.tangent(t);
    BLPoint N = bCurve.normal(t);
    
     // .. or use a more compact notation ( provided that i,j are in range)
    BLPoint P = contours[i][j].at(t)
    BLPoint T = contours[i][j].tangent(t)
    BLPoint N = contours[i][j].normal(t)
    
