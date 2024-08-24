#include  <stdio.h>
#include  <stdlib.h>  // NULL

#include  "banner.h"
#include  "hsl2rgb.h"
#include  "common.h"   // uint get_ms()
#include  "text.h"
#include  "get_bootsel_button.h"
#include  "zxorigins/zxorigins.h"

//+============================================================================
// This demo never returns
//
void  demo_font (void)
{
	// What's a scroller without greetz, Lorum ipsum, and pangrams
	const char*  lines[] = {
		"All fonts \177 Damien Guard : https://damieng.com/typography/zx-origins/ #~.,;'@_=-}>  1234567890",
		"The quick brown fox jumps over the lazy dog  #~.,;'@_=-}>  1234567890",
		       "Sphinx of black quartz, judge my vow. #~.,;'@_=-}>  1234567890",
		       "Waltz, bad nymph, for quick jigs vex. #~.,;'@_=-}>  1234567890",
		         "Glib jocks quiz nymph to vex dwarf. #~.,;'@_=-}>  1234567890",
		        "How quickly daft jumping zebras vex! #~.,;'@_=-}>  1234567890",
		       "The five boxing wizards jump quickly. #~.,;'@_=-}>  1234567890",
		      "Jackdaws love my big sphinx of quartz. #~.,;'@_=-}>  1234567890",
		    "Pack my box with five dozen liquor jugs. #~.,;'@_=-}>  1234567890",
		  "Lorem ipsum dolor sit amet, consectetur adipiscing elit. 1234567890",
	};

	// Fix the first display
	int  zxid = SECTOR_ZERO;
	int  lid  = 0;

	// This demo never returns
	while (true) {
		char  fname[64] = {0} ;  // longest font name is 30-something chars

		// Poor entropy, but better than nothing I guess :/
		// We need to do this because using CLR_RND with the scroller breaks the RNG
		srand(get_ms()*10000 +get_ms());

		// colour snow effect
		for (int i = 1;  i < 16;  i++) {
			clsr(CLR_RND);
			sleep_ms(16);
		}
		cls(CLR_BLK);

		fontSet(0, zxid);      // font slot 0
		snprintf(fname, sizeof(fname), "%s - ", zxorigins[zxid].name);  // append " - "

		// configure the font name scroller
		scrollerInit(1, &(scroller_t){
			.x     = 0,
			.y     = 8,           // top row
			.w     = grid.w,

			.s     = fname,       // font name
			.slot  = 0,           // font slot 0

			.ink   = clist[rand()%(clistCnt-1)],  // exclude black
			.paper = CLR_BLK,

			.ms    = 80,
			.loop  = true,
		});

		// configure the text scroller
		scrollerInit(0, &(scroller_t){
			.x     = 0,
			.y     = 0,           // bottom row
			.w     = grid.w,

			.s     = lines[lid],  // random text
			.slot  = 0,

			.ink   = clist[rand()%(clistCnt-2)],  // exclude rnd & black
			.paper = CLR_BLK,

			.ms    = 40,
			.loop  = false,
		});

		// Run the scrollers until the non-looping text finishes
		// scrollerStep() returns the number of active scrollers
		while (scrollerStep() > 1) {
			if (get_bootsel_button()) goto next ;  // scared yet?
			sleep_ms(1) ;                          // don't cook the 2040
		}
		// Stop the font name from looping and speed it up
		// There is an unresolved glitch if the
		//   last and first characters are both visible at this moment
		scroller(1)->loop = false;
		scroller(1)->ms   = 30;
		// run until the font-name text finishes
		while (scrollerStep() > 0)  sleep_ms(1) ;

next:
		srand(get_ms());  // we need to do this because CLR_RND breaks the RNG
		zxid = rand() % zxoCnt;
		lid  = rand() % ((sizeof lines)/sizeof(*lines));
	}
}

//+============================================================================
// randomSeed(analogRead(A0));  ...poor, but better than nothing I guess :/
//
void  demo_walk (const grbw_t _c,  const int ms)
{
	grbw_t*  pp = frame + (vbuf * grid.pcnt);  // start of active video buffer
	grbw_t   c  = dim(_c);

	cls(CLR_BLK);
	for (int i = 0;  i < grid.pcnt;  i++) {
		if (_c == CLR_RND)  c = dim(rand()) ;
		*pp++ = c;
		refresh();
		sleep_ms(ms);
	}
}

//+============================================================================
// Show off the basic palette
//
void  demo_primary (const uint ms)
{
	for (int c = 0;  c < clistCnt;  c++) {
		if (clist[c] == CLR_RND)  continue ;
		clsr(clist[c]);
		sleep_ms(ms);
	}
}

//+============================================================================
// Show the order of the LEDs
//
void  demo_steps (const grbw_t _c,  const int ms)
{
	// Steps
	for (int yy = 0;  yy < grid.h;  yy++) {
		for (int xx = 0;  xx < grid.w;  xx++) {
			grbw_t  c = (_c == CLR_RND) ? clist[rand()%(clistCnt-1)] : _c;
			plotr(xx,yy, c);
			sleep_ms(ms);
		}
	}
}

//+============================================================================
// Do an HSV rotation (or two)
// ...Needs to be moderately bright to have enough steps for it to look good.
//
void  demo_hrot (const int n)
{
	static const int  S = 1000;  // full colour saturation
	static const int  L =  500;  // brightest without using other LEDs

	for (int i = 0;  i < n;  i++) {         // iteration counter
		for (int H = 0;  H < 1000;  H++) {  // Hue rotation
			clsr(hsl2rgb(H,S,L));
			sleep_ms(3);
		}
	}
	clsr(CLR_BLK);
}

//+============================================================================
// pico-examples/pio/ws2812/ws2812.c   ...it's pretty!
//
void  demo_twinkle (const int ms)
{
	uint32_t  start = get_ms();

	cls(CLR_BLK);

	while (start + ms > get_ms())
		plotr( rand()%grid.w,  rand()%grid.h,
		       rand()%16 ? 0 : 0x01010202 * (rand()%20) );

	clsr(CLR_BLK);
}

//+============================================================================
// Render from specified buffer to the NEXT filter buffer
// leave 'fbuf' pointing at this new image
//
void  fx_dim (uint _from)
{
	uint32_t* from = frame + ( _from   * grid.pcnt);
	uint32_t* to   = frame + ((ebuf^1) * grid.pcnt);  // NEXT effects buffer

	for (int i = 0;  i < grid.pcnt;  i++, to++, from++)
		*to = dim(*from);

	ebuf ^= 1;  // set the "next" buffer to be the active buffer
}

//+============================================================================
// Fade to black, then "fade in to 100%, and back to black" 'cnt' times
//
// That 100% is 100% of its original brightness, NOT 100% bright
// ie. this will NOT make pixels Brighter than their start point
//
void  fx_fader (uint b,  int ms)
{
	brightness(b);     // set brightness {0..100}
	fx_dim(vbuf);      // run the filter on the current image
	show(ebuf);        // show the filtered image

	if (ms)  sleep_ms(ms) ;
}

//+============================================================================
void  demo_fader (grbw_t c,  int cnt)
{
	int  b;

	cls(CLR_BLK);                                    // cls black
	demo_steps(c, 3);                                // draw steps

	for (b = bright;  b >= 0;  fx_fader(b--, 16)) ;  // fade to black

	for (int n = 0;  n < cnt;  n++) {                // repeat 'cnt' times
		for (b++;  b <= 100;  fx_fader(b++, 16)) ;   //   fade to 100%
		for (b--;  b >= 0;  fx_fader(b--, 16)) ;     //   fade to black (0%)
	}
}
