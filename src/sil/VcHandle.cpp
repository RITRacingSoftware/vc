#include <string>

#include "VcHandle.h"

#include "CanLogger.h"
#include "VcEcu.h"
#include "Simulation.h"
#include "main_bus.h"

Simulation* sim;
VcEcu* vc;
CanLogger* logger;
extern "C"{
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

void inject_mc_state_msg(int state, bool enabled)
{
    ecusim::CanMsg msg;
    msg.id = MAIN_BUS_M170_INTERNAL_STATES_FRAME_ID;
    msg.dlc = 8;
    
    struct main_bus_m170_internal_states_t state_data;
    state_data.d1_vsm_state = state;
    state_data.d6_inverter_enable_state = enabled;
    main_bus_m170_internal_states_pack((uint8_t*)msg.data, &state_data, 8);
    vc->injectCan(msg);
}

}