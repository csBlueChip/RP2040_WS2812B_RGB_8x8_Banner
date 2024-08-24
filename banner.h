#ifndef  BANNER_H_
#define  BANNER_H_

#include  <stdint.h>
#include  <stdbool.h>

#include  "common.h"
#include  "config.h"

//----------------------------------------------------------------------------- ---------------------------------------
// typedefs
//
typedef  uint32_t      grbw_t;  // the colour of a single pixel

//----------------------------------------------------------------------------- ---------------------------------------
extern  uint      bright;  // 0..100% ...this is done at PLOT (not SHOW) time - to reduce CPU overhead
extern  grbw_t*   frame;   // FOUR frame buffers
extern  uint      vbuf;    // active video  buffer {0,1}  `buf ^ 1` toggles between buffers
extern  uint      ebuf;    // active effect buffer {2,3}  ...for both pairs

//----------------------------------------------------------------------------- ---------------------------------------
// MACROS
//
// Generate pixel colour from RGB colours
#define  RGBW(r,g,b,w)  ( (((grbw_t)(g) & 0xFF) << 24) | (((grbw_t)(b) & 0xFF) << 8) \
                        | (((grbw_t)(r) & 0xFF) << 16) |  ((grbw_t)(w) & 0xFF)       )

#define  RGB(r,g,b)     ( (((grbw_t)(g) & 0xFF) << 24) | (((grbw_t)(b) & 0xFF) << 8) \
                        | (((grbw_t)(r) & 0xFF) << 16) |  ((grbw_t)(0) & 0xFF)       )

//----------------------------------------------------------------------------- ---------------------------------------
// more typedefs
//

// Let's define all the primary colours (plus black & white) + the "random colour" ID
typedef
	enum colour { // this is used as a mask, {0x00, 0xFF} only
		CLR_BLK  = RGBW(0x00, 0x00, 0x00, 0x00),  // Darkest
		CLR_BLU  = RGBW(0x00, 0x00, 0xFF, 0x00),  //    |
		CLR_RED  = RGBW(0xFF, 0x00, 0x00, 0x00),  //    v
		CLR_MAG  = RGBW(0xFF, 0x00, 0xFF, 0x00),  //    |
		CLR_GRN  = RGBW(0x00, 0xFF, 0x00, 0x00),  //    |
		CLR_CYN  = RGBW(0x00, 0xFF, 0xFF, 0x00),  //    v
		CLR_YEL  = RGBW(0xFF, 0xFF, 0x00, 0x00),  //    |
#if WS_RGBW
		CLR_WHT  = RGBW(0x00, 0x00, 0x00, 0xFF),  // Brightest (RGBW)
		CLR_BWHT = RGBW(0xFF, 0xFF, 0xFF, 0xFF),  // Brighter'est (RGBW)
#else
		CLR_WHT  = RGBW(0xFF, 0xFF, 0xFF, 0x00),  // Brightest (RGB)
#endif
		CLR_RND  = RGBW(0x2B, 0x00, 0xB1, 0xE5),  // Special value: random colour
	}
colour_t;

extern  colour_t  clist[];
extern  int       clistCnt;

//----------------------------------------------------------------------------- ---------------------------------------
// more MACROS
//
#define  RGB_G(grbw)   (uint8_t)( ((grbw_t)(grbw) & CLR_GRN) >>24 )
#define  RGB_R(grbw)   (uint8_t)( ((grbw_t)(grbw) & CLR_RED) >>16 )
#define  RGB_B(grbw)   (uint8_t)( ((grbw_t)(grbw) & CLR_BLU) >> 8 )

#define  MIN3(a,b,c)   ( (((a)<(b)) && ((a)<(c))) ? (a) : (((b)<(c)) ? (b) : (c)) )
#define  RGB_W(grbw)   (uint8_t)( (grbw_t)MIN3(RGB_R(grbw), RGB_G(grbw), RGB_B(grbw)) )
// MIN3: The lowest level of any LED (within a single pixel) defines the "whiteness" of the pixel
// eg. R=11, G=19, B=31 ...is seen as: White=11 + Red=0 + Green=8 + Blue=20

#define  RGBW_G(grbw)  (uint8_t)( ((grbw_t)(grbw) & CLR_GRN) >>24 )
#define  RGBW_R(grbw)  (uint8_t)( ((grbw_t)(grbw) & CLR_RED) >>16 )
#define  RGBW_B(grbw)  (uint8_t)( ((grbw_t)(grbw) & CLR_BLU) >> 8 )
#define  RGBW_W(grbw)  (uint8_t)( ((grbw_t)(grbw) & CLR_WHT)      )

#define  XRGB(grbw,r,g,b) \
	do{ r = RGB_R(grbw), g = RGB_G(grbw), b = RGB_B(grbw);                  }while(0)

#define  XRGBW(grbw,r,g,b,w) \
	do{ r = RGB_R(grbw), g = RGB_G(grbw), b = RGB_B(grbw), w = RGB_W(grbw); }while(0)

//----------------------------------------------------------------------------- ---------------------------------------
// library functions
//
bool    show (const uint f) ;
grbw_t  dim  (const grbw_t c) ;
bool    plot (const int x,  const int y,  const grbw_t _c) ;
void    cls  (const grbw_t _c) ;

#include "transform.h"
grbw_t*  frameSetup (transID_t id) ;

//----------------------------------------------------------------------------- ---------------------------------------
// inline funcitons
//
static inline  uint  brightness (const uint b)    {  bright = (b > 100) ? 100 : b ;  }
static inline  bool  refresh    (void)            {  return show(vbuf);  }
static inline  void  clsr       (const grbw_t c)  {  cls(c);  refresh();  }

static inline  bool  plotr      (const int x,  const int y,  const grbw_t c)
	{  return plot(x, y, c) ? refresh() : false ; }

#endif //BANNER_H_
