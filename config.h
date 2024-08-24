#ifndef  CONFIG_H_
#define  CONFIG_H_

//----------------------------------------------------------------------------- ---------------------------------------
// ws2812B 8x8 (and other sizes)
// https://www.aliexpress.com/item/1005007032871138.html
// https://www.aliexpress.com/item/1005007055750516.html
// Magnets: https://www.aliexpress.com/item/1005007294064412.html
//
#define WS_IS_RGB   (true)
#define WS_IS_RGBW  (!WS_IS_RGB)

#define WS_W        (8)
#define WS_H        (8)
#define NUM_PIXELS  (WS_W * WS_H)

#define PIN_WS2812  (16)  // GPIO #16

#endif //CONFIG_H_
