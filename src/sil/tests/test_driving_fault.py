import pathlib
from VcHandle import vc, BLF_OUT_DIR


def test_mc_lost(vc):
    # run for a bit and make sure we start out with no faults
    vc.run_ms(10000)
    assert vc.signals['VcFaultVector_BRAKE_SENSOR_IRRATIONAL'] == 0
    assert vc.signals['VcFaultVector_APPS_SENSOR_DISAGREEMENT'] == 0
    assert vc.signals['VcFaultVector_APPS_DOUBLE_PEDAL'] == 0

    # run until the VC starts commanding torque
    vc.hold_mc_state(state=5, enabled=1)
    vc.run_ms(10000)

    # press the pedal, make sure we get torque commanded
    vc.acc_press(50)
    vc.run_ms(20)

    assert vc.signals['Torque_Command'] > 0

    # run a bit like dat
    vc.run_ms(10000)

    # make sure we still got torque
    print(vc.signals['Torque_Command'])
    assert vc.signals['Torque_Command'] > 0

    # make the motor controller go MIA
    vc.stop_holding_mc_state()

    vc.run_ms(1000)
    print(vc.signals['Torque_Command'])
    assert vc.signals['Torque_Command'] == 0


def test_pedal_breakage(vc):
    vc.begin_logging(BLF_OUT_DIR / "test_pedal_breakage.blf")

    # run for a bit and make sure we start out with no faults
    vc.run_ms(10000)
    assert vc.signals['VcFaultVector_BRAKE_SENSOR_IRRATIONAL'] == 0
    assert vc.signals['VcFaultVector_APPS_SENSOR_DISAGREEMENT'] == 0
    assert vc.signals['VcFaultVector_APPS_DOUBLE_PEDAL'] == 0

    # run until the VC starts commanding torque
    vc.hold_mc_state(state=5, enabled=1)
    vc.run_ms(10000)

    # press the pedal, make sure we get torque commanded
    vc.acc_press(50)
    vc.run_ms(20)

    assert vc.signals['Torque_Command'] > 0

    # run a bit like dat
    vc.run_ms(10000)

    # make sure we still got torque
    print(vc.signals['Torque_Command'])
    assert vc.signals['Torque_Command'] > 0

    # make the pedal sensors not match, simulating a break in one or something
    vc['accela'] = 2.2
    vc['accelb'] = 0.12

    vc.run_ms(1000)
    print(vc.signals['Torque_Command'])
    assert vc.signals['Torque_Command'] == 0
    
    # make sure the correct fault was thrown
    assert vc.signals['VcFaultVector_BRAKE_SENSOR_IRRATIONAL'] == 0
    assert vc.signals['VcFaultVector_APPS_SENSOR_DISAGREEMENT'] == 1
    assert vc.signals['VcFaultVector_APPS_DOUBLE_PEDAL'] == 0


def test_double_pedal(vc):
    # vc.begin_logging(BLF_OUT_DIR / "test_pedal_b_irrational.blf")

    # run for a bit and make sure we start out with no faults
    vc.run_ms(10000)
    assert vc.signals['VcFaultVector_BRAKE_SENSOR_IRRATIONAL'] == 0
    assert vc.signals['VcFaultVector_APPS_SENSOR_DISAGREEMENT'] == 0
    assert vc.signals['VcFaultVector_APPS_DOUBLE_PEDAL'] == 0

    # run until the VC starts commanding torque
    vc.hold_mc_state(state=5, enabled=1)
    vc.run_ms(10000)

    # press the pedal, make sure we get torque commanded
    vc.acc_press(50)
    vc.run_ms(20)

    assert vc.signals['Torque_Command'] > 0

    # run a bit like dat
    vc.run_ms(10000)

    # make sure we still got torque
    print(vc.signals['Torque_Command'])
    assert vc.signals['Torque_Command'] > 0

    # simulate a brake press that's under the intentional brake press threshold
    vc['brakep'] = 0.5
    print("--------------SET BRAKE PRESSURE -------------------------")
    # should still be requesting torque
    vc.run_ms(10000)
    assert vc.signals['Torque_Command'] > 0

    # this press should trigger a double pedal fault
    vc['brakep'] = 1.1

    vc.run_ms(100)
    assert vc.signals['Torque_Command'] == 0
    
    # make sure the correct fault was thrown
    assert vc.signals['VcFaultVector_BRAKE_SENSOR_IRRATIONAL'] == 0
    assert vc.signals['VcFaultVector_APPS_SENSOR_DISAGREEMENT'] == 0
    assert vc.signals['VcFaultVector_APPS_DOUBLE_PEDAL'] == 1
