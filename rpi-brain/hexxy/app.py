import sys
import requests
from flask import Flask, render_template, request, redirect

from protocol.min import MINTransportSerial, ThreadsafeTransportMINSerialHandler

import protocol.serial as Proto

app = Flask(__name__)
SERVO_COUNT = 24
MIN_PORT="/dev/serial0"
#MIN_PORT=None
min_handler = []

@app.route('/')
def index():
    #debug
    return render_template("index.html")

@app.route('/serialtest', methods=['POST'])
def serial_test():
    test = bytearray(b'thisisatest')
    print("about to send")
    min_handler.send_frame(1, test)
    return redirect("/", code=302)

@app.route('/calibration', methods=['GET','POST'])
def calibrate():
    errors = []
    servos = [2000, 1959, 1918, 1877, 1836, 1795, 1754, 1713, 1672, 1631, 1590, 1549, 1508, 1467, 1426, 1385, 1344,
              1303, 1262, 1221, 1180, 1139, 1098, 1057]
    if request.method == "POST":
        try:
            data = request.form
            Proto.calibrate_servos(data)
        except:
            print("error happened ):")

    return render_template("calibration.html", servo_count=SERVO_COUNT, servos=servos)

###################################################################
# Main shizzle
###################################################################
if __name__ == '__main__':
    print("Starting app")
    min_handler = ThreadsafeTransportMINSerialHandler(port=MIN_PORT)
    app.run(debug=True, host='0.0.0.0')
