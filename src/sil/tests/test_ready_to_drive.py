from VcHandle import VcHandle

def test_ready_to_drive():
    vc = VcHandle()
    for i in range(0,1000):
        vc.run_ms(1)
        print("hi")
        print(vc['led'])