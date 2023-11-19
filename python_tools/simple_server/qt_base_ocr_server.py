from PyQt5.QtNetwork import QHostAddress
from PyQt5.QtWebSockets import QWebSocketServer
from PyQt5.QtWidgets import QApplication, QMessageBox, QPushButton
from PyQt5.QtCore import QDir, QBuffer, QByteArray, QFile, QTextStream, QFileInfo, QIODevice
from PyQt5.QtGui import QImage, QFont

import argparse
import json
import sys

ap = argparse.ArgumentParser()
ap.add_argument("--downsample", required=False, type=int, default=1)
ap.add_argument("--port", required=False, type=int, default=1235)
ap.add_argument("--folder", type=str, required=True, help="Location of the folder contains image")
ap.add_argument("--save_at", type=str, required=False, default="", help="Save ocr results at, default value will be OcrTasksAssets/(args['folder'] base name)")

args = vars(ap.parse_args())

# Easier to shutdown the server if create it as a widget
class simple_websocket_server(QPushButton):
    def __init__(self, port : int = args["port"], parent = None):
        super(simple_websocket_server, self).__init__(parent)

        self.camera = None
        self.downsample = args["downsample"]
        self.images_to_send = None
        self.port = port
        self.send_file_info = None
        self.server = QWebSocketServer("OCR websocket server", QWebSocketServer.NonSecureMode, self)
        self.socket = None

        font = QFont()
        font.setPixelSize(50)
        self.setFont(font)

        if(len(args["save_at"])) == 0:
            self.save_at = "OcrTasksAssets/" + QFileInfo(args["folder"]).baseName()
        else:
            self.save_at = args["save_at"]

        QDir().mkpath(self.save_at)


        self.setText("Close ocr server")
        self.clicked.connect(self.close)

        dir = QDir(args["folder"])
        dir.setFilter(QDir.Files);

        filters = ["*.jpg", "*.jpeg", "*.bmp", "*.png", "*.tiff", "*.webp"]
        dir.setNameFilters(filters)
        self.images_to_send = dir.entryInfoList()
        print("images to send len = {}".format(len(self.images_to_send)))

        if(len(self.images_to_send) == 0):
            QMessageBox.information(self, "Info", "Cannot find any images")

        if(self.server.listen(QHostAddress.Any, port)):
            print("Ocr server listening on port: ", port)
            self.server.newConnection.connect(self.on_new_connection)

    def on_new_connection(self):

        print("socket connected")
        if(self.socket):
            print("This simple app only allowed one socket connection")
            return

        self.socket = self.server.nextPendingConnection()
        self.socket.disconnected.connect(self.socket_disconnected)
        self.socket.textMessageReceived.connect(self.received_text_message)

        self.send_frame()

    def received_text_message(self, message : str):
        print("received text message : {}, msg len = {}".format(self.send_file_info.baseName(), len(message)))
        try:
            jobj = json.loads(message)
            if jobj:
                qfile = QFile()
                qfile.setFileName((self.save_at + "/{}.json").format(self.send_file_info.baseName()))
                if(qfile.open(QIODevice.WriteOnly)):
                    qstream = QTextStream(qfile)
                    qstream.setCodec("UTF-8")
                    qstream << json.dumps(jobj, ensure_ascii=False, indent=1).encode('utf8')
            else:
                print("cannot convert {} to json".format(self.send_file_info.fileName()))
        except:
            print("cannot save alert to json by text message")

        self.send_frame()

    def send_frame(self):
        try:
            if(len(self.images_to_send) > 0):
                self.send_file_info = self.images_to_send.pop()
                print("send file = {}".format(self.send_file_info.absoluteFilePath()))
                img = QImage(self.send_file_info.absoluteFilePath())
                if self.socket and img.isNull() == False:
                    print("can send img {}".format(self.send_file_info.fileName()))
                    ba = QByteArray()
                    buffer = QBuffer(ba)
                    buffer.open(QIODevice.WriteOnly)
                    print("open buffer")

                    if args["downsample"] != 1:
                        img = img.scaled(int(img.width() / self.downsample), int(img.height() / self.downsample))

                    img.save(buffer, "jpg")
                    print("send binary image")
                    self.socket.sendBinaryMessage(ba)
                else:
                    if img.isNull():
                        print("cannot open image {}".format(self.send_file_info.absoluteFilePath()))
                    self.send_frame()
            else:
                print("Job done")

        except:
            print("send frame exception occur")

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