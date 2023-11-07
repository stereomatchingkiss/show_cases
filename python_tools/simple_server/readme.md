# Install dependencies

. Install anaconda
. conda create --name your_name
. conda install pip
. pip install opencv-python
. pip install pyqt5

# Execute

python qt_base_opencv_videocapture_server.py --opencv_url your_url/your_video.mp4 #This will send the frame to the client

# Cautions

. This simple server accept one client connection only
. If the frame is too large, please enter "--downsample 2" to scale down the image, 2 mean scale down 2 times, 3 mean times etc

