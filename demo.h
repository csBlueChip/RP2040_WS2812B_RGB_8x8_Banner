#ifndef  DEMO_COLOUR_H_
#define  DEMO_COLOUR_H_

#include  "banner.h"  // grbw_t
#include  "common.h"  // uint

void  demo_font    (void) ;
void  demo_primary (const uint   ms) ;
void  demo_steps   (const grbw_t _c,  const int ms) ;
void  demo_hrot    (const int    _n) ;
void  demo_twinkle (const int    ms) ;
void  demo_walk    (const grbw_t _c,  const int ms) ;

void  fx_dim       (uint _from) ;
void  fx_fader     (uint b,    int ms) ;
void  demo_fader   (grbw_t c,  int cnt) ;


#endif //DEMO_COLOUR_H_
