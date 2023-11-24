[English](./trouble_records.md) | 简体中文

# 简介

- 记录开发过程中遇见的各种问题
- 桌面版大约两天的时间就开发完毕了，但是为了让他可以在网页上顺利的执行，我至少多花了一个礼拜的时间
- 这些问题难在跟我一样使用Qt+opencv+onnxruntime+ncnn开发webapp的人不多，遇上问题的时候很难找到现成的答案，只能自己摸索

# 疑难杂症

- paddle ocr v4无法转换成ncnn
- 利用ncnn执行的paddle ocr v3，在浏览器上的输出结果是错误的
- opencv dnn依旧不支持动态输入，这导致paddle ocr v3和v4的文字识别模型全都无法执行
- 把paddle ocr v4的文字辨识模型转换成onnx后，输入格式不是bgrbgr，是bbbbb.....gggggg......rrrrrr
- onnxruntime的c++接口很容易和其他库产生各种冲突，建议直接使用js版本
- EM_JS定义的函数只能在main thread中呼叫， 为了避免程序freeze，只好修改api，使用QTimer周期性的检查预测结果是否已经完成，这增加了程序设计的复杂度
- paddle ocr rec支持任意宽度的图像输入，但图像过大会导致记忆体不足，这问题让人感到困惑的地方是onnxruntime抛出来的错误讯息非常奇怪
- browser对一系列的图像执行ocr后，部分按钮会消失，这问题透过重新resize就能解决
