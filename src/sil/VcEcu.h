#ifndef VC_ECU_H
#define VC_ECU_H

#include "Ecu.h"
#include "formula_main_dbc.h"

using namespace ecusim;

class VcEcu: public Ecu {
    public:
        VcEcu(void);
        void tick(void);
        void injectCan(ecusim::CanMsg);
        void set(std::string, float);
        float get(std::string);
    private:
        formula_main_dbc_vc_shutdown_status_t shutdown_status;
        bool can_data_available;
};

#endif // VC_ECU_H