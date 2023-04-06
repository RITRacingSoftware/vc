#ifndef VC_CONFIG_H
#define VC_CONFIG_H

/**
 * Faults
 */

// any bits set in this won't get set in the fault vector
#define DISABLE_FAULT_MASK (0)

/**
 * Motor Controller Interface
 */

// Should be the maximum possible legal torque the vehicle is capable of producing.
// 0 means default to EEPROM value, currently 240 Nm
#define ABSOLUTE_MAX_TORQUE_NM 0

// How long the VC must go without receiving a MC status message before declaring the motor controller MIA
#define MC_CAN_TIMEOUT_MS 300 // message cycle time is 100ms so this means we missed three messages

// If the motor controller isn't unlocked on the first attempt, this is how long to wait before trying again.
#define UNLOCK_ATTEMPT_TIMEOUT_MS 500

/**
 * Torque Conversion (pedal position -> torque transfer function)
 */
#define MAX_TORQUE_NM 240.0

// the first and last % of the accelerator torque curve will be flat
// ex: if both are set to 5, torque will start being commanded at 5% pedal travel, and reach 100% pedal travel at 95%
#define ACC_LOWER_DEADZONE_PERCENT 5.0
#define ACC_UPPER_DEADZONE_PERCENT 5.0

/**
 * APPS
 */

// How different the accelerator pedal positon sensors can read for a disagreement to be determined
#define APPS_PEDAL_DISAGREEMENT_PERCENTAGE 10
// How long the accelerator position sensors can disagree for before causing a fault
#define APPS_PEDAL_DISAGREEMENT_TIMEOUT_MS 70
// How long the accelerator position sensors must agree after a disagreement fault before the fault clears
#define APPS_PEDAL_DISAGREEMENT_RECOVERY_MS 100
// How long the brake and accelerator pedal can read as pressed for before causing a fault
#define APPS_DOUBLE_PEDAL_TIMEOUT_MS 30

// How far the brake must be pressed while also pressing the accelerator to trigger a double pedal fault
#define DOUBLE_PEDAL_APS_THRESHOLD 25
// How far the brake must be released to to clear a double pedal fault
#define DOUBLE_PEDAL_APS_RECOVERY_THRESHOLD 5

// The maiximum number of samples in a row that the brake can be irrational before a fault is set
#define MAX_BRAKE_IRRATIONAL_COUNT 3

// Pressure indicating the driver has intentionally applied force to the brake pedal.
#define BRAKE_PRESSED_V 0.8

/**
 * Sound Control
 */

// How long a sound trigger is held.
#define SOUND_TRIGGER_HOLD_MS 2000

/**
 * CAN
 */

// How many message ids can be counted at once.
#define NUM_ID_COUNTERS 5

// How much space is allocated for messages in a CanQueue_s
#define CAN_QUEUE_NUM_MSGS 15

//How much space is allocated for messages in the can tx queue
#define CAN_TX_QUEUE_LEN 15

//How much space is allocated for messages in the can rx queue
#define CAN_RX_QUEUE_LEN 15

/**
 * ADC
 */

#define ADC_MAX_VAL 4095.0 // 12 bit adc
#define ADC_MAX_VOLTAGE 3.3

/**
 * Accelerator Position Sensors
 */

// to what discretion voltages must be equal to be considered equal
#define VOLTAGE_TOL 0.001

#define APS_A_SENSOR_V 3.2 //2.83
#define APS_A_OFFSET_V 0.28
#define APS_A_SENSOR_RANGE_V (APS_A_SENSOR_V - APS_A_OFFSET_V)
#define APS_A_IRRATIONAL_V 0.2 //Anything below this is considered irrational (open)

#define APS_B_SENSOR_V 1.5
#define APS_B_OFFSET_V 0.0 //0.16
#define APS_B_SENSOR_RANGE_V (APS_B_SENSOR_V - APS_B_OFFSET_V)
#define APS_B_IRRATIONAL_V 0.0


/**
 * Brake Pressure Sensor
 */

#define BPS_MIN_V 0.1
#define BPS_MAX_V 3.3 // the sensor still only goes to 4.5v, if it reads up here itll be irrational
#define BPS_IRRATIONAL_V 3.15
// #define BPS_RANGE_V (BPS_MAX_V - BPS_MIN_V)


#endif // VC_CONFIG_H scons sim
