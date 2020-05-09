/* ==========================================================================
**  BLX_Countours.h                                           
**
**  A.Buratti fecit.
**  The Unlicense - 
**   This is free and unencumbered software released into the public domain.                                                                         
** ==========================================================================
*/

#ifndef BLX_CONTOURS_H
#define BLX_CONTOURS_H

#include <vector>

class Bcurve : public std::vector<BLPoint> {
  public:
     // allocate space for a Bcurve of degree n (n+1 points)
    BL_INLINE Bcurve(size_t n) { 
        this->reserve(n+1); 
        for (int i=0; i<=n; ++i) {
            this->push_back(BLPoint());
        }
    }
    BL_INLINE size_t degree() noexcept {return this->size()-1;}
    BLPoint at(double t) noexcept;  // t should be [0.0 .. 1.0]
    BLPoint tangent(double t) noexcept; // normalized; t should be [0.0 .. 1.0] 
    BLPoint normal(double t) noexcept;  // normalized; t should be [0.0 .. 1.0]
};

typedef std::vector<Bcurve> BLX_Contour;

class BLX_Contours : public std::vector<BLX_Contour> {
  public:
    BLResult init (const BLPath& blPath) noexcept; 
};

#endif
