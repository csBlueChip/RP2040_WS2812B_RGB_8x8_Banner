#include  <stdbool.h>
#include  <stdint.h>
#include  <stdlib.h>
#include  <stdio.h>
#include  <string.h>

#include  "common.h"
#include  "banner.h"
//#include  "transform.h"

#include  "hardware/pio.h"

//-----------------------------------------------------------------------------
uint     bright = 5;
grbw_t*  frame  = NULL;  // FOUR frame buffers

// to toggle between buffers, use `xbuf ^= 1`
uint     bufCnt = 4;
uint     vbuf   = 0;     // active video  buffer {0,1}
uint     ebuf   = 2;     // active effect buffer {2,3}

//-----------------------------------------------------------------------------
colour_t  clist[]  = {
	CLR_RED, CLR_GRN, CLR_BLU, 
	CLR_CYN, CLR_MAG, CLR_YEL, 
	CLR_WHT, CLR_RND, CLR_BLK
};

int  clistCnt = sizeof(clist) / sizeof(*clist);

//+============================================================================
// No transDestroy() ...this program never exits!
//
grbw_t*  frameSetup (const transID_t id)
{
	free(frame), frame = NULL;

	trans[id](0xDEBB1E, 0xDA11A5);  // configure the grid

	uint  size = bufCnt * sizeof(grbw_t) * grid.pcnt;
	if ( (frame = malloc(size)) )  memset(frame, 0, size) ;

	vbuf = 0, ebuf = 2;

	return frame;
}

//+============================================================================
// Dim the specified colour according to the system brightness
//
// The brightness is modified at the point in time the pixel is written the
//   video buffer, NOT when it is written to the pixels
//   ...otherwise we have to recalculate the brightness of every pixel
//      every time it is plotted to the display - which is CPU-expensive
//
// Used by plot() and cls() ...Functions such as fader() may use this function
//   to implement new video effects (such as Fade To Black)
//
// This "curve" is linear - it looks awful
// Triangular roots is the way forward
//
grbw_t  dim (const grbw_t c)
//!! shit fade curve
{
	uint8_t  r = RGBW_R(c);
	uint8_t  g = RGBW_G(c);
	uint8_t  b = RGBW_B(c);

	r = (uint8_t)((((uint16_t)r) * bright) / 100);
	g = (uint8_t)((((uint16_t)g) * bright) / 100);
	b = (uint8_t)((((uint16_t)b) * bright) / 100);

	return RGB(r,g,b);
}

//+============================================================================
// Render the specified frame buffer
// false => bad frame buffer
//
bool  show (const uint f)
{
	if (f >= bufCnt)  return false ;

	grbw_t*  pp = frame + (f * grid.pcnt);  // frame base address

	for (int i = 0;  i < grid.pcnt;  i++,  pp++)
		pio_sm_put_blocking(pio0, 0, *pp);
//	sleep_us(50);

	return true;
}

//+============================================================================
// x,y  {0,0} is bottom-left
// false => clipped
//
bool  plot (const int x,  const int y,  const grbw_t _c)
{
	grbw_t   c    = dim(_c);
	grbw_t*  fp   = frame + (vbuf * grid.pcnt);  // frame base address
	uint     offs = transform(x, y);             // pixel offset

	return (offs < 0) ? false : (*(fp+offs) = c), true ;
}

//+============================================================================
// randomSeed(analogRead(A0));  ...poor, but better than nothing I guess :/
void  cls (const grbw_t _c)
{
	grbw_t*  pp = frame + (vbuf * grid.pcnt);  // frame base address

	if (_c == CLR_RND) {
		for (int i = 0;  i < grid.pcnt;  i++)  *pp++ = dim(rand()) ;

	} else {
		grbw_t  c  = dim(_c);
		for (int i = 0;  i < grid.pcnt;  i++)  *pp++ = c ;
	}
}
