import ctypes
import cantools

"""
Python Wrapping of VcHandle.c functions.
"""

class VcHandle:
    def __init__(self):
        self.can_db = cantools.database.load_file('/vc/src/app/main_bus/main_bus.dbc')
        self.signals = {}
        self.handle = ctypes.CDLL('/vc/build/g++/sil/tests/libVcHandle.so')
        self.handle.init()

    def begin_logging(self, filename):
        self.handle.begin_logging(filename)
    
    def __getitem__(self, key):
        return self.handle.get(key)
    
    def __setitem__(self, key, value):
        self.handle.set(key, value)
