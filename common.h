#ifndef  COMMON_H_
#define  COMMON_H_

#include  "pico/stdlib.h"

typedef  unsigned int  uint_t;    // I hate typing "unsigned int", and it makes my code scruffy

static inline uint32_t  get_ms (void)  {  return to_ms_since_boot(get_absolute_time());  }

#endif //COMMON_H_
