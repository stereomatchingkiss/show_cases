cmake_minimum_required(VERSION 3.5)

project(video_understand_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(3RDLIBS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdLibs)
set(PLUGIN_LIBS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../libs)
set(BOOST_LIBS_PATH ${3RDLIBS_PATH}/boost/boost_1_81_0)

if(WIN32)
    message("building windows platform")
    set(OPENCV_CV_PATH ${3RDLIBS_PATH}/opencv/build/install)
    #set(ONNX_PATH ${3RDLIBS_PATH}/onnx/onnxruntime-training-win-x64-1.16.3)
    set(NCNN_PATH ${3RDLIBS_PATH}/ncnn/build/ncnn-20230816-windows-vs2019/x64/lib/cmake)

    #set(VULKAN_PATH "C:/VulkanSDK/1.3.236.0")
    #find_package(Vulkan REQUIRED)
    #target_link_libraries(target ${Vulkan_LIBRARIES})
    set(CMAKE_PREFIX_PATH "${OPENCV_CV_PATH};${NCNN_PATH};${CMAKE_PREFIX_PATH};")
endif()

add_executable(${PROJECT_NAME} main.cpp
               ${PLUGIN_LIBS_PATH}/cv_algo/video/video_classify_base.hpp
               ${PLUGIN_LIBS_PATH}/cv_algo/video/video_classify_base.cpp
               ${PLUGIN_LIBS_PATH}/cv_algo/video/video_classify_pptsm_opencv.hpp
               ${PLUGIN_LIBS_PATH}/cv_algo/video/video_classify_pptsm_opencv.cpp
               ${PLUGIN_LIBS_PATH}/cv_algo/video/video_frame_extractor.hpp
               ${PLUGIN_LIBS_PATH}/cv_algo/video/video_frame_extractor.cpp
               ${PLUGIN_LIBS_PATH}/utils/fps_estimator.hpp
               ${PLUGIN_LIBS_PATH}/utils/fps_estimator.cpp
            )

if(ANDROID OR WIN32)
    find_package(OpenCV REQUIRED)
    find_package(ncnn REQUIRED)

    target_include_directories(${PROJECT_NAME} PRIVATE ${PLUGIN_LIBS_PATH})
    target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_LIBS_PATH})

    target_link_libraries(${PROJECT_NAME}
                          PRIVATE ${OpenCV_LIBS}
                          PRIVATE ncnn
                          )
endif()

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
