cmake_minimum_required(VERSION 3.5)

project(generic_cv_tasks VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_AS_EXE 1)

#add_definitions(-DANNOYLIB_MULTITHREADED_BUILD)
#add_definitions(-DFMT_HEADER_ONLY)
add_definitions(-DUSE_OPENCV)

set(PLUGIN_LIBS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../libs)
set(BOOST_LIBS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../3rdLibs/boost/boost_1_81_0)

if(WIN32)
    set(3RDLIBS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../3rdLibs)
    message("building windows platform")
    #set(OPENCV_CV_PATH ${3RDLIBS_PATH}/opencv/opencv_4_5_4/opencv/build)
    set(OPENCV_CV_PATH ${3RDLIBS_PATH}/opencv/opencv_4_8_0/opencv/build)
    #set(NCNN_PATH ${3RDLIBS_PATH}/ncnn/build-vs2022/install/lib/cmake)
    set(NCNN_PATH ${3RDLIBS_PATH}/ncnn/build/ncnn-20230816-windows-vs2019/x64/lib/cmake)
    #set(NCNN_PATH ${3RDLIBS_PATH}/ncnn/build/ncnn-20221128-windows-vs2022-shared/x64/lib/cmake)

    #set(VULKAN_PATH "C:/VulkanSDK/1.3.236.0")
    #find_package(Vulkan REQUIRED)
    #target_link_libraries(target ${Vulkan_LIBRARIES})

    #set(CMAKE_PREFIX_PATH "${OPENCV_CV_PATH};${NCNN_PATH};${VULKAN_PATH};${CMAKE_PREFIX_PATH};")
    set(CMAKE_PREFIX_PATH "${OPENCV_CV_PATH};${NCNN_PATH};${CMAKE_PREFIX_PATH};")
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia)

set(PROJECT_SOURCES
        main.cpp
        algo/frame_display_worker.hpp algo/frame_display_worker.cpp
        algo/obj_detect/nanodet_worker.hpp algo/obj_detect/nanodet_worker.cpp
        config/config_read_write.hpp config/config_read_write.cpp
        config/object_detect_model_enum.hpp
        config/object_detect_model_select_config.hpp
        config/select_object_to_detect_config.hpp config/select_object_to_detect_config.cpp
        global/global_keywords.hpp global/global_keywords.cpp
        ui/mainwindow.cpp ui/mainwindow.hpp ui/mainwindow.ui
        ui/widget_select_object_to_detect.cpp ui/widget_select_object_to_detect.hpp ui/widget_select_object_to_detect.ui
        ui/widget_object_detect_model_select.cpp ui/widget_object_detect_model_select.hpp ui/widget_object_detect_model_select.ui
        ui/widget_source_selection.hpp ui/widget_source_selection.cpp ui/widget_source_selection.ui
        ui/widget_stream_player.hpp ui/widget_stream_player.cpp ui/widget_stream_player.ui
        ${PLUGIN_LIBS_PATH}/concepts_tools/numerical_concepts.hpp
        ${PLUGIN_LIBS_PATH}/cv_algo/converter/box_type_converter.hpp
        ${PLUGIN_LIBS_PATH}/cv_algo/converter/box_type_converter.cpp
        ${PLUGIN_LIBS_PATH}/cv_algo/converter/qt_and_cv_rect_converter.hpp
        ${PLUGIN_LIBS_PATH}/cv_algo/converter/qt_and_cv_rect_converter.cpp
        ${PLUGIN_LIBS_PATH}/cv_algo/obj_detect/coco_object_index.hpp
        ${PLUGIN_LIBS_PATH}/cv_algo/obj_detect/common_obj_det_type.hpp
        ${PLUGIN_LIBS_PATH}/cv_algo/obj_detect/common_obj_det_type.cpp
        ${PLUGIN_LIBS_PATH}/cv_algo/obj_detect/obj_det_base.hpp
        ${PLUGIN_LIBS_PATH}/cv_algo/obj_detect/obj_det_base.cpp
        ${PLUGIN_LIBS_PATH}/cv_algo/obj_detect/obj_det_utils.hpp
        ${PLUGIN_LIBS_PATH}/cv_algo/obj_detect/obj_det_utils.cpp
        ${PLUGIN_LIBS_PATH}/cv_algo/obj_detect/nanodet/nanodet.hpp
        ${PLUGIN_LIBS_PATH}/cv_algo/obj_detect/nanodet/nanodet.cpp
        ${PLUGIN_LIBS_PATH}/cv_algo/obj_detect/nanodet/nanodet_raw_ncnn.hpp
        ${PLUGIN_LIBS_PATH}/cv_algo/obj_detect/nanodet/nanodet_raw_ncnn.cpp
        ${PLUGIN_LIBS_PATH}/cv_algo/obj_detect/yolo_v8/yolo_v8.hpp
        ${PLUGIN_LIBS_PATH}/cv_algo/obj_detect/yolo_v8/yolo_v8.cpp
        ${PLUGIN_LIBS_PATH}/cv_algo/ocr/paddle_ocr/clipper.hpp
        ${PLUGIN_LIBS_PATH}/cv_algo/ocr/paddle_ocr/clipper.cpp
        ${PLUGIN_LIBS_PATH}/cv_algo/ocr/paddle_ocr/common.hpp
        ${PLUGIN_LIBS_PATH}/cv_algo/ocr/paddle_ocr/common.cpp
        ${PLUGIN_LIBS_PATH}/cv_algo/tracker/track_object_pass.hpp
        ${PLUGIN_LIBS_PATH}/cv_algo/tracker/track_object_pass.cpp
        ${PLUGIN_LIBS_PATH}/cv_algo/tracker/track_results.hpp
        ${PLUGIN_LIBS_PATH}/cv_algo/tracker/byte_track/BYTETracker.hpp
        ${PLUGIN_LIBS_PATH}/cv_algo/tracker/byte_track/BYTETracker.cpp
        ${PLUGIN_LIBS_PATH}/cv_algo/tracker/byte_track/KalmanFilter.hpp
        ${PLUGIN_LIBS_PATH}/cv_algo/tracker/byte_track/KalmanFilter.cpp
        ${PLUGIN_LIBS_PATH}/cv_algo/tracker/byte_track/lapjv.hpp
        ${PLUGIN_LIBS_PATH}/cv_algo/tracker/byte_track/lapjv.cpp
        ${PLUGIN_LIBS_PATH}/cv_algo/tracker/byte_track/Object.hpp
        ${PLUGIN_LIBS_PATH}/cv_algo/tracker/byte_track/Object.cpp
        ${PLUGIN_LIBS_PATH}/cv_algo/tracker/byte_track/Rect.hpp
        ${PLUGIN_LIBS_PATH}/cv_algo/tracker/byte_track/STrack.hpp
        ${PLUGIN_LIBS_PATH}/cv_algo/tracker/byte_track/STrack.cpp
        ${PLUGIN_LIBS_PATH}/cv_algo/utils/draw_rect.hpp
        ${PLUGIN_LIBS_PATH}/cv_algo/utils/draw_rect.cpp
        ${PLUGIN_LIBS_PATH}/cv_algo/utils/image_process.hpp
        ${PLUGIN_LIBS_PATH}/cv_algo/utils/image_process.cpp
        ${PLUGIN_LIBS_PATH}/event/thread_safe_event.hpp
        ${PLUGIN_LIBS_PATH}/event/thread_safe_event.cpp
        ${PLUGIN_LIBS_PATH}/json/json_utils.hpp
        ${PLUGIN_LIBS_PATH}/json/json_utils.cpp
        ${PLUGIN_LIBS_PATH}/multimedia/stream_enum.hpp
        ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_capture_base_worker.hpp
        ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_capture_base_worker.cpp
        ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_capture_controller.hpp
        ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_capture_controller.cpp
        ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_capture_opencv_worker.hpp
        ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_capture_opencv_worker.cpp
        ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_capture_params.hpp
        ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_process_base_worker.hpp
        ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_process_base_worker.cpp
        ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_process_controller.hpp
        ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_process_controller.cpp
        ${PLUGIN_LIBS_PATH}/multimedia/camera/single_frame_with_multi_worker.hpp
        ${PLUGIN_LIBS_PATH}/multimedia/camera/single_frame_with_multi_worker.cpp
        ${PLUGIN_LIBS_PATH}/multimedia/sound/alert_sound_manager.hpp
        ${PLUGIN_LIBS_PATH}/multimedia/sound/alert_sound_manager.cpp
        ${PLUGIN_LIBS_PATH}/ui/label_select_roi.hpp
        ${PLUGIN_LIBS_PATH}/ui/label_select_roi.cpp
        ${PLUGIN_LIBS_PATH}/ui/utils/qtable_item_utils.hpp ${PLUGIN_LIBS_PATH}/ui/utils/qtable_item_utils.cpp
        ${PLUGIN_LIBS_PATH}/ui/utils/utility.hpp ${PLUGIN_LIBS_PATH}/ui/utils/utility.cpp
        ${PLUGIN_LIBS_PATH}/utils/error_log.hpp
        ${PLUGIN_LIBS_PATH}/utils/file_reader.hpp
        ${PLUGIN_LIBS_PATH}/utils/file_reader.cpp
        ${PLUGIN_LIBS_PATH}/utils/meyers_singleton.hpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET pose_estimation_expert APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(${PROJECT_NAME} SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(${PROJECT_NAME}
            ${PROJECT_SOURCES}
        )
    endif()
endif()

#target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

target_include_directories(${PROJECT_NAME} PRIVATE ${PLUGIN_LIBS_PATH} ${BOOST_LIBS_PATH} ${3RDLIBS_PATH}/eigen ui)

if(ANDROID OR WIN32)
    find_package(OpenCV REQUIRED)
    find_package(ncnn REQUIRED)

    target_link_libraries(${PROJECT_NAME}
                          PRIVATE ${OpenCV_LIBS}
                          PRIVATE ncnn
                          PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
                          PRIVATE Qt${QT_VERSION_MAJOR}::Multimedia
                          )
    #target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
