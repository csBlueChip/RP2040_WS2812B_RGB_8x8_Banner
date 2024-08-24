#include  <stdbool.h>
#include  <stdint.h>
#include  <stdlib.h>
#include  <string.h>

#include  "banner.h"
#include  "text.h"
#include  "zxorigins/zxorigins.h"

//-----------------------------------------------------------------------------
#define  FONT_MAX  (2)

uint     font[FONT_MAX]       = {0};
uint8_t  chw[FONT_MAX][96][4] = {0};  // l, r, w, x

//+============================================================================
int  fontSet (const uint slot,  const uint zxoID)
{
	if (slot > FONT_MAX)  return -1 ;

	font[slot] = zxoID < zxoCnt ? zxoID : 0 ;

	// Build character masks for proportional spacing
	for (char ch = 1;  ch < 96;  ch++) {
		const uint8_t*  rp = &zxoFont(font[slot])[ch *8];

		uint8_t  left  = 0x80;
		uint8_t  right = 0x01;
		uint8_t  width = 0;

		// build width mask
		uint8_t  mask = 0x00;
		for (int i = 0;  i < 8;  i++)  mask |= *rp++ ;

		// find left
		while(left && !(left & mask))  left >>= 1 ;
		if (!left)  left = 0x80 ;

		// find right
		while(right && !(right & mask))  right <<= 1 ;
		right >>= 1;
		if (!right)  right = 0x01 ;

		// find width
		for (int m = left;  m != right;  m >>= 1)  width++ ;
		width++;

		chw[slot][ch][0] = left;
		chw[slot][ch][1] = right;
		chw[slot][ch][2] = width;
		chw[slot][ch][3] = 0;      // unused (for memory alignemnt)
	}

	// Space is same width as "[" [Damien Guard, font author]
	chw[slot][0][0] = chw[slot]['[' - ' '][0];
	chw[slot][0][1] = chw[slot]['[' - ' '][1] << 1;
	chw[slot][0][2] = chw[slot]['[' - ' '][2]  - 1;
	chw[slot][0][3] = chw[slot]['[' - ' '][3];

	return zxoID;
}

//+============================================================================
// CHaracter Ink Paper Font AT{x,y}
//
int  chipfat (const int    x,    const int    y,
                    char   ch,   const int    slot,
                    grbw_t ink,  const grbw_t paper )
{
	// translate echaracter offset      vvv--- unknown maps to <space>
	ch = ((ch < ' ') || (ch > 0x7F)) ? (' ' - ' ') : (ch - ' ') ;

	const uint8_t*  row   = &zxoFont(font[slot])[(ch) *8];
	const uint8_t   left  = chw[slot][ch][0];
	const uint8_t   right = chw[slot][ch][1];

	if (ink == CLR_RND)
		ink = RGB((rand()%64)+10, (rand()%64)+10, (rand()%64)+10);

	for (int yy = 7;  yy >= 0;  yy--, row++)
		for (int xx = 0,  mask = left;  mask >= right;  xx++,  mask >>= 1)
			plot(x+xx, y+yy, (*row & mask) ? ink : paper);

	return chw[slot][ch][2];
}

//+============================================================================
// This example h-scroller relies on the edges of the display for clipping
// If you want a box in the middle of the display, you're going to need to 
//   sort out the clipping at the box edges.
//
void  stripwat ( const int    _x,   const int _y,        const int _w,
                 const char*  s,    const int slot,
                       grbw_t ink,  const grbw_t paper,  const int ms )
{
	const char*  sp  = s;      // display from here in the string
	const char*  cp  = NULL;   // active character

	int    x   = _x + _w - 1;  // display from here on the display
	int    xx;                 // offset from x

	while (1) {
		int  w;

		for (xx = x,  cp = sp;  xx < grid.w;  xx += w) {
			if (xx == 0)  sp = cp,  x = 0 ;  // new char at screen-left

			if (ink == CLR_RND) {
				srand(cp[0] * cp[1]);
				ink = RGB(100+(rand()%156), 100+(rand()%156), 100+(rand()%156));
			}
			w = chipfat(xx,_y, *cp,slot, ink,paper);

			if (ms)  sleep_ms(ms) ;
			if (*cp)  cp++ ;
		}
		refresh();

		if (!*cp && cp==sp)  break ;
		x--;
	}
}

//-----------------------------------------------------------------------------
// ### ### ### ### #   #   ### ###
// #   #   # # # # #   #   #   # #
// ### #   ### # # #   #   ##  ###
//   # #   ##  # # #   #   #   ##
// ### ### # # ### ### ### ### # #
//-----------------------------------------------------------------------------

static scroller_t  sc[SCROLLER_MAX] = {0};

//+============================================================================
// Return pointer to chosen scroller
// So you can edit the parameters of a running scroller (speed, loop, etc)
//
scroller_t*  scroller (uint id)
{
	return (id < SCROLLER_MAX) ? &sc[id] : &sc[0];
}

//+============================================================================
// This example h-scroller relies on the edges of the display for clipping
// If you want a box in the middle of the display, you're going to need to 
//   sort out the clipping at the box edges.
//
int _scrollerStep (scroller_t*  ps)
{
	int     xx;  // active x-coordinate
	int     w;   // width of last character

	grbw_t  ink = ps->ink;  // ink colour (for RND)

	for (xx = ps->ax,  ps->cp = ps->sp;  xx < grid.w;  xx += w) {
		if (xx == 0)  ps->sp = ps->cp,  ps->ax = 0 ;  // new char @ screen-left

		if ( (ps->loop) && (!*ps->cp) )  ps->cp = ps->s ;  // looping

		// random colours
		if (ps->ink == CLR_RND) {
			srand(ps->cp[0] * ps->cp[1] * 1337);
			ink = RGB(20+(rand()%236), 20+(rand()%236), 20+(rand()%236));
		}

		w = chipfat(xx, ps->y,  *ps->cp, ps->slot,  ink, ps->paper);

		if (*ps->cp)  ps->cp++ ;  // advance to next char (unless at NUL)
	}

	// keep going until the NUL is the FIRST character being displayed
	if ((*ps->cp == '\0') && (ps->cp == ps->sp))  return (ps->run = 0) ;

	// move left...
	ps->ax--;

	return 1;
}

//+========================================================
// Update every scroller that is ready to advance
//
int  scrollerStep (void)
{
	uint32_t  now     = get_ms();
	int       running = 0;

	for (int n = 0;  n < SCROLLER_MAX;  n++) {
		if (sc[n].run) {
			if (now > sc[n].lastms + sc[n].ms) {
				sc[n].lastms  = now;
				running      += _scrollerStep(&sc[n]);
			} else   running += 1 ;
		}
	}
	refresh();

	return running;
}

//+============================================================================
// Initialiser a scroller
//
int  scrollerInit (const int n,  const scroller_t* const ps)
{
	memcpy(&sc[n], ps, sizeof(*sc));

	sc[n].sp  = sc[n].s;                // active start point (in string)
	sc[n].cp  = NULL;                   // active character
	sc[n].ax  = sc[n].x + sc[n].w - 1;  // active x-coordinate
	sc[n].run = 1;                      // start it running
}
