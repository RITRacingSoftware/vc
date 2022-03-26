import pathlib
from VcHandle import vc, BLF_OUT_DIR

"""
The VC should not play the ready to drive sound or allow torque until:
    -the motor controller is ready
    -the VC isn't faulted
"""
def test_ready_to_drive(vc):
    # vc.begin_logging(BLF_OUT_DIR / "test_ready_to_drive.blf")
    
    # run for a bit and make sure we start out with no faults
    vc.run_ms(10000)
    assert vc.signals['VcFaultVector_BRAKE_SENSOR_IRRATIONAL'] == 0
    assert vc.signals['VcFaultVector_APPS_SENSOR_DISAGREEMENT'] == 0
    assert vc.signals['VcFaultVector_APPS_DOUBLE_PEDAL'] == 0

    # simulate a motor controller power on by sending status messages to the VC
    # should see Inverter_Enable flip to 0 then back to 1 to unlock then enable the MC
    vc.hold_mc_state(state=0, enabled=0)
    vc.run_ms(1000)
    # make sure we are unlocking
    assert vc.signals['Inverter_Enable'] == 0

    # press brake pedal and rtd button
    vc['rtd'] = 1
    vc['brakep'] = 1.1

    # wait for enable to go high
    while vc.signals['Inverter_Enable'] == 0:
        vc.run_ms(10)

    # no torque should be commanded yet
    assert vc.signals['Torque_Command'] == 0
    
    # simulate motor controller becoming enabled
    vc.hold_mc_state(state=0,enabled=1)
    # release brake
    vc['brakep'] = 0.5

    # VC should know it has enabled MC at this point, but should still not request torque
    vc.run_ms(10000)
    assert vc.signals['Torque_Command'] == 0

    # indicate the motor controller is ready
    vc.hold_mc_state(state=5, enabled=1)

    # MC is ready but pumps are not yet running
    vc.run_ms(10000)
    assert vc.signals['Torque_Command'] == 0
    
    vc.inject_pbx_status_msg(True)

    # ready to drive sound should be triggered
    while vc['sound0'] == 1:
        vc.run_ms(1)
    
    # torque should still not be requested yet since we havent pressed the pedal
    vc.run_ms(10000)
    assert vc.signals['Torque_Command'] == 0

    vc.acc_press(50)
    vc.run_ms(1000)
    assert vc.signals['VcFaultVector_BRAKE_SENSOR_IRRATIONAL'] == 0
    assert vc.signals['VcFaultVector_APPS_SENSOR_DISAGREEMENT'] == 0
    assert vc.signals['VcFaultVector_APPS_DOUBLE_PEDAL'] == 0
    assert vc.signals['Torque_Command'] > 0