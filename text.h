#ifndef  TEXT_H_
#define  TEXT_H_

#include "banner.h"

//----------------------------------------------------------------------------- ---------------------------------------
typedef
	struct scroller {
		// user data
		int          x, y;    // bottom-left
		int          w;       // width
		const char*  s;       // text
		int          slot;    // font slot
		grbw_t       ink;
		grbw_t       paper;
		int          ms;      // delay betweem frames
		bool         loop;    // does the text loop

		// state data
		int          run;     // 1=running
		uint32_t     lastms;  // last animation time
		const char*  sp;      // active start point (in string)
		const char*  cp;      // active character
		int          ax;      // active start coordinate
	}
scroller_t;

//----------------------------------------------------------------------------- ---------------------------------------
#define      SCROLLER_MAX (2)
int          scrollerInit (const int n,  const scroller_t* const ps) ;
int          scrollerStep (void) ;
scroller_t*  scroller     (uint id) ;

//----------------------------------------------------------------------------- ---------------------------------------
int   fontSet  (const uint slot,  const uint zxoID) ;

int   chipfat  (const int x,     const int y,             char   ch,
                const int slot,        grbw_t ink,  const grbw_t paper ) ;

void  stripwat ( const int    _x,   const int _y,        const int _w,
                 const char*  s,    const int slot,
                 const grbw_t ink,  const grbw_t paper,  const int ms ) ;

#endif //TEXT_H_
