English | [简体中文](./readme_cn.md)

# How to install

- The web version does not need to be installed. Open this [website](https://fall-down-detection.netlify.app/generic_cv_tasks.html) with Firefox/Chrome/edge to use.
- Since it is developed using WebAssembly, it will take some time to compile the first time you open it. Please be patient.
- The UI of this program is designed for desktop, please open it on the desktop.

# Introduction

- The model used are [nanodet](https://github.com/RangiLyu/nanodet)and[yolov8](https://github.com/ultralytics/ultralytics)
- Provide simple and easy-to-use fall detection tools to meet daily needs

# Third-party software used

- [opencv 4.8.0](https://github.com/opencv/opencv)
- [Qt6.5.2](https://www.qt.io/)
- [ncnn](https://github.com/Tencent/ncnn)
- [Boost 1.81.0](https://www.boost.org/)

# How to use the websocket of this software

- Please send the stream via websocket. For details, please refer to [qt_base_opencv_videocapture_server.py](https://github.com/stereomatchingkiss/show_cases/blob/master/python_tools/simple_server/qt_base_opencv_videocapture_server.py)
- This software supports the function of sending fall alarms to email. In order to send emails, you must first apply for a gmail mailbox and make some settings. For details on this part, please refer to this [video](https://www.youtube.com/watch?v=g_j6ILT-X0k&t=582s). After applying for and setting up the mailbox, please use [qt_base_fall_down_obj_det_alert_receiver.py](https://github.com/stereomatchingkiss/show_cases/blob/master/python_tools/simple_server/qt_base_fall_down_obj_det_alert_receiver.py) to receive and send alerts

# Limitations of the web version

- The running speed is not as fast as the desktop version.
- Cannot actively write settings or alerts to the local hard drive. To bypass this limitation, the web version provides websocket to send alerts.
- When sending information through websocket, if it is not a local machine, you must use SSL, and the key used by the target must be recognized by the browser.
- Qt for WebAssembly is very good, but it only has GPL and commercial licenses.

# Examples

- [Video tutorial](https://www.youtube.com/watch?v=gI_S8KvLkkM)
- ![Example 1](./imgs/00.png)
- ![Example 2](./imgs/01.png)
- ![Example 3](./imgs/02.png)

# Future plans

- Support monitoring multiple videos at the same time
- Support different object detection models
- Use other algorithms to detect falls
- other

# Source codes

- Located in [generic_cv_tasks](../generic_cv_tasks), since there is a lot of code duplication, I chose to integrate this function into generic_cv_tasks and control which functions should be compiled through CMake

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