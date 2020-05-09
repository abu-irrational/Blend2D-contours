/* ========================================================================== 
**   BLX_Contours.cxx                                                         
**
**  A.Buratti fecit.
**  The Unlicense - 
**   This is free and unencumbered software released into the public domain.                                                                            */
** ==========================================================================
 */

#include <blend2d.h>
#include <math.h>    // just for sqrt()
#include "BLX_Contours.h"


 // evaluate B(t)   ( t in [0.0..1.0] )
BLPoint Bcurve::at(double t) noexcept {
    const BLPoint* coeff = data();
    BLPoint B = coeff[0];
    double tj = t;  // tj means t**j
    for (int j=1 ; j< size() ; ++j) {
        B += coeff[j]*tj ;
        tj *= t; 
    }
    return B;
}

 // grabbed and adapted from  blend2d/geometry_p.h 
static BL_INLINE BLPoint blUnitVector(const BLPoint& v) noexcept { return v / sqrt(v.x * v.x + v.y * v.y); }


 // evaluate B'(t)   ( t in [0.0..1.0] )
 //  resulting vector is normalized
BLPoint Bcurve::tangent(double t) noexcept {
    const BLPoint* coeff = data();
    BLPoint B = coeff[1];
    double tj = t;  // tj means t**(j-1)
    for (int j=2 ; j< size() ; ++j) {
        B += j*coeff[j]*tj ;
        tj *= t; 
    }
    return blUnitVector(B);
}

 // evaluate normal(t)   ( t in [0.0..1.0] )
 //  resulting vector is normalized
BLPoint Bcurve::normal(double t) noexcept {
    BLPoint P = this->tangent(t);
    // normal is tangent rotated 90 degrees
    return BLPoint(-P.y,P.x);
}


/*
** We're going to scan cmdData[] and vertexData[].
** trying to split vertexData[] in the control-points of eack 'B-curve'
** Note that the first control-point of each stroke is the last point of the previous stroke.
** Special rules:
** The very first cmd (i,e. cmdData[0]) MUST be BL_PATH_MOVE;
*/

BLResult BLX_Contours::init (const BLPath& blPath) noexcept {
    BLResult blRes = BL_SUCCESS;
    const BLPathView& view = blPath.view();

    if (view.size == 0) {
        // having nothing to do is always a success
        return BL_SUCCESS;
    } 

    const uint8_t* cmd = view.commandData;
    const BLPoint* vtx = view.vertexData;

    this->clear();
    BLX_Contour contour = BLX_Contour();
      
    // what happens if first command is not a MOVETO ??
    // --> it's an error INVALID_GEOMETRY
    if ( cmd[0] != BL_PATH_CMD_MOVE ) {
        return BL_ERROR_INVALID_GEOMETRY;
    }

    uint32_t hasPrevVertex = false;
    BLPoint  startVertex;    

    int i = 0;
    while (i < view.size  &&  blRes==BL_SUCCESS) {
        uint32_t c = cmd[i];
        switch (c) {
         case BL_PATH_CMD_MOVE:
            hasPrevVertex = true;
            startVertex = vtx[i];
            break;
         case BL_PATH_CMD_ON:
            if (!hasPrevVertex)  {
                blRes = BL_ERROR_INVALID_GEOMETRY;
                break;
            }
            {
             BLPoint P0 = BLPoint(vtx[i-1]);
             BLPoint P1 = BLPoint(vtx[i]);

             Bcurve stroke = Bcurve(1);
             stroke[0] = P0            ;
             stroke[1] = P0*(-1.0) + P1;            

             contour.push_back(stroke);
            }
            break;
         case BL_PATH_CMD_QUAD:
            if (!hasPrevVertex)  {
                blRes = BL_ERROR_INVALID_GEOMETRY;
                break;
            }
            {
             BLPoint P0 = BLPoint(vtx[i-1]);
             BLPoint P1 = BLPoint(vtx[i]);
             BLPoint P2 = BLPoint(vtx[i+1]);

             Bcurve stroke = Bcurve(2);
             stroke[0] = P0                         ;
             stroke[1] = P0*(-2.0) + P1*(+2.0)      ;            
             stroke[2] = P0        + P1*(-2.0) + P2 ;            

             contour.push_back(stroke);
            }
            i += 1;
            break;
         case BL_PATH_CMD_CUBIC:
            if (!hasPrevVertex)  {
                blRes = BL_ERROR_INVALID_GEOMETRY;
                break;
            }
            {
             BLPoint P0 = BLPoint(vtx[i-1]);
             BLPoint P1 = BLPoint(vtx[i]);
             BLPoint P2 = BLPoint(vtx[i+1]);
             BLPoint P3 = BLPoint(vtx[i+2]);

             Bcurve stroke = Bcurve(3);
             stroke[0] = P0                                    ;
             stroke[1] = P0*(-3.0) + P1*(+3.0)                 ;            
             stroke[2] = P0*(+3.0) + P1*(-6.0) + P2*(+3.0)     ;            
             stroke[3] = P0*(-1.0) + P1*(+3.0) + P2*(-3.0) + P3;            
            
             contour.push_back(stroke);
            }
            i += 2;
            break;
         case BL_PATH_CMD_CLOSE:
            // if current vertex  (vtx[i-1]) is different from startVertex,
            //  then add a straight line to startVertex
            if (vtx[i-1] != startVertex ) {
                 BLPoint P0 = BLPoint(vtx[i-1]);
                 BLPoint P1 = startVertex;

                 Bcurve stroke = Bcurve(1);
                 stroke[0] = P0            ;
                 stroke[1] = P0*(-1.0) + P1;            

                 contour.push_back(stroke);
            }
            hasPrevVertex = false;
            if ( ! contour.empty() ) {
                this->push_back(contour);
            }
            contour.clear();
            break;        
         default:
             // unexpected token
            blRes = BL_ERROR_INVALID_GEOMETRY;
            break;
        }
        ++i;
    }
    if (blRes != BL_SUCCESS) {
        // invalidate everything
        contour.clear();
        this->clear();
    }

    // if the last contour is not empty, append it
    if ( ! contour.empty() ) {
      this->push_back(contour);
    }
    this->shrink_to_fit();
    
    return blRes;    
}
