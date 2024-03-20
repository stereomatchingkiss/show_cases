from PyQt5.QtNetwork import QHostAddress
from PyQt5.QtWebSockets import QWebSocketServer
from PyQt5.QtWidgets import QApplication, QPushButton
from PyQt5.QtCore import QDir

import argparse
import json
import sys

from py_utils import *

ap = argparse.ArgumentParser()
ap.add_argument("--port", required=False, type=int, default=2230)

args = vars(ap.parse_args())

# Easier to shutdown the server if create it as a widget
class qt_base_many_similar_pose_estimation_server(QPushButton):
    def __init__(self, port : int = args["port"], parent = None):
        super(qt_base_many_similar_pose_estimation_server, self).__init__(parent)

        self.port = port
        self.server = QWebSocketServer("Simple websocket server", QWebSocketServer.NonSecureMode, self)
        self.socket = None

        self.jobj = None
        self.dpaths = None
        self.im_paths = None

        self.setText("Close similar pose estimation server")

        self.clicked.connect(self.close)

        if(self.server.listen(QHostAddress.Any, port)):
            print("listening on port: ", port)
            self.server.newConnection.connect(self.on_new_connection)

    def send_start_message(self):
        self.socket.sendTextMessage("start")

    def on_new_connection(self):

        print("socket connected")
        if(self.socket):
            print("This simple app only allowed one socket connection")
            return

        self.socket = self.server.nextPendingConnection()
        self.socket.disconnected.connect(self.socket_disconnected)
        self.socket.textMessageReceived.connect(self.received_text_message)

        self.socket.sendTextMessage("start")

    def is_valid_folder(self, folder_name) -> bool:
        return len(folder_name) > 0 and QDir(folder_name).exists()

    def get_data_paths(self, jobj : dict) -> list:
        dpath = []
        if "data_folder" in jobj and self.is_valid_folder(jobj["data_folder"]):
            for val in get_files(jobj["data_folder"]):
                dpath.append(val.split(".")[0])

        return dpath

    def get_im_paths(self, jobj : dict) -> list:
        if self.is_valid_folder(jobj["im_folder"]):
            return get_files(self.jobj["im_folder"])

        return []

    def send_next_data(self):
        if len(self.im_paths) > 0:
            im_folder = self.jobj["im_folder"]
            im_path = self.im_paths[0]
            im_base_name = im_path.split(".")[0]
            if im_base_name in self.dpaths:
                with open(self.jobj["data_folder"] + "/" + im_base_name + ".json", "r") as fin:
                    jcontent = json.load(fin)
                    self.socket.sendTextMessage(json.dumps(jcontent))
            else:
                im_full_path = "{}/{}".format(im_folder, im_path)
                print("im full path =", im_full_path)
                jcontent = {"im" : read_image_as_base64_string(im_full_path), "im_path" : im_full_path}
                self.socket.sendTextMessage(json.dumps(jcontent))

            self.im_paths.pop(0)

    def received_text_message(self, message : str):
        print("received text message = ", (message))
        if message != "next":
            self.jobj = json.loads(message)
            self.dpaths = self.get_data_paths(self.jobj)
            self.im_paths = self.get_im_paths(self.jobj)
            self.send_next_data()
        else:
           self.send_next_data()

    def socket_disconnected(self):
        print("socket disconnected")
        if self.socket:
            self.socket.deleteLater()
            self.socket = None

def main():
    app = QApplication(sys.argv)

    win = qt_base_many_similar_pose_estimation_server()
    win.resize(400, 400)
    win.show()

    sys.exit(app.exec_())

if __name__ == "__main__":
    main()