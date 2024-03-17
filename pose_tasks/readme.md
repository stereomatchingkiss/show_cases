English | [简体中文](./readme_cn.md)

# How to install

- No installation required for web version. Open [this link](https://pose-tasks.netlify.app/pose_tasks.html) in Firefox/Chrome.
- As it is developed using WebAssembly, it takes a while to compile the first time you open it. Please wait patiently.
- The UI of this program is designed for desktop, please open it on desktop

# Introduction

- Provides a simple and easy-to-use human posture estimation tool to meet daily needs

# Third-party software used

- [opencv 4.8.0](https://github.com/opencv/opencv)
- [ncnn-20230816](https://github.com/Tencent/ncnn)
- [Qt6.5.2](https://www.qt.io/)
- [Boost 1.81.0](https://www.boost.org/)

# Features of this software

- Pose estimation of a single human body in pictures and videos (v1.0)
- Return the detected posture message (x, y, confidence)

# How to use the websocket of this software

- If you want to transmit images through websocket, please refer to [qt_base_pose_estimation_server](https://github.com/stereomatchingkiss/object_detection_and_alarm/blob/main/qt_base_pose_estimation_server.py)
- [install_qt_script.md](https://github.com/stereomatchingkiss/object_detection_and_alarm/blob/main/install_qt_script.md)，explain how to install the required packages

# Limitations of the web version

- The running speed is not as fast as the desktop version
- Deep learning algorithms do not support GPU
- When using websocket to transmit information, if it is not a local machine, SSL must be used, and the key used by the target must be recognized by the browser.
- Neither OpenCV nor Qt's multimedia module provides good support for browsers, so the web version only supports image transmission using websockets.
- Qt for WebAssembly is a very useful tool, but it is only available under the GPL and commercial licenses.

# Examples

- [Video tutorial](https://www.youtube.com/watch?v=LGUZxGAwyCw)
- ![example1](./imgs/00.png)
- ![example2](./imgs/01.png)
- ![example3](./imgs/02.png)

# Future plans

- Supports multi-person pose estimation
- Use a better model ([mmpose](https://github.com/open-mmlab/mmpose))
- Fitness functions (squat, push-ups etc)
- Estimate pose similarity
- From a large number of pictures, find the picture whose pose is closest to the target
- Others

# Others

- If you think it is good after using it, please give me a star, thank you
- If you find a bug, please open an issue
- If you need new features, please open an issue
- GPU support will be completed by Vulkan. We don't plan to support CUDA for now, because it's too bloated and only supports Nvidia GPUs.
- The developer of this software created it as a demo and does not intend to use it for commercial purposes.
- Except for direct sales, this software can be used for personal or commercial purposes.
- When using this software, please comply with relevant laws and regulations. The software developer is not responsible for any losses caused by using this software.
- How to compile Qt6.5.2 that supports SIMD and thread, please refer to [build_qt6_wasm_steps.txt](https://github.com/stereomatchingkiss/object_detection_and_alarm/blob/main/build_qt6_wasm_steps.txt)
- The UI of this program is designed for desktop, please open it on the desktop
- Release the desktop version

# Contact me

- If you have a case to ask me for help, please leave your email
- Good at solving problems using existing technologies, prefer to use Qt