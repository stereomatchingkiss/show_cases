import smtplib, ssl

from email import encoders
from email.mime.base import MIMEBase
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart

def send_plain_text_email(sender_email, receiver_email, password, message, port = 587, smtp_server = "smtp.gmail.com"):
    with smtplib.SMTP(smtp_server, port) as server:
        server.ehlo()  # Can be omitted
        context = ssl.create_default_context()
        server.starttls(context=context)
        server.ehlo()  # Can be omitted
        server.login(sender_email, password)
        server.sendmail(sender_email, receiver_email, message)
        print("send mail done")

def attach_binary_part(filename, save_as, message):
    # Open PDF file in binary mode
    with open(filename, "rb") as attachment:
        # Add file as application/octet-stream
        # Email client can usually download this automatically as attachment
        part = MIMEBase("application", "octet-stream")
        part.set_payload(attachment.read())

        # Encode file in ASCII characters to send by email
        encoders.encode_base64(part)

        # Add header as key/value pair to attachment part
        part.add_header(
            "Content-Disposition",
            f"attachment; filename= {save_as}",
        )

        # Add attachment to message and convert message to string
        message.attach(part)

def send_multipart_email(sender_email, receiver_email, password, body, filename, save_as, port = 465, smtp_server = "smtp.gmail.com"):
    context = ssl.create_default_context()
    with smtplib.SMTP_SSL(smtp_server, port, context=context) as server:
        message = MIMEMultipart()
        message["From"] = sender_email
        message["To"] = receiver_email
        message["Subject"] = "Fall down alert"
        message["Bcc"] = receiver_email  # Recommended for mass emails

        message.attach(MIMEText(body, "plain"))
        attach_binary_part(filename, save_as, message)

        server.login(sender_email, password)
        server.sendmail(sender_email, receiver_email, message.as_string())
