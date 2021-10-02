#include <string.h>
#include "SoundController.h"

#include "Config.h"
#include "HAL_Dio.h"

// Used to determine which digital output corresponds to which sound.
DIOpin_e pin_map[Sounds_NUM];
// Keeps track of how long each trigger has been actuated.
unsigned int pin_durations_ms[Sounds_NUM];

void SoundController_init(void)
{
    // set up pin mapping
    pin_map[Sounds_READY_TO_DRIVE] = DIOpin_SOUND_0;

    // zero out pin low time timers
    for (int i = 0; i < Sounds_NUM; i++)
    {
        pin_durations_ms[i] = 0;
    }
}

void SoundController_play_sound(Sounds_e sound)
{
    HAL_Dio_write(pin_map[sound], false);
    // reset duration to keep this low for longer if its already low
    pin_durations_ms[sound] = 0;
}

/**
 * Stop sound triggers that are active after they've been held for enough time.
 */
void SoundController_100Hz(void)
{
    for (int sound = 0; sound < Sounds_NUM; sound++)
    {   
        // only care if output is low
        if (HAL_Dio_read(pin_map[sound] == false))
        {
            pin_durations_ms[sound] += 10;

            // if trigger has been low for long enough, set to high
            if (pin_durations_ms[sound] >= SOUND_TRIGGER_HOLD_MS)
            {
                HAL_Dio_write(sound, true);
                pin_durations_ms[sound] = 0;
            }
        }
    }
}