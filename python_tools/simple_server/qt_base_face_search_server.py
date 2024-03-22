from PyQt5.QtNetwork import QHostAddress
from PyQt5.QtWebSockets import QWebSocketServer
from PyQt5.QtWidgets import QApplication, QPushButton
from PyQt5.QtCore import QDirIterator, QDir, QFileInfo, QByteArray, QBuffer, QIODevice, QFile, QUuid
from PyQt5.QtGui import QImage, QFont

import argparse
import json
import sys

ap = argparse.ArgumentParser()
ap.add_argument("--port", required=False, type=int, default=1236)
ap.add_argument("--features_folder", type=str, required=True, help="The folder contain the features of faces")
ap.add_argument("--save_at", type=str, help="The folder you want to save the search results", default="face_search_results")
ap.add_argument("--search_folder", type=str, required=True, help="The folder you want to search the most similar faces")

args = vars(ap.parse_args())

# Easier to shutdown the server if create it as a widget
class qt_base_face_search_server(QPushButton):
    def __init__(self, port : int = args["port"], parent = None):
        super(qt_base_face_search_server, self).__init__(parent)

        self.features_folder = args["features_folder"]
        self.search_folder = args["search_folder"]
        self.server = QWebSocketServer("qt_base_face_search_server", QWebSocketServer.NonSecureMode, self)
        self.socket = None

        font = QFont()
        font.setPixelSize(25)
        self.setFont(font)

        self.last_image_to_search = None

        QDir().mkpath(args["save_at"])

        self.setText("Close qt_base_face_search_server")

        self.clicked.connect(self.close)

        if(self.server.listen(QHostAddress.Any, port)):
            print("Video capture server listening on port: ", port)
            self.server.newConnection.connect(self.on_new_connection)

        #self.search_file_location = self.get_files_location(self.search_folder)
        #self.send_image()

    def get_files_location(self, folder : str):
        it = QDirIterator(folder, QDir.Files | QDir.NoDotAndDotDot, QDirIterator.Subdirectories)
        file_locations = []
        while(it.hasNext()):
            file_locations.append(it.next())

        return file_locations

    def on_new_connection(self):

        print("socket connected")
        if(self.socket):
            print("This simple app only allowed one socket connection")
            return

        self.features_location = self.get_files_location(self.features_folder)
        self.search_file_location = self.get_files_location(self.search_folder)

        self.socket = self.server.nextPendingConnection()
        self.socket.disconnected.connect(self.socket_disconnected)
        self.socket.textMessageReceived.connect(self.received_text_message)

        self.send_data()

    def received_text_message(self, message : str):
        print("received text message")
        jobj = json.loads(message)
        mode = jobj["mode"]
        if(mode == "search"):
            bname = QFileInfo(self.last_image_to_search).baseName()
            fname = args["save_at"] + "/" + bname + ".json"
            if QFile(fname).exists():
                fname = args["save_at"] + "/" + QUuid().toString()
            with open(fname, 'w') as f:
                f.write(message)

        self.send_data()

    def send_features(self):
        if(len(self.features_location) > 0):
            loc = self.features_location.pop(0)

            with open(loc, 'r') as f:
                data = json.load(f)
                data_to_send = {}
                data_to_send["face_features"] = data["face_features"]
                data_to_send["mode"] = "register"
                data_to_send["face_name"] = QFileInfo(loc).baseName()
                self.socket.sendTextMessage(json.dumps(data_to_send))

    def send_image_by_socket(self, qimg):
        data_to_send = {}
        data_to_send["mode"] = "search"

        ba = QByteArray()
        bu = QBuffer(ba)
        bu.open(QIODevice.WriteOnly)
        qimg.save(bu, "JPG")

        im_string = str(ba.toBase64())
        data_to_send["img"] = im_string[1:]
        data_to_send["location"] = self.last_image_to_search

        self.socket.sendTextMessage(json.dumps(data_to_send))

    def send_image(self):
        if len(self.search_file_location) > 0:
            self.last_image_to_search = self.search_file_location.pop(0)
            print("send image = ", self.last_image_to_search)
            qimg = QImage(self.last_image_to_search)
            if(qimg.isNull()):
                print("cannot read img ", self.last_image_to_search)
                self.send_data()
            else:
                self.send_image_by_socket(qimg)

    def send_data(self):
        try:
           if(len(self.features_location) > 0):
               self.send_features()
           elif(len(self.search_file_location) > 0):
                self.send_image()
        except:
            print("All done")

    def socket_disconnected(self):
        print("socket disconnected")
        if self.socket:
            self.socket.deleteLater()
            self.socket = None

def main():
    app = QApplication(sys.argv)

    win = qt_base_face_search_server()
    win.resize(400, 400)
    win.show()

    sys.exit(app.exec_())

if __name__ == "__main__":
    main()