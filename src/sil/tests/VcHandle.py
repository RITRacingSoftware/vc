import ctypes
import cantools
import pathlib
import pytest

"""
Python Wrapping of VcHandle.c functions.

** Paths here must be relative to vc for builds to work locally and on jenkins.
"""

BLF_OUT_DIR = pathlib.Path("build/sil_test_blfs/")

class VcHandle:
    def __init__(self):
        self.can_db = cantools.database.load_file('src/app/main_bus/main_bus.dbc')
        self.signals = {}
        self.handle = ctypes.CDLL('build/g++/sil/tests/libVcHandle.so')
        self.handle.get.restype = ctypes.c_float # tell python "get" returns a float
        self.handle.init()

        # now set up inputs that won't cause a fault
        self['accela'] = 0.0
        self['accelb'] = 0.0
        self['brakep'] = 0.5

        # dont automatically send mc state messages
        self.holding_mc_state = False
        self.ms_since_last_mc_state = 0
    
    def __del__(self):
        self.handle.deinit()

    
    def run_ms(self, ms):
        if self.holding_mc_state:
            for i in range(0, ms):
                self.handle.run_ms(1)
                self.ms_since_last_mc_state += 1
                if self.ms_since_last_mc_state == 100:
                    self.inject_mc_state_msg(state=self.mc_state, enabled=self.mc_enabled)
                    self.ms_since_last_mc_state = 0
        else:
            self.handle.run_ms(ms)

        # get all the new CAN messages and update the signal database
        can_data = ctypes.c_int64()
        can_id = 0
        while(can_id != -1):
            can_id = self.handle.next_can_msg(ctypes.byref(can_data))
            if (can_id != -1):
                self.signals.update(self.can_db.decode_message(can_id & 0x1FFFFFFF, can_data.value.to_bytes(8, "little", signed=True)))

    def begin_logging(self, filePath):
        filePath.parent.mkdir(exist_ok=True)
        if not filePath.exists():
            filePath.touch()
        print(str(filePath))
        self.handle.begin_logging(ctypes.c_char_p(str(filePath).encode("utf-8")))
    
    def hold_mc_state(self, state, enabled):
        """
        Automatically send a MC internal states message with these parameters every 100ms
        """
        self.holding_mc_state = True
        self.mc_state = state
        self.mc_enabled = enabled

    def stop_holding_mc_state(self):
        self.holding_mc_state = False

    def inject_mc_state_msg(self, state, enabled):
        """
        Send a single motor controller internal states message to the VC.
        """
        self.handle.inject_mc_state_msg(state, enabled)

    def __getitem__(self, key):
        return self.handle.get(ctypes.c_char_p(key.encode("utf-8")))
    
    def __setitem__(self, key, value):
        self.handle.set(ctypes.c_char_p(key.encode("utf-8")), ctypes.c_float(value))

    # Helper functions 
    def acc_press(self, pos):
        """
        Press the accelerator. Assume both sensors are working perfectly.
        pos is accelerator position from 0-100
        """
        self['accela'] = (pos/100) * (1.6)
        self['accelb'] = (pos/100) * (1.55)

@pytest.fixture
def vc():
    return VcHandle()