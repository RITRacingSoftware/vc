#ifndef SOUND_CONTROLLER_H
#define SOUND_CONTROLLER_H

/**
 * Interface with sound controller. Handles Digital input selection to play sounds.
 */

typedef enum {
    Sounds_READY_TO_DRIVE
} Sounds_e;

/**
 * Play a specific sound once.
 * sounds [in] - sound selection
 */
void SoundController_play_sound(Sounds_e sound);

#endif // SOUND_CONTROLLER_H