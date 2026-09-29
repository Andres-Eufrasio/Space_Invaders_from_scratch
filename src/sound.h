#ifndef SOUND_H
#define SOUND_H

#include <SDL2/SDL.h>

typedef struct
{
    Uint8 *buffer;
    Uint32 length;

    Uint32 position;
    int playing;

    SDL_AudioSpec spec;
} Sound;


int sound_init(void);

void sound_quit(void);

int sound_load(Sound *sound, const char *filename);

void sound_free(Sound *sound);

void sound_play(Sound *sound);

#endif