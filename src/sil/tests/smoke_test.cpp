/**
 * Test of SIL features without python interface to catch any errors
 * involved with the C++ library.
 */

#include <stdio.h>

#include "VcEcu.h"
#include "CanLogger.h"
#include "Simulation.h"

int main(void)
{
    Simulation sim;
    VcEcu vc;
    char logFileName[] = "logFile.blf";
    CanLogger canLogger(logFileName);

    sim.add_ecu(&vc);
    sim.add_ecu(&canLogger);

    for (int i = 0; i < 10000; i++)
    {
        vc.set("accela", (((float)i)/100)*3.3);
        vc.set("accelb", (((float)(100-i))/100)*1.8);
        vc.set("brakep", (((float)i)/100)*3.3);
        printf("STATUS_LED: %d\r\n", (int)vc.get("led"));

        sim.run_ms(1);
    }

    return 0;
}