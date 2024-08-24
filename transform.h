#ifndef  TRANSFORM_H_
#define  TRANSFORM_H_

// TO ADD A TRANSFORM
//                                             ____ EXAMPLE _____________________
// 1. Pick a name for it                     :      "4x3_A"
// 2. In the .h file
//    a) Create a transID enum for it        :     T_4X3_A
//    b) Create a function prototype         : int t_4x3_a (int x,  int y) ;
// 3. In the .c file
//    a) write the function                  : int t_4x3_a (int x,  int y) { ... }
//    b) add it to the lookup table          :    [T_2X3_A] = t_2x3_a,

#include  "banner.h"  // uint

//----------------------------------------------------------------------------- ---------------------------------------
// Transform functions
//
typedef  int (*transFn_t) (int, int) ;

// List of transform function prototypes
int  t_2x3_a (int x,  int y) ;
int  t_3x2_a (int x,  int y) ;

//----------------------------------------------------------------------------- ---------------------------------------
// Lookup table
//
extern  transFn_t   trans[];
extern  const uint  transCnt;

//----------------------------------------------------------------------------- ---------------------------------------
// Available transformations
//
typedef
	enum transID {
		T_2X3_A,  // W x H
		T_3X2_A,

		T_EOT     // End of Table - MUST be LAST!
	} 
transID_t;

//----------------------------------------------------------------------------- ---------------------------------------
typedef
	struct grid {
		int        w;     // width (in pixles)
		int        h;     // height
		int        bcnt;  // board count
		int        pcnt;  // pixel count
		transID_t  id;
		transFn_t  fn;
	}
grid_t;

extern  grid_t  grid;

//----------------------------------------------------------------------------- ---------------------------------------
// transform() entry point
//
static inline  int  transform (int x,  int y)  {  return grid.fn(x, y);  }

#endif //TRANSFORM_H_
