#include "CanLogger.h"
#include "BlfWriter.h"

CanLogger::CanLogger(char* fileName)
{
    this->currentTimeMs = 0;
    BlfWriter_create_log_file(fileName);
}

CanLogger::~CanLogger(void)
{
    BlfWriter_close_log_file();
}

void CanLogger::tick(void)
{
    this->currentTimeMs++;
}

void CanLogger::injectCan(ecusim::CanMsg msg)
{
    // log it!
    BlfWriter_log_message(msg.id, *(uint64_t*)&msg.data, msg.dlc, this->currentTimeMs);
}
        
void CanLogger::set(std::string, float)
{
    // no set options
}