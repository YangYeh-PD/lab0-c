#ifndef MT19937_64_H
#define MT19937_64_H
#pragma once

#include <stdint.h>

/* initializes mt[NN] with a seed */
void mt19937_init(uint64_t seed);

/* generates a random number on [0, 2^64-1]-interval */
uint64_t mt19937_rand(void);
#endif
