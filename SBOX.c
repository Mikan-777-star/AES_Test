#include <stdio.h>
#include <stdint.h>
#include <time.h>

#include "Random.h"

void swap(uint8_t* s1, uint8_t *s2){
    uint8_t temp = *s2;
    *s2 = *s1;
    *s1 = temp;
}

void dump(uint8_t* array, size_t len){
    printf("uint8_t SBOX[] = {");
    for(int i = 0; i < len; i++){
        printf("0x%hx,", array[i]);
    }
    printf("}\n");
}

int main(int argc, char const *argv[])
{
    uint8_t test[256];
    for (int i = 0; i < 256; i++)
    {
        test[i] = (uint8_t)(i & 0xff);      
    }
    rand_set((long)time(NULL));
    int p1, p2;
    for(size_t i = 0; i < 1000; i++){
        p1 = (int)(256 * rand_next());
        p2 = (int)(256 * rand_next());
        if(p1 == p2)continue;
        swap(&test[p1], &test[p2]);
    }
    dump(test, 256);    
    return 0;
}
