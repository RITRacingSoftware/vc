import pathlib
from VcHandle import vc, BLF_OUT_DIR

"""
Comment here for what the test is looking for
"""

def test_regen_braking(vc):
    """
    check if brake and acc pedel are press, = 0
    not pressed = min brake,
    check if torque is negative in brake_to_t
    """
    # run for a bit and make sure we start out with no faults
    vc.run_ms(10000)
    assert vc.signals['VcFaultVector_BRAKE_SENSOR_IRRATIONAL'] == 0
    assert vc.signals['VcFaultVector_APPS_SENSOR_DISAGREEMENT'] == 0
    assert vc.signals['VcFaultVector_APPS_DOUBLE_PEDAL'] == 0

    # check to see if both the brake and the acc are pressed the same, they return zero torque
    vc.acc_press(50)
    vc.brake_press(50)

    assert vc.signals['Torque_Command'] == 0

    vc.run(10000)

    print(vc.signals['Torque_Command'])
    assert vc.signals['Torque_Command'] == 0 

    vc.brake_press(50)
    assert vc.signals['Torque_Command'] < 0

