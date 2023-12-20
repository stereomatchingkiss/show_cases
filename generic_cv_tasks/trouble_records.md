English | [简体中文](./trouble_records_cn.md)

# Introduction

- Records various problems encountered during development.
- Almost all of the problems come from the limitations of the browser. It can be said that more than 80% of the time is spent on how to solve the limitations of the browser as a "platform".

# Difficult problems v1.0

- The coordinate conversion of roi is not smooth. I had to use a brute-force solution, which is to keep the size of the QLabel for selecting roi and the QLabel for displaying the results the same.
- How to compile a Qt for WebAssembly version that supports threads and SIMD is written in build_qt6_wasm.txt: ../readme/build_qt6_wasm.txt.
- The webassembly version of OpenCV does not support highgui and imgcodecs, which means that videoCapture and imread cannot be used.
- The performance of Qt's MediaPlayer and QCamera in the browser is poor, and it is best not to use them.
- Github pages does not support sharedArray buffer, so I had to choose netlify.

# Difficult problems v2.0

- The C++ example for ppstm in paddleVideo has an input format error. The correct format is "rgb...rgb...rgb", not "rrr...gggg...bbbb". I spent an entire night trying to figure out what was wrong, running the Python code several times.
- pptsmv2 is trained based on kinetic400 and is not suitable for monitoring applications. If time and resources permit, the model still needs to be fine-tuned.