import onnx
from onnxsim import simplify

import torch

def call_subprocess(cmd):
    import subprocess

    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
    out, err = p.communicate()
    result = out.decode('utf-8').split('\n')
    for lin in result:
        if not lin.startswith('#'):
            print(lin)

def convert_model_to_ncnn(input_model_name, output_model_name):
    # load your predefined ONNX model
    model = onnx.load(input_model_name)

    # convert model
    model_simp, check = simplify(model)

    #assert check, "Simplified ONNX model could not be validated"

    onnx.save(model_simp, "{}_simple.onnx".format(output_model_name))

    cmd = 'python -m onnxsim {} {}_simple.onnx'.format(input_model_name, output_model_name)
    call_subprocess(cmd)

    ncnn_path = "D:/programming/3rdLibs/ncnn/build/ncnn-20230816-windows-vs2019/x86/bin/"
    cmd = '{}/onnx2ncnn.exe {}_simple.onnx {}.param {}.bin'.format(ncnn_path, output_model_name, output_model_name, output_model_name)
    call_subprocess(cmd)
    cmd = '{}/ncnnoptimize {}.param {}.bin {}_opt.param {}_opt.bin 65536'.format(ncnn_path, output_model_name, output_model_name, output_model_name, output_model_name)
    call_subprocess(cmd)

#convert_model_to_ncnn("ch/det_onnx/model.onnx", "ch_PP-OCRv4_det")
#convert_model_to_ncnn("ch/rec_onnx/model.onnx", "ch_PP-OCRv4_rec")
#convert_model_to_ncnn("nanodet-plus-m-1.5x_320.onnx", "nanodet-plus-m-1.5x_320")
convert_model_to_ncnn("nanodet-plus-m-1.5x_416.onnx", "nanodet-plus-m-1.5x_416")