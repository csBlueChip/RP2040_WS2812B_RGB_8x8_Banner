#include  <stdint.h>

#include  "banner.h"

//+============================================================================
// https://www.purebasic.fr/english/viewtopic.php?t=68518
// I have no idea how it works!
//
static
float  hue2rgb (float p,  float q,  float vH)
{
	if (vH < 0)  vH += 1 ;
	if (vH > 1)  vH -= 1 ;

	if ((6 * vH) < 1)  return (p + (q - p) * 6 * vH) ;
	if ((2 * vH) < 1)  return (q) ;
	if ((3 * vH) < 2)  return (p + (q - p) * ((2/3.0) - vH) * 6) ;
	else               return (p) ;
}

//+============================================================================
// 0 <= {H, S, L} <= 1000
//
grbw_t  hsl2rgb (uint h,  uint s,  uint l)
{
	uint8_t  R, G, B ;

	if ((h > 1000) || (s > 1000) || (l > 1000))  return CLR_BLK ;

	float  H = (h > 1000) ? 1.0 : (h / 1000.0) ;
	float  S = (s > 1000) ? 1.0 : (s / 1000.0) ;
	float  L = (l > 1000) ? 1.0 : (l / 1000.0) ;

	if (s == 0) {
		R = G = B = (uint8_t)(L * 255.0);

	} else {
		float  q = (L < 0.5) ? L * (1.0 + S)
		                     : (L + S) - (L * S) ;

		float  p = (2 * L) - q;

		R = 255 * hue2rgb(p, q, H + (1/3.0));
		G = 255 * hue2rgb(p, q, H          );
		B = 255 * hue2rgb(p, q, H - (1/3.0));
	}

	return RGB(R,G,B);
}
