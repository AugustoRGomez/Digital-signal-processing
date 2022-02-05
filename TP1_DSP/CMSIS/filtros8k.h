/*
 * Discrete-Time FIR Filter (real)
 * -------------------------------
 * Filter Structure  : Direct-Form FIR
 * Filter Length     : 63
 * Stable            : Yes
 * Linear Phase      : Yes (Type 1)
 */

/* General type conversion for MATLAB generated C-code  */
//#include "tmwtypes.h"
/* 
 * Expected path to tmwtypes.h 
 * C:\Program Files\MATLAB\R2017b\extern\include\tmwtypes.h 
 */

const int NL = 63;
const q15_t N[63] = {
      106,   -139,    165,   -180,    182,   -167,    134,    -81,     11,
       73,   -167,    263,   -353,    429,   -482,    501,   -480,    412,
     -292,    121,    101,   -367,    671,  -1000,   1341,  -1680,   2000,
    -2288,   2528,  -2709,   2821,  29819,   2821,  -2709,   2528,  -2288,
     2000,  -1680,   1341,  -1000,    671,   -367,    101,    121,   -292,
      412,   -480,    501,   -482,    429,   -353,    263,   -167,     73,
       11,    -81,    134,   -167,    182,   -180,    165,   -139,    106
};
