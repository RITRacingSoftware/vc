#ifndef VC_ECU_H
#define VC_ECU_H

#include "Ecu.h"
#include "main_bus.h"

using namespace ecusim;

class VcEcu: public Ecu {
    public:
        VcEcu(void);
        void tick(void);
        void injectCan(ecusim::CanMsg);
        void set(std::string, float);
        float get(std::string);
    private:
        main_bus_vc_shutdown_status_t shutdown_status;
};

#endif // VC_ECU_H