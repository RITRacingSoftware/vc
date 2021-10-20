import pathlib
from VcHandle import VcHandle, BLF_OUT_DIR

"""
The VC should not play the ready to drive sound or allow torque until:
    -the motor controller is ready
    -the VC isn't faulted
"""
def test_ready_to_drive():
    vc = VcHandle()
    vc.begin_logging(BLF_OUT_DIR / "test_ready_to_drive.blf")
        
    # run for a bit and make sure we start out with no faults
    vc.run_ms(10000)

    assert vc.signals['VcFaultVector']