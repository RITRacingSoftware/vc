#include "unity.h"
#include "MockCAN.h"

// need to define this here since CMock isn't smart enough to define it in MockCAN.h
CAN_BUS can_bus;

// no need tbh