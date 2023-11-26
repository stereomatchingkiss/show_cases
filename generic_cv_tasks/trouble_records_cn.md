[English](./trouble_records.md) | 简体中文

# 简介

- 记录开发过程中遇见的各种问题
- 几乎所有的问题都来自于浏览器的限制上，可以说80%以上的时间都耗在如何解决浏览器这个"平台"带来的限制

# 疑难杂症

- roi的坐标转换不顺利，被逼无奈使用了一个暴力的解决办法，就是让Select roi的QLabel和显示结果的QLabel维持一样的大小
- 编译支援thread和simd的Qt for webassembly版本，该怎么做已经写在了[build_qt6_wasm.txt](../readme/build_qt6_wasm.txt)
- opencv的webassembly版不支援highgui和imgcodecs，这代表videoCapture, imread都无法使用
- Qt的MediaPlayer，QCamera在浏览器上的运行效果不佳，属于最好别用的水准
- Github pages不支援sharedArray buffer，不得已只好选择netlify
