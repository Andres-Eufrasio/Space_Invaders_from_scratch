#include "sound.h"

#include <stdio.h>
#include <string.h>

#define SAMPLE_RATE 44100
#define CHANNELS 2
#define MAX_SOUNDS 32

static SDL_AudioDeviceID audio_device;

static Sound *playing_sounds[MAX_SOUNDS];


/*
 * SDL calls this function whenever it needs
 * more audio data.
 */
static void audio_callback(
    void *userdata,
    Uint8 *stream,
    int len)
{
    /*
     * Start with silence.
     */
    SDL_memset(stream, 0, len);

    Sint16 *output = (Sint16 *)stream;

    int output_samples =
        len / sizeof(Sint16);


    /*
     * Mix every currently playing sound
     * into the output buffer.
     */
    for (int s = 0; s < MAX_SOUNDS; s++)
    {
        Sound *sound = playing_sounds[s];

        if (sound == NULL)
            continue;


        Sint16 *input =
            (Sint16 *)(sound->buffer + sound->position);


        int remaining =
            sound->length - sound->position;


        int input_samples =
            remaining / sizeof(Sint16);


        int samples =
            input_samples;


        if (samples > output_samples)
            samples = output_samples;


        /*
         * Add this sound to the output.
         */
        for (int i = 0; i < samples; i++)
        {
            int mixed =
                output[i] + input[i];


            /*
             * Clamp the value so we don't
             * overflow a Sint16.
             */
            if (mixed > 32767)
                mixed = 32767;

            if (mixed < -32768)
                mixed = -32768;


            output[i] = mixed;
        }


        /*
         * Move forward in the sound.
         */
        sound->position +=
            samples * sizeof(Sint16);


        /*
         * Has the sound finished?
         */
        if (sound->position >= sound->length)
        {
            sound->position = 0;
            sound->playing = 0;

            playing_sounds[s] = NULL;
        }
    }
}


int sound_init(void)
{
    SDL_AudioSpec spec;

    SDL_zero(spec);

    spec.freq = SAMPLE_RATE;
    spec.format = AUDIO_S16SYS;
    spec.channels = CHANNELS;
    spec.samples = 1024;

    spec.callback = audio_callback;
    spec.userdata = NULL;


    audio_device =
        SDL_OpenAudioDevice(
            NULL,
            0,
            &spec,
            NULL,
            0
        );


    if (audio_device == 0)
    {
        printf(
            "Could not open audio: %s\n",
            SDL_GetError()
        );

        return 0;
    }


    /*
     * Start the audio device.
     */
    SDL_PauseAudioDevice(
        audio_device,
        0
    );


    return 1;
}


int sound_load(
    Sound *sound,
    const char *filename)
{
    SDL_zero(*sound);


    if (SDL_LoadWAV(
            filename,
            &sound->spec,
            &sound->buffer,
            &sound->length
        ) == NULL)
    {
        printf(
            "Could not load %s: %s\n",
            filename,
            SDL_GetError()
        );

        return 0;
    }


    /*
     * For now we require all sounds
     * to use the same format as our mixer.
     */
    if (sound->spec.freq != SAMPLE_RATE ||
        sound->spec.format != AUDIO_S16SYS ||
        sound->spec.channels != CHANNELS)
    {
        printf(
            "%s must be:\n"
            "  44100 Hz\n"
            "  16-bit\n"
            "  stereo\n",
            filename
        );

        SDL_FreeWAV(sound->buffer);

        sound->buffer = NULL;

        return 0;
    }


    return 1;
}


void sound_play(Sound *sound)
{
    /*
     * Stop here if the sound wasn't loaded.
     */
    if (sound == NULL ||
        sound->buffer == NULL)
    {
        return;
    }


    /*
     * The audio callback runs on another thread,
     * so protect the playing_sounds array.
     */
    SDL_LockAudioDevice(audio_device);


    /*
     * Find an available channel.
     */
    for (int i = 0; i < MAX_SOUNDS; i++)
    {
        if (playing_sounds[i] == NULL)
        {
            sound->position = 0;
            sound->playing = 1;

            playing_sounds[i] = sound;

            break;
        }
    }


    SDL_UnlockAudioDevice(audio_device);
}


void sound_free(Sound *sound)
{
    if (sound == NULL)
        return;


    /*
     * Make sure the callback isn't
     * currently using this sound.
     */
    SDL_LockAudioDevice(audio_device);


    for (int i = 0; i < MAX_SOUNDS; i++)
    {
        if (playing_sounds[i] == sound)
        {
            playing_sounds[i] = NULL;
        }
    }


    sound->playing = 0;


    SDL_UnlockAudioDevice(audio_device);


    if (sound->buffer != NULL)
    {
        SDL_FreeWAV(sound->buffer);

        sound->buffer = NULL;
    }


    sound->length = 0;
}


void sound_quit(void)
{
    if (audio_device != 0)
    {
        SDL_CloseAudioDevice(
            audio_device
        );

        audio_device = 0;
    }
}