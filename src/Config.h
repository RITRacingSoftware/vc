#ifndef VC_CONFIG_H
#define VC_CONFIG_H

#define CAN_TX_QUEUE_LEN 15

/**
 * Faults
 */

// any bits set in this won't get set in the fault vector
#define DISABLE_FAULT_MASK 0

/**
 * Motor Controller Interface
 */

// This value is what the motor controller is configured with.
// Should be the maximum possible legal torque the vehicle is capable of producing.
#define ABSOLUTE_MAX_TORQUE_N 60

// How long the VC must go without receiving a MC status message before declaring the motor controller MIA
#define MC_CAN_TIMEOUT_MS 300 

// If the motor controller isn't unlocked on the first attempt, this is how long to wait before trying again.
#define UNLOCK_ATTEMPT_TIMEOUT_MS 4000

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
#define DOUBLE_PEDAL_APS_THRESHOLD 25
// How far the brake must be released to to clear a double pedal fault
#define DOUBLE_PEDAL_APS_RECOVERY_THRESHOLD 5

// Pressure indicating the driver has intentional force applied to the brake pedal.
#define BRAKE_ON_PSI 1000

/**
 * CAN
 */

// How many message ids can be counted at once.
#define NUM_ID_COUNTERS 5

/**
 * ADC
 */

#define ADC_MAX_VAL 4095.0 // 12 bit adc
#define ADC_MAX_V 3.3

/**
 * Accelerator Position Sensors
 */

#define APS_A_SENSOR_V 3.3
#define APS_A_MIN_RATIONAL_V 0.1
#define APS_A_MAX_RATIONAL_V 3.2
#define APS_A_RANGE_V ((APS_A_MAX_RATIONAL_V) - (APS_A_MIN_RATIONAL_V))

#define APS_B_SENSOR_V 1.8
#define APS_B_MIN_RATIONAL_V 0.1
#define APS_B_MAX_RATIONAL_V 1.7
#define APS_B_RANGE_V ((APS_B_MAX_RATIONAL_V) - (APS_B_MIN_RATIONAL_V))


/**
 * Brake Pressure Sensor
 */

#define BPS_MIN_V 0.5
#define BPS_RANGE_V 4.0
#define BPS_MAX_V 5.0 // the sensor still only goes to 4.5v, if it reads up here itll be irrational

#define BPS_MIN_PSI 50
#define BPS_RANGE_PSI 7950


#endif // VC_CONFIG_H