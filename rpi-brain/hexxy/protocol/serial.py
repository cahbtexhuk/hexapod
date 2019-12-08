from threading import Thread
# from min import MINTransportSerial, ThreadsafeTransportMINSerialHandler
#
# def process_frames(frames: []):
#     for frame in frames:
#         print(frame.payload)
#
# def serial_listen(handler: ThreadsafeTransportMINSerialHandler):
#     frames = []
#     while True:
#         frames = handler.poll()
#         if len(frames) > 0:
#             x = Thread(target=process_frames, args=(frames,))
#             x.start()

def calibrate_servos(servos: []):
    return 1

def load_calibration():
    offsets = []
    # debug
    for x in range(24):
        offsets.append(1500)

    return offsets
