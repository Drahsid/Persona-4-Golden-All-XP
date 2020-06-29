#pragma once

#ifndef CHARACTER_H
#define CHARACTER_H

#include <inttypes.h>

// These are probably not exactly right lol

enum eCharacterIndex {
    Yosuke = 0,
    Chie,
    Yukiko,
    Rise,
    Kanji,
    Naoto,
    Teddie
};

typedef struct {
    int16_t hp;
    int16_t sp;
    uint8_t unk_0x04[0x50];
    int xp;
    uint8_t unk_0x44[0x2C];
} character_t;

typedef struct {
    int16_t hp;
    int16_t sp;
    uint8_t unk_0x04[0x3C];
    int xp;
    uint8_t unk_0x3A[0x48];
} protag_t;


void character_t_add_xp(character_t* character, int rhs, int index) {
    character->xp += rhs;

    switch (index)
    {
    case eCharacterIndex::Yosuke:
        printf("yosuke received %d xp!\n", rhs);
        break;
    case eCharacterIndex::Chie:
        printf("chie received %d xp!\n", rhs);
        break;
    case eCharacterIndex::Yukiko:
        printf("yukiko received %d xp!\n", rhs);
        break;
    case eCharacterIndex::Rise:
        printf("rise received %d xp!\n", rhs);
        break;
    case eCharacterIndex::Kanji:
        printf("kanji received %d xp!\n", rhs);
        break;
    case eCharacterIndex::Naoto:
        printf("naoto received %d xp!\n", rhs);
        break;
    case eCharacterIndex::Teddie:
        printf("teddie received %d xp!\n", rhs);
        break;
    default:
        break;
    }
}


#endif

