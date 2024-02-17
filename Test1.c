#include <stdio.h>
#include <stdint.h>


void junkan_shift_left(uint32_t* state, uint8_t target){
    *state = *state << target | (*state >> (32 - target));
}

int main(int argc, char const *argv[])
{
    uint32_t test = 0x12345678;
    uint32_t test1  = test;
    junkan_shift_left(&test1, 8);
    printf("TEST = %x\nJUNKAN_8 = %x\n", test, test1);
}
