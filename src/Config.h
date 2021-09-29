#ifndef VC_CONFIG_H
#define VC_CONFIG_H

#define CAN_TX_QUEUE_LEN 15

/**
 * Faults
 */

// any bits set in this won't get set in the fault vector
#define DISABLE_FAULT_MASK 0

/**
 * APPS
 */

// How different the accelerator pedal positon sensors can read for a disagreement to be determined
#define APPS_PEDAL_DISAGREEMENT_PERCENTAGE 10
// How long the accelerator position sensors can disagree for before causing a fault
#define APPS_PEDAL_DISAGREEMENT_TIMEOUT_MS 100
// How long the accelerator position sensors must agree after a disagreement fault before the fault clears
#define APPS_PEDAL_DISAGREEMENT_RECOVERY_MS 100

// How far the brake must be pressed while also pressing the accelerator to trigger a double pedal fault
#define DOUBLE_PEDAL_BRAKE_POS_THRESHOLD 25
// How far the brake must be released to to clear a double pedal fault
#define DOUBLE_PEDAL_BRAKE_POS_RECOVERY_THRESHOLD 5

/**
 * ADC
 */

#define ADC_MAX_VAL 4095 // 12 bit adc
#define ADC_MAX_V 3.3

/**
 * Brake Pressure Sensor
 */

#define BPS_MIN_V 0.5
#define BPS_RANGE_V 4.0
#define BPS_MAX_V 5.0 // the sensor still only goes to 4.5v, if it reads up here itll be irrational

#define BPS_MIN_PSI 50
#define BPS_RANGE_PSI 7950


#endif // VC_CONFIG_H