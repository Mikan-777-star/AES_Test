#include <stdint.h>
#include <stdio.h>

float _____nextSeed;
void rand_set(long seed){
    _____nextSeed = ((seed & 0xffff))/(float)0xffff;
    printf("seed = %ld ,startSeed = %f",seed, _____nextSeed);
}

float rand_next(){
    _____nextSeed = 4*_____nextSeed *(1-_____nextSeed);
    return _____nextSeed;
}