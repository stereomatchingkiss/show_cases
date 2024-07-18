from PyQt5.QtCore import QByteArray, QCoreApplication, QDateTime, QDir, QFile, QJsonDocument, QIODevice, QTextStream, QSettings
from PyQt5.QtNetwork import QHostAddress
from PyQt5.QtWebSockets import QWebSocketServer
from PyQt5.QtWidgets import QApplication, QGridLayout, QLabel, QLineEdit, QPushButton, QWidget, QSizePolicy, QSpacerItem, QVBoxLayout
from PyQt5.QtGui import QImage

from send_email import send_multipart_email

import argparse
import json
import sys

ap = argparse.ArgumentParser()
ap.add_argument("--port", required=False, type=int, default=2345)
ap.add_argument("--save_at", type=str, default="GenericCVTasksAssets/alert/fall_down_obj_det/", help="Location to save the alert")

args = vars(ap.parse_args())

# Easier to shutdown the server if create it as a widget
class simple_websocket_server(QWidget):
    def __init__(self, port : int = args["port"], parent = None):
        super(simple_websocket_server, self).__init__(parent)

        self.setWindowTitle("Fall down alert")
        self.port = port
        self.server = QWebSocketServer("Simple alert server", QWebSocketServer.NonSecureMode, self)
        self.socket = None

        self.glayout = QGridLayout()

        self.label_email = QLabel("Email address")
        self.label_password = QLabel("Password")
        self.label_send_to = QLabel("Send to")

        self.line_email = QLineEdit()
        self.line_password = QLineEdit()
        self.line_send_to = QLineEdit()
        self.line_password.setEchoMode(QLineEdit.Password)

        self.line_email.setPlaceholderText("aaa@gmail.com")
        self.line_password.setPlaceholderText("aaaa bbbb cccc dddd")
        self.line_send_to.setPlaceholderText("your@gmail.com")

        self.close_button = QPushButton()

        self.glayout.addWidget(self.label_email, 0, 0)
        self.glayout.addWidget(self.line_email, 0, 1)
        self.glayout.addWidget(self.label_password, 1, 0)
        self.glayout.addWidget(self.line_password, 1, 1)
        self.glayout.addWidget(self.label_send_to, 2, 0)
        self.glayout.addWidget(self.line_send_to, 2, 1)
        self.glayout.addWidget(self.close_button, 3, 0, 1, 2)

        self.close_button.setText("Close alert server")
        self.close_button.clicked.connect(self.close)

        self.vbox_layout = QVBoxLayout(self)
        self.vbox_layout.addLayout(self.glayout)

        self.spacer = QSpacerItem(100, 100, QSizePolicy.Expanding, QSizePolicy.Expanding)
        self.vbox_layout.addItem(self.spacer)

        settings = QSettings()
        if settings.contains("email_address"):
            self.line_email.setText(settings.value("email_address"))
        if settings.contains("email_password"):
            self.line_password.setText(settings.value("email_password"))
        if settings.contains("send_to"):
            self.line_send_to.setText(settings.value("send_to"))

        if(self.server.listen(QHostAddress.Any, port)):
            print("Alert server listening on port: ", port)
            self.server.newConnection.connect(self.on_new_connection)

    def save_settings(self):
        settings = QSettings()
        settings.setValue("email_address", self.line_email.text())
        settings.setValue("email_password", self.line_password.text())
        settings.setValue("send_to", self.line_send_to.text())

    def closeEvent(self, event):
        self.save_settings()
        # here you can terminate your threads and do other stuff

        # and afterwards call the closeEvent of the super-class
        super(simple_websocket_server, self).closeEvent(event)

    def on_new_connection(self):

        print("socket connected")
        if(self.socket):
            print("This simple app only allowed one socket connection")
            return

        self.save_at = args["save_at"] + "/cam0/" + QDateTime.currentDateTime().toString("yyyy_MM_dd") + "/"
        QDir().mkpath(self.save_at)
        self.socket = self.server.nextPendingConnection()
        self.socket.disconnected.connect(self.socket_disconnected)
        self.socket.textMessageReceived.connect(self.received_text_message)

    def extract_the_image(self, image_text, image_name):
        print("image in jobj")
        img = QImage()
        print("load from data")
        img.loadFromData(QByteArray.fromBase64(QByteArray(image_text)))
        print("load from data end")
        if(img.isNull() == False):
            img.save(self.save_at + image_name + ".jpg")

    def received_text_message(self, message : str):
        print("received text message len = ", len(message))
        try:
            jobj = json.loads(message)
            if jobj:
                if "image" in jobj:
                    self.extract_the_image(jobj["image"].encode(), jobj["image_name"]) #uncomment this line if you want to extract the image
                    del jobj["image"]
                qfile = QFile()
                qfile.setFileName((self.save_at + "/{}.txt").format(jobj["image_name"]))
                if(qfile.open(QIODevice.WriteOnly)):
                    qstream = QTextStream(qfile)
                    qstream << json.dumps(jobj)

                self.send_mail(self.save_at + jobj["image_name"] + ".jpg")
            else:
                print("cannot convert to json")
        except:
            print("cannot save alert to json by text message")

    def send_mail(self, filename):
        body = "This message is sent from fall down alert"
        save_as = filename.split("/")[-1]
        send_multipart_email(self.line_email.text(), self.line_send_to.text(), self.line_password.text(), body, filename, save_as)

    def socket_disconnected(self):
        print("socket disconnected")
        if self.socket:
            self.socket.deleteLater()
            self.socket = None

def main():
    app = QApplication(sys.argv)

    QCoreApplication.setOrganizationName("show_cases");
    QCoreApplication.setOrganizationDomain("show.com");
    QCoreApplication.setApplicationName("fall_down_alert");

    win = simple_websocket_server()
    win.resize(600, 100)
    win.show()

    sys.exit(app.exec_())

if __name__ == "__main__":
    main()
