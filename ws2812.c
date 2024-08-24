#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ws2812.pio.h"

#include "banner.h"
#include "demo.h"

//=============================================================================
void  demo (void)
{
#if 0
	brightness(10);
		demo_fader(CLR_RND, 1);
		sleep_ms(500);
#endif

#if 1
	brightness(5);
	demo_primary(500);
	sleep_ms(1000);
#endif

#if 0
	brightness(5);
	demo_walk(CLR_MAG, 10);
	sleep_ms(1000);
#endif

#if 0
	brightness(5);
	cls(CLR_BLK);
	demo_steps(CLR_RND,8);
	demo_steps(CLR_BLK,8);
#endif

#if 0
	brightness(5);
	demo_twinkle(5000);  // mS
#endif

#if 0
	brightness(20);
	demo_hrot(1);  // repetitions
	sleep_ms(1000);
#endif

#if 0
	brightness(50);
	clsr(CLR_RND);
	for (int b = bright;  b >= 0;  fx_fader(b--, 20)) ;  // fade to black
	cls(CLR_BLK);
	sleep_ms(1000);
#endif

#if 1
	brightness(5);
	demo_font();  // never returns
#endif
}

//=============================================================================
void  hang (void)
{
	brightness(5);
	while (true) {
		clsr(CLR_RND);
		sleep_ms(100);
	}
}

//++===========================================================================
int  main (void)
{
	// Start the serial monitor
	stdio_init_all();

	// Load PIO program
	PIO   pio    = pio0;
	int   sm     = 0;
	uint  offset = pio_add_program(pio, &ws2812_program);
	ws2812_program_init(pio, sm, offset, PIN_WS2812, 800000, WS_IS_RGBW);

//	sleep_ms(3000);  // wait for minicom
	printf("# WS2812 demo @ GPIO Pin #%d\n", PIN_WS2812);

	(void)frameSetup(T_3X2_A);

	demo();
//	hang();  // never returns
}
