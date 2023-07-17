#include <string>
#include <deque>

#include "VcHandle.h"

#include "CanBucket.h"
#include "CanLogger.h"
#include "VcEcu.h"
#include "Simulation.h"
#include "formula_main_dbc.h"

Simulation* sim;
VcEcu* vc;
CanLogger* logger;
CanBucket* bucket;

static std::deque<CanMsg>* emptied;
    
extern "C"{
void init(void)
{
    sim = new Simulation();
    vc = new VcEcu();
    bucket = new CanBucket();
    sim->add_ecu(vc);
    sim->add_ecu(bucket);
    emptied = new std::deque<CanMsg>;
}

void deinit(void)
{
    // delete vc;
    // delete sim;
    // delete bucket;
    // delete emptied;
}

void begin_logging(char* filename)
{
    logger = new CanLogger(filename);
    sim->add_ecu(logger);
}

void run_ms(int ms)
{
    sim->run_ms(ms);

    std::vector<CanMsg>* newMsgs = bucket->drain();

    // grab any new CAN messages
    for (auto iter = newMsgs->begin(); iter < newMsgs->end(); iter++){
        emptied->push_back(*iter);
    }

    
    // emptied->reserve(emptied->size() + distance(newMsgs->begin(),newMsgs->end()));
    // emptied->insert(emptied->end(),newMsgs->begin(),newMsgs->end());
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
    msg.id = FORMULA_MAIN_DBC_MCU_INTERNAL_STATES_FRAME_ID;
    msg.dlc = 8;
    
    struct formula_main_dbc_mcu_internal_states_t state_data;
    state_data.d1_vsm_state = state;
    state_data.d6_inverter_enable_state = enabled;
    formula_main_dbc_mcu_internal_states_pack((uint8_t*)msg.data, &state_data, 8);
    vc->injectCan(msg);
}

void inject_pbx_status_msg(bool pumps_on)
{
    ecusim::CanMsg msg;
    msg.id = FORMULA_MAIN_DBC_PBX_STATUS_FRAME_ID;
    msg.dlc = 8;

    struct formula_main_dbc_pbx_status_t pbx_status;
    pbx_status.pbx_status_pump_on = pumps_on;
    formula_main_dbc_pbx_status_pack((uint8_t*)msg.data, &pbx_status, 8);
    vc->injectCan(msg);
}

unsigned long int next_can_msg(int64_t* data)
{
    if (!emptied->empty())
    {
        CanMsg msg = emptied->front();
        emptied->pop_front();

        *data = *(uint64_t*)msg.data;
        return msg.id;
    }

    return -1;
}

}