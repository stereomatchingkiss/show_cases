[English](./readme.md) | 简体中文

# 如何安装

- 所有的webapp都不需要安装，使用firefox/chrome开启即可使用
- 由于是使用webassembly开发的，第一次开启需要花点时间编译，请耐心等待
- 所有程序的UI都是针对桌面设计的，请在桌面上开启

# [python_tools](./python_tools/)
- 这里收集了各种辅助webapp的脚本，由于browser的各种限制，只好使用websocket进行资料上的传输，如何安装需要的库，在[readme.md](./python_tools/simple_server/readme.md)有说明

# 简介

- 提供简单好用，跨平台且无需安装的电脑视觉软件，满足日常需求

# 网页版的限制

- 运行速度不如桌面版快
- 深度学习的算法不支援GPU
- 使用websocket传送信息的时候，如果不是本地机器，必须使用ssl，且目标所使用的key必须被浏览器承认
- 无论是opencv或Qt的多媒体模组，对浏览器的支援都不好，所以网页版只支持利用websocket传输影像的模式
- Qt for webassembly虽然很好用，但他只支援GPL和商用的license

# 各资料夹的说明

- [aruco_tasks](./aruco_tasks/) : 跟aruco有关的工具，目前支持aruco检测和生成
- [anpr_tasks](./anpr_tasks)， 车牌辨识的demo，这个问题相较于10年前容易解决的多，
我相信就算是独立开发，只要勤于收集资料，训练对应的模型，针对不同的地区调试，要做出商业水平的作品不是问题
- [face_recognition_tasks](./face_recognition_tasks)，支援批量人脸辨识的demo。
这已经是一个很成熟的技术，不乏公开且达到商用水准的模型，不过因为许可证的问题，想商用的话得自己动手训练。由于训练资料和代码都是公开的，要复现不是问题
- [generic_cv_task](./generic_cv_tasks)，一个支持物件检测、行为分类，跟踪、计数、区域警报、警报记录的webapp
- [ocr_tasks](./ocr_tasks),支援ocr的webapp
- [pose_tasks](./pose_tasks/) : 支持姿势估计，从大量图像中搜索相似姿势的webapp
- [libs](./libs), 各webapp的通用库
- [python_tools](./python_tools)，收集各种为了webaapp而设计的简易程序
- [simple_downloader](./simple_downloader), 这是我为了收集资料而写的一个小程序

# 视频示例

- [anpr_tasks](https://www.youtube.com/watch?v=Eyp1smxac1k)
- [pose_tasks, 姿态估计](https://www.youtube.com/watch?v=LGUZxGAwyCw)
- [pose_tasks, 搜寻相似姿态](https://www.youtube.com/watch?v=VJIFm6UnS0I)
- [face_recognition_tasks](https://www.youtube.com/watch?v=PICwMVUFLPw)
- [generic_cv_task](https://youtu.be/TNancuXjlcM)
- [ocr_tasks](https://youtu.be/30p-nRqsVss)

# 将来的打算

- 一次性学习的通用物体分类
- 情绪，年龄，性别辨识和警报
- 摔倒检测
- 姿势辨识
- 图像放大(super resolution)
- 图像生成(stable diffusion)
- 图像修复(image painting)
- 其他

# 其他

- 如果使用后觉得不错，请给我一颗星星，谢谢
- 如果发现bug，请开个issue
- 如果需要新的功能，请开个issue
- 所有软件仅作为演示创建，不打算用于商业目的。
- 除直接售卖之外，本软件可用于个人或商业用途
- 使用本软件时，请遵守相关法律法规。软件开发者对因使用本软件而产生的任何损失不承担任何责任
- [readme](./readme)资料夹里会介绍各类软体该如何编译出webassembly版

# 联系我

- 如果你有案子要请我帮忙，请留下你的email
- 擅长利用现有的技术解决问题，倾向于使用Qt，opencv，ncnn，pytorch等现成的软体开发软件
- 擅长开发跨平台（windows, ubuntu, android, browser）的程式，plug in(windows, ubuntu, android, ios, browser)
- 擅长实现电脑视觉的算法，opencv contrib的image hash就是我开发的，除此之外也实现过一些paper上的算法
- 了解如何训练新的深度学习模型以满足客户的客制化需求