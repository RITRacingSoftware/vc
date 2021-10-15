#include <string>

#include "VcHandle.h"

#include "CanLogger.h"
#include "VcEcu.h"
#include "Simulation.h"

Simulation* sim;
VcEcu* vc;
CanLogger* logger;

void init(void)
{
    sim = new Simulation();
    vc = new VcEcu();
    sim->add_ecu(vc);
}

void begin_logging(char* filename)
{
    logger = new CanLogger(filename);
    sim->add_ecu(logger);
}

void run_ms(int ms)
{
    sim->run_ms(ms);
}

void set(char* key, float val)
{
    vc->set(std::string(key), val);
}

float get(char* key)
{
    return vc->get(std::string(key));
}