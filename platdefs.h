#ifndef PLATDEFS_H
#define PLATDEFS_H

#include <stdlib.h>
#include <time.h>

static inline unsigned unif_random(unsigned n) {
    static int seeded = 0;

    if(!seeded) {
        srand((unsigned int)time(NULL));
        seeded = 1;
    }

    return rand() % n;
}

#endif
