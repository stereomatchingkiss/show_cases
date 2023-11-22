cmake_minimum_required(VERSION 3.5)

project(opencv_paddle_ocr_test LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(3RDLIBS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../3rdLibs)
set(PLUGIN_LIBS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../libs)
set(BOOST_LIBS_PATH ${3RDLIBS_PATH}/boost/boost_1_81_0)

if(WIN32)
    message("building windows platform")
    set(OPENCV_CV_PATH ${3RDLIBS_PATH}/opencv/build/install)
    set(ONNX_PATH ${3RDLIBS_PATH}/onnx/onnxruntime-training-win-x64-1.16.3)

    #set(VULKAN_PATH "C:/VulkanSDK/1.3.236.0")
    #find_package(Vulkan REQUIRED)
    #target_link_libraries(target ${Vulkan_LIBRARIES})
    set(CMAKE_PREFIX_PATH "${OPENCV_CV_PATH};${CMAKE_PREFIX_PATH};")
endif()

add_executable(opencv_paddle_ocr_test main.cpp               
               crnn_process.hpp crnn_process.cpp
               onnx_get_names_utils.hpp onnx_get_names_utils.cpp
               paddle_ocr_rec_onnx.hpp paddle_ocr_rec_onnx.cpp

               ${PLUGIN_LIBS_PATH}/cv_algo/ocr/paddle_ocr/clipper.hpp
               ${PLUGIN_LIBS_PATH}/cv_algo/ocr/paddle_ocr/clipper.cpp
               ${PLUGIN_LIBS_PATH}/cv_algo/ocr/paddle_ocr/common.hpp
               ${PLUGIN_LIBS_PATH}/cv_algo/ocr/paddle_ocr/common.cpp
               ${PLUGIN_LIBS_PATH}/cv_algo/ocr/paddle_ocr/db_post_process.hpp
               ${PLUGIN_LIBS_PATH}/cv_algo/ocr/paddle_ocr/db_post_process.cpp
               ${PLUGIN_LIBS_PATH}/cv_algo/ocr/paddle_ocr/paddle_ocr_det_opencv.hpp
               ${PLUGIN_LIBS_PATH}/cv_algo/ocr/paddle_ocr/paddle_ocr_det_opencv.cpp

           )

if(ANDROID OR WIN32)
    add_definitions(-Duse_int32)
    find_package(OpenCV REQUIRED)

    target_include_directories(${PROJECT_NAME} PRIVATE ${PLUGIN_LIBS_PATH})
    target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_LIBS_PATH})
    target_include_directories(${PROJECT_NAME} PRIVATE ${ONNX_PATH}/include)

    target_link_libraries(${PROJECT_NAME}
                          PRIVATE ${OpenCV_LIBS}
                          PRIVATE ${ONNX_PATH}/lib/onnxruntime.lib
                          PRIVATE ${ONNX_PATH}/lib/onnxruntime_providers_shared.lib
                          )
endif()

include(GNUInstallDirs)
install(TARGETS opencv_paddle_ocr_test
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
