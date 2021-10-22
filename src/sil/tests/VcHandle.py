import ctypes
import cantools
import pathlib

"""
Python Wrapping of VcHandle.c functions.
"""

BLF_OUT_DIR = pathlib.Path("/vc/build/sil_test_blfs/")

class VcHandle:
    def __init__(self):
        self.can_db = cantools.database.load_file('/vc/src/app/main_bus/main_bus.dbc')
        self.signals = {}
        self.handle = ctypes.CDLL('/vc/build/g++/sil/tests/libVcHandle.so')
        self.handle.get.restype = ctypes.c_float # tell python "get" returns a float
        self.handle.init()

        # now set up inputs that won't cause a fault
        self['accela'] = 0.11
        self['accelb'] = 0.11
        self['brakep'] = 0.5

        self.inject_mc_state_msg(0, 0)


    
    def run_ms(self, ms):
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
    
    def inject_mc_state_msg(self, state, enabled):
        self.handle.inject_mc_state_msg(state, enabled)

    def __getitem__(self, key):
        return self.handle.get(ctypes.c_char_p(key.encode("utf-8")))
    
    def __setitem__(self, key, value):
        self.handle.set(ctypes.c_char_p(key.encode("utf-8")), ctypes.c_float(value))
