/**
 * Test of SIL features without python interface to catch any errors
 * involved with the C++ library.
 */

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

    sim.run_ms(1000000);

    return 0;
}