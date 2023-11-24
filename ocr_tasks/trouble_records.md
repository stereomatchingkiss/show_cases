
[English] | [简体中文](./trouble_records_cn.md)

# Introduction

- Records various problems encountered during development.
- The desktop version was developed in about two days, but it took me at least a week to get it to run smoothly on the web.
- These problems are difficult because there are not many people who develop webapp using Qt+opencv+onnxruntime+ncnn, so it is difficult to find ready-made answers when encountering problems.

# Difficult problems

- Paddle OCR v4 cannot be converted to ncnn.
- The output result of paddle OCR v3 executed by ncnn is incorrect in the browser.
- OpenCV dnn still does not support dynamic input, which prevents paddle OCR v3 and v4 text recognition models from being executed.
- After converting the paddle OCR v4 text recognition model to onnx, the input format is not bgrbgr, but bbbb.....gggggg......rrrrrr.
- The C++ interface of onnxruntime is very likely to conflict with other libraries, so it is recommended to use the JavaScript version directly.
- The functions defined by EM_JS can only be called in the main thread. In order to avoid program freezing, I had to modify the api and use QTimer to periodically check    whether the prediction results are complete. This increases the complexity of program design.
- Paddle OCR rec supports arbitrary width of image input, but the image will cause memory overflow if it is too large. The confusing part of this problem is that the error message thrown by onnxruntime is very strange.
- After the browser performs ocr on a series of images, some buttons will disappear. This problem can be solved by resizing again.
- Qt for WebAssembly cannot use the system fonts provided by the operating system. The fonts available on the internet are either too large or incomplete.