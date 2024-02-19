#include <stdint.h>
#include <string.h>

#include <stdio.h>

#define AES_BLOCK_SIZE 16
#define AES_ROUNDS 10
#define AES_KEY_LEN 16
#define WORD_LEN 4

const uint8_t SBOX[256] = {0x70,0x7c,0xa,0x2e,0x26,0xea,0xe,0x71,0x25,0xd6,0x24,0x8a,0x98,0x45,0x32,0xfd,0x2,0xc0,0xd4,0xc1,0xd5,0x83,0x93,0x5d,0x89,0x3e,0x12,0x7f,0x82,0x63,0x76,0x4,0x2d,0x8d,0x67,0x86,0x78,0x99,0x87,0x64,0xff,0x8b,0x2c,0x79,0x15,0x94,0xb,0xf6,0xb7,0x1a,0x50,0x9,0xfe,0x35,0xf,0x72,0x36,0x29,0xb5,0xb6,0xcd,0x10,0xbd,0xec,0x5e,0x1f,0xbe,0x81,0xae,0xed,0xa2,0xcb,0x9d,0x49,0x4b,0x22,0x88,0x6c,0x4e,0xdb,0xb9,0x52,0x16,0x6e,0xf8,0xd8,0x5b,0xac,0xa4,0xe8,0x4c,0x5c,0x57,0xb8,0x7a,0xc2,0xe5,0xc8,0xf3,0x9e,0xaa,0x7,0x69,0xdc,0x3c,0x97,0x0,0x6b,0xfc,0x6a,0x44,0x59,0xb4,0x34,0xdf,0x73,0x75,0x84,0xe6,0x1d,0x21,0xc7,0xf1,0xca,0x7e,0x7d,0x80,0xd,0x9a,0xd9,0x9f,0xb0,0x17,0x51,0x85,0x58,0x28,0xf9,0x77,0xa5,0x8e,0x2a,0xc,0xc5,0x11,0x2f,0x6d,0x92,0x9c,0x1e,0x96,0x66,0xa6,0x53,0xd2,0x48,0xdd,0x1,0x33,0xb3,0xef,0xba,0xa7,0x46,0xd7,0x61,0x5a,0x3,0x5,0x62,0xe3,0xd1,0xbb,0xe4,0xab,0xbc,0x38,0xb1,0x4d,0xa3,0x90,0x65,0x4a,0xcf,0x1b,0x91,0xa9,0xa0,0x3b,0xbf,0xa8,0xcc,0x13,0x3a,0xc9,0x3f,0x42,0xce,0xb2,0x74,0xf7,0x43,0x37,0x40,0xeb,0x31,0xc3,0xd0,0x9b,0x23,0x8f,0xfb,0x6,0x8c,0x3d,0xf0,0x27,0xc6,0x95,0x39,0x20,0xde,0x7b,0xad,0xaf,0x5f,0x54,0xe1,0x1c,0x19,0xe9,0xe7,0xe2,0x4f,0xfa,0x30,0xa1,0x60,0xc4,0xf2,0x14,0x56,0x41,0x2b,0xf5,0xda,0x68,0xe0,0xee,0x18,0xd3,0x47,0x8,0x6f,0x55,0xf4};


uint8_t Rcon[11] = {
    0x00, // Rcon[0] は使用されない
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36
    // 以降、必要な数だけRcon値を続ける
};
void RotWord(uint8_t word[WORD_LEN]){
    uint8_t temp = word[0];
    for(int i = 1; i < WORD_LEN; i++){
        word[i - 1] = word[i];
    }
    word[WORD_LEN - 1] = temp;
}

void SubWord(uint8_t word[WORD_LEN]){
    for (size_t i = 0; i < WORD_LEN; i++){
        word[i] = SBOX[word[i]];
    }
}

void KeyExpansion(const uint8_t* key, uint8_t roundKeys[]){
    uint8_t temp[WORD_LEN];
    for (size_t i = 0; i < AES_KEY_LEN; i++)
    {
        roundKeys[i] = key[i];
    }
    
    for(int i = 4; i < 4 * (AES_ROUNDS + 1); i++){
        for(int j = 0; j < WORD_LEN; j++){
            temp[j] = roundKeys[((i - 1)*4) + j ];
        }
        if(i % 4 == 0){
            printf("i = %d", i);
            RotWord(temp);
            SubWord(temp);
            temp[0] = temp[0] ^ Rcon[i / 4];
        }
        for(uint8_t j = 0; j < 4; j++){
            roundKeys[i * 4 + j] = roundKeys[(i - 4) * 4 + j] ^ temp[j];
        }
    }
}

void AddRoundKey(uint8_t* state, const uint8_t* roundKey){
    for(int i = 0; i < AES_BLOCK_SIZE; i++){
        state[i] = state[i] ^ roundKey[i]; 
    }
}

void junkan_shift_left(uint32_t* state, uint8_t target){
    *state = *state << target | (*state >> (32 - target));
}

void SubBytes(uint8_t* state){
    for(int i = 0; i < AES_BLOCK_SIZE; i++){
        state[i] = SBOX[state[i]];
    }
}

void ShiftRows(uint8_t* state){
    uint32_t* ans = (uint32_t*)state;
    for(int i = 3; i >= 0; i--){
        junkan_shift_left(&ans[i], i * 8);
    }
}

uint8_t gf_mul(uint8_t a, uint8_t b){
    uint32_t ap = a;
    uint32_t bp = b;
    return (ap * bp) & 0xff;
}

void MixColumns(uint8_t state[4][4]){
    uint8_t tmp[4][4];
    for (int c = 0; c < 4; c++) {
        tmp[0][c] = gf_mul(0x02, state[0][c]) ^ gf_mul(0x03, state[1][c]) ^ state[2][c] ^ state[3][c];
        tmp[1][c] = state[0][c] ^ gf_mul(0x02, state[1][c]) ^ gf_mul(0x03, state[2][c]) ^ state[3][c];
        tmp[2][c] = state[0][c] ^ state[1][c] ^ gf_mul(0x02, state[2][c]) ^ gf_mul(0x03, state[3][c]);
        tmp[3][c] = gf_mul(0x03, state[0][c]) ^ state[1][c] ^ state[2][c] ^ gf_mul(0x02, state[3][c]);
    }
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            state[r][c] = tmp[r][c];
        }
    }
}

void printRoundKeys(uint8_t roundkey[(AES_ROUNDS + 1) * AES_BLOCK_SIZE]){
    for(int i = 0; i < AES_ROUNDS; i++){
        for(int j = 0; j < 4; j++){
            for(int k = 0; k < 4; k++)printf("%hx,", roundkey[(i*16)+(j * 4) + k]);
            printf("\n");
        }
        printf("\n");
    }
}

void AES_Encrypt(const uint8_t* in, uint8_t* out, const uint8_t* key){
    uint8_t state[AES_BLOCK_SIZE];
    uint8_t roundKeys[(AES_ROUNDS + 1) * AES_BLOCK_SIZE];

    memcpy(state, in, AES_BLOCK_SIZE);

    KeyExpansion(key, roundKeys);

    printRoundKeys(roundKeys);

    AddRoundKey(state, roundKeys);
    
    for(int round = 0; round < AES_ROUNDS; round++){
        SubBytes(state);
        ShiftRows(state);
        MixColumns((unsigned char (*)[4])state);
        AddRoundKey(state, roundKeys + round * AES_BLOCK_SIZE);
    }

    SubBytes(state);
    ShiftRows(state);
    AddRoundKey(state, roundKeys + AES_ROUNDS * AES_BLOCK_SIZE);
    memcpy(out, state, AES_BLOCK_SIZE);
}

char* memorydump(char* buf, uint8_t* array, int len){
    for(int i = 0; i < len; i++){
        sprintf(buf + (i * 6), "0x%02hx, ", array[i]);
    }
    return buf;
}
char* textPrint(char* buf, uint8_t* text, int len){
    for(int i = 0; i < len; i++){
        buf[i] = text[i];
    }
    buf[len] = '\0';
    return buf;
}
// Example usage
int main() {
    uint8_t key[AES_KEY_LEN] = "1234567890laoed";
    uint8_t plaintext[AES_BLOCK_SIZE] = "Hello world AES";
    uint8_t ciphertext[AES_BLOCK_SIZE];

    char buf[1024];

    AES_Encrypt(plaintext, ciphertext, key);
    printf("text = %s\n", textPrint(buf, plaintext, AES_BLOCK_SIZE));
    printf("memory = %s\n", memorydump(buf, plaintext, AES_BLOCK_SIZE));
    printf("text = %s\n", textPrint(buf, ciphertext, AES_BLOCK_SIZE));
    printf("memory = %s\n", memorydump(buf, ciphertext, AES_BLOCK_SIZE));

    return 0;
}