from PyQt5.QtNetwork import QHostAddress
from PyQt5.QtWebSockets import QWebSocketServer
from PyQt5.QtWidgets import QApplication, QWidget

import argparse
import base64
import cv2
import sys

ap = argparse.ArgumentParser()
ap.add_argument("--downsample", required=False, type=int, default=1)
ap.add_argument("--port", required=False, type=int, default=8000)
ap.add_argument("--opencv_url", type=str, required=True,
                help="Url of the video, could be local location of video, rtsp address, or enter number(ex : 0) to open webcam")

args = vars(ap.parse_args())

# Easier to shutdown the server if create it as a widget
class simple_websocket_server(QWidget):
    def __init__(self, port : int = args["port"], parent = None):
        super(simple_websocket_server, self).__init__(parent)

        self.downsample = args["downsample"]
        self.port = port
        self.server = QWebSocketServer("Simple websocket server", QWebSocketServer.NonSecureMode, self)
        self.socket = None

        if(self.server.listen(QHostAddress.Any, port)):
            print("SSL Echo Server listening on port: ", port)
            self.server.newConnection.connect(self.on_new_connection)

    def on_new_connection(self):

        print("socket connected")
        if(self.socket):
            print("This simple app only allowed one socket connection")
            return

        self.socket = self.server.nextPendingConnection()
        self.socket.disconnected.connect(self.socket_disconnected)

        self.send_frame()

    def send_frame(self):
        camera = cv2.VideoCapture(args["opencv_url"], )
        if(camera.isOpened() == False):
            print("cannot open camera from the url = ", args["opencv_url"])
            return

        while True:
            try:
                _, frame = camera.read()
                if frame is None:
                    print("cannot capture frame")
                    break
                if self.socket is None:
                    break

                if self.socket:
                    if args["downsample"] != 1:
                        height, width, _ = frame.shape
                        frame = cv2.resize(frame, (int(width / self.downsample), int(height / self.downsample)))

                    _, buffer = cv2.imencode('.jpg', frame)
                    self.socket.sendBinaryMessage(buffer.tobytes())

            except:
                camera.release()
                cv2.destroyAllWindows()
                break

    def socket_disconnected(self):
        print("socket disconnected")

        if self.socket:
            self.socket.deleteLater()
            self.socket = None

def main():
    app = QApplication(sys.argv)

    win = simple_websocket_server()
    win.resize(400, 400)
    win.show()

    sys.exit(app.exec_())

if __name__ == "__main__":
    main()