#ifndef CAN_LOGGER_H
#define CAN_LOGGER_H
#include "Ecu.h"

using namespace ecusim;

class CanLogger: public Ecu {
    public:
        CanLogger(char* fileName);
        ~CanLogger(void);
        void tick(void);
        void injectCan(ecusim::CanMsg);
        void set(std::string, float);
    
    private:
        uint64_t currentTimeMs;
};

#endif // CAN_LOGGER_H