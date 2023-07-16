#ifndef SHUTDOWN_MONITOR_H
#define SHUTDOWN_MONITOR_H

#include <stdbool.h>
#include "CAN.h"

/**
 * Set initial internal states of shutdown lines to high, assuming the VC turns on after the shutdown circuit.
 */
void ShutdownMonitor_init(void);

/**
 * Update internal state of shutdown lines, send updated CAN message.
 */
void ShutdownMonitor_update(struct formula_main_dbc_vc_shutdown_status_t* new_status);

#endif // SHUTDOWN_MONITOR_H