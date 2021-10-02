#ifndef TORQUE_LIMITER_H
#define TORQUE_LIMITER_H

/**
 * Take a commanded torque as input, return the limited version depending on vehicle state.
 */
float TorqueLimiter_apply_limit(float commanded_torque);


#endif // TORQUE_LIMITER_H