var global_onnx_session = null;
var global_buffer = null;
var global_buffer_available = false;
var global_candidate_text_size = 0;

function getGlobalBufferAvailable()
{
    return global_buffer_available;
}

function getGlobalBuffer()
{
    return global_buffer;
}

function getGlobalCandidateTextSize()
{
    return global_candidate_text_size;
}

function releaseGlobalBuffer()
{
    if(global_buffer != null){              
        qtLoader.module()._free(global_buffer)
        global_buffer = null;
    }
}

async function recOcrText(img, width, height)
{
    global_buffer_available = false;
    releaseGlobalBuffer();
    global_buffer = null;

    qmodule = qtLoader.module();
    total_pixels = width * height * 3
    input_data = new Float32Array(total_pixels);
    for(var i = 0; i != total_pixels; ++i){     
        input_data[i] = qmodule.HEAPF32[img/Float32Array.BYTES_PER_ELEMENT + i];        
    }

    const input_tensor = new ort.Tensor('float32', input_data, [1, 3, height, width]);
    const feeds = { "x": input_tensor};
    const results = await global_onnx_session.run(feeds);
    
    output_data = results["softmax_11.tmp_0"].data
    global_candidate_text_size = output_data.length / 6625;
    
    global_buffer = qmodule._malloc(output_data.length * Float32Array.BYTES_PER_ELEMENT);    
    qmodule.HEAPF32.set(output_data, global_buffer >> 2);        

    global_buffer_available = true;
}

function global_buffer_ready()
{
    return global_buffer_available;
}

async function createGlobalSession()
{    
    global_onnx_session = await ort.InferenceSession.create('./ch_PP-OCRv4_rec_infer.onnx');    
}