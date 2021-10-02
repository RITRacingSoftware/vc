#ifndef SOUND_CONTROLLER_H
#define SOUND_CONTROLLER_H

/**
 * Interface with sound controller. Handles Digital input selection to play sounds.
 */

typedef enum {
    Sounds_READY_TO_DRIVE,
    Sounds_NUM // number of entries in enum
} Sounds_e;

/**
 * Build sound pin mapping.
 */
void SoundController_init(void);

/**
 * Assert a trigger to start playing a sound.
 * sounds [in] - sound selection
 */
void SoundController_play_sound(Sounds_e sound);

/**
 * Stop sound triggers that are active after they've been held for enough time.
 */
void SoundController_100Hz(void);

#endif // SOUND_CONTROLLER_H