English | [简体中文](./readme_cn.md)

# How to install

- All webapp does not need to be installed. You can use Firefox/Chrome to open it.
- Since it is developed using WebAssembly, it will take some time to compile the first time you open it. Please be patient
- The UI of all programs is designed for desktop, please open it on desktop.

# [python_tools](./python_tools/)

- This collects various scripts to assist webapp. Due to various limitations of the browser, websocket is used for data transmission. How to install the libs is explained in [readme.md](./python_tools/simple_server/readme.md)

# Introduction

- Provide simple, easy-to-use, cross-platform and installation-free computer vision software to meet daily needs.

# Limitations of the web version

- The running speed is not as fast as the desktop version.
- Deep learning algorithms do not support GPU.
- When sending information using websocket, if it is not a local machine, SSL must be used, and the key used by the target must be recognized by the browser.
- Whether the multimedia module of OpenCV or Qt is not well supported by the browser, so the web version only supports the mode of transmitting images using websocket.
- Qt for WebAssembly is very good, but it only supports GPL and commercial licenses.

# Explanation of each folder

- [aruco_tasks](./aruco_tasks/) : A collection of tasks related to aruco. Support aruco generation and detection
- [anpr_tasks](./anpr_tasks) : Demo of license plate recognition. This problem is much easier to solve than 10 years ago. I believe that even if you develop independently, as long as you are diligent in collecting data, training corresponding models, and tune them for different regions, it will not be a problem to create commercial-level works.
- [face_recognition_tasks](./face_recognition_tasks) : Demo that supports batch face recognition.
This is already a very mature technology, and there are many open and commercial-level models. However, due to licensing issues, if you want to use it commercially, you have to do your own training. Since the training materials and codes are public, reproducing them is not a problem.
- [fall_detection](./fall_detection/) : A fall down detection webapp
- [generic_cv_task](./generic_cv_tasks/) : A webapp that supports object detection, action classify, tracking, counting, area alarm, and alarm recording.
- [ocr_tasks](./ocr_tasks/) : A webapp supports ocr(Chinese and English, support batch processing)
- [pose_tasks](./pose_tasks/) : A webapp support pose estimation and similar pose search from images
- [libs](./libs) : The common library for all webapp.
- [python_tools](./python_tools) : Collects various simple programs designed for webaapp.
- [simple_downloader](./simple_downloader) : This is a small program I wrote to collect data.

# Video examples

- [anpr_tasks](https://www.youtube.com/watch?v=Eyp1smxac1k)
- [pose_tasks, pose estimation](https://www.youtube.com/watch?v=LGUZxGAwyCw)
- [pose_tasks, search similar pose](https://www.youtube.com/watch?v=VJIFm6UnS0I)
- [face_recognition_tasks](https://www.youtube.com/watch?v=PICwMVUFLPw)
- [generic_cv_task](https://youtu.be/TNancuXjlcM)
- [ocr_tasks](https://youtu.be/30p-nRqsVss)

# Future plans

- Generic object classification with one shot learning
- Emotion, age, and gender recognition and alarm
- Fall detection
- Pose recognition
- Image super resolution
- Image generation (stable diffusion)
- Image restoration (image painting)
- Other

# Others
- If you think it is good after using it, please give me a star, thank you.
- If you find a bug, please open an issue.
- If you need new features, please open an issue.
- All software is created for demonstration only and is not intended for commercial purposes.
- Except for direct sale, this software can be used for personal or commercial purposes.
- When using this software, please comply with relevant laws and regulations. The software developer is not responsible for any losses caused by the use of this software.
- The [readme](./readme) folder will introduce how to compile each type of software into the WebAssembly version.

# Contact me

- If you have a case that you want me to help with, please leave your email.
- I am good at using existing technologies to solve problems, and I tend to use existing software development software such as Qt, OpenCV, ncnn, and PyTorch.
- I am good at developing cross-platform (Windows, Ubuntu, Android, Browser) programs and plugins (Windows, Ubuntu, Android, iOS, Browser).
- I am good at implementing computer vision algorithms. The image hash module of OpenCV Contrib was developed by me. In addition, I have implemented some algorithms from papers.
- I understand how to train new deep learning models to meet customer's customized needs.
