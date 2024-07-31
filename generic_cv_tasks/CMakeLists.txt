cmake_minimum_required(VERSION 3.5)

project(generic_cv_tasks VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(3RDLIBS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../3rdLibs)
set(PLUGIN_LIBS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../libs)
set(BOOST_LIBS_PATH ${3RDLIBS_PATH}/boost/boost_1_81_0)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia WebSockets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia WebSockets)

if(WIN32)
    message("building windows platform")
    set(OPENCV_CV_PATH ${3RDLIBS_PATH}/opencv/build/install)
    set(NCNN_PATH ${3RDLIBS_PATH}/ncnn/build/ncnn-20230816-windows-vs2019/x64/lib/cmake)

    #set(VULKAN_PATH "C:/VulkanSDK/1.3.236.0")
    #find_package(Vulkan REQUIRED)
    #target_link_libraries(target ${Vulkan_LIBRARIES})
    set(CMAKE_PREFIX_PATH "${OPENCV_CV_PATH};${NCNN_PATH};${CMAKE_PREFIX_PATH};")
endif()

if(EMSCRIPTEN)

    message("build for wasm")
    set(NCNN_PATH       ${3RDLIBS_PATH}/ncnn/ncnn-20231027/build/wasm/simd_threads/install/)
    set(OPENCV_CV_PATH  ${3RDLIBS_PATH}/opencv/build/4.8.0/simd_threads/install)

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s NO_EXIT_RUNTIME=1 -s FORCE_FILESYSTEM=1 -s INITIAL_MEMORY=256MB -s ALLOW_MEMORY_GROWTH -s MAXIMUM_MEMORY=4GB -s EXIT_RUNTIME=1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s NO_EXIT_RUNTIME=1 -s FORCE_FILESYSTEM=1 -s INITIAL_MEMORY=256MB -s ALLOW_MEMORY_GROWTH -s MAXIMUM_MEMORY=4GB -s EXIT_RUNTIME=1")
    set(CMAKE_EXECUTBLE_LINKER_FLAGS "${CMAKE_EXECUTBLE_LINKER_FLAGS} -s FORCE_FILESYSTEM=1 -s INITIAL_MEMORY=256MB -s ALLOW_MEMORY_GROWTH -s MAXIMUM_MEMORY=4GB -s EXIT_RUNTIME=1")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@.")
    set(CMAKE_EXECUTBLE_LINKER_FLAGS "${CMAKE_EXECUTBLE_LINKER_FLAGS} --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@.")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp -pthread -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=4")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -pthread -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=4")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fopenmp -pthread -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=4")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -fexperimental-library")
endif()

set(CV_ALGO_SOURCES ${PLUGIN_LIBS_PATH}/cv_algo/converter/box_type_converter.hpp
    ${PLUGIN_LIBS_PATH}/cv_algo/converter/box_type_converter.cpp
    ${PLUGIN_LIBS_PATH}/cv_algo/converter/qt_and_cv_rect_converter.hpp
    ${PLUGIN_LIBS_PATH}/cv_algo/converter/qt_and_cv_rect_converter.cpp
    ${PLUGIN_LIBS_PATH}/cv_algo/obj_detect/coco_object_index.hpp
    ${PLUGIN_LIBS_PATH}/cv_algo/obj_detect/common_obj_det_type.hpp
    ${PLUGIN_LIBS_PATH}/cv_algo/obj_detect/common_obj_det_type.cpp
    ${PLUGIN_LIBS_PATH}/cv_algo/obj_detect/ncnn_det_model_auxiliary.hpp
    ${PLUGIN_LIBS_PATH}/cv_algo/obj_detect/ncnn_det_model_auxiliary.cpp
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
    ${PLUGIN_LIBS_PATH}/cv_algo/video/video_classify_base.hpp
    ${PLUGIN_LIBS_PATH}/cv_algo/video/video_classify_base.cpp
    ${PLUGIN_LIBS_PATH}/cv_algo/video/video_classify_pptsm_opencv.hpp
    ${PLUGIN_LIBS_PATH}/cv_algo/video/video_classify_pptsm_opencv.cpp
    ${PLUGIN_LIBS_PATH}/cv_algo/video/video_frame_extractor.hpp
    ${PLUGIN_LIBS_PATH}/cv_algo/video/video_frame_extractor.cpp
)

set(MULTIMEDIA_SOURCE ${PLUGIN_LIBS_PATH}/multimedia/stream_enum.hpp
    ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_capture_params.hpp
    ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_process_base_worker.hpp
    ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_process_base_worker.cpp
    ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_process_controller.hpp
    ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_process_controller.cpp
    ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_capture_qcamera.hpp
    ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_capture_qcamera.cpp
    ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_capture_qcamera_params.hpp
    ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_capture_qmediaplayer.hpp
    ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_capture_qmediaplayer.cpp
    ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_capture_qmediaplayer_params.hpp
    ${PLUGIN_LIBS_PATH}/multimedia/camera/single_frame_with_multi_worker_base.hpp
    ${PLUGIN_LIBS_PATH}/multimedia/camera/single_frame_with_multi_worker_base.cpp
    ${PLUGIN_LIBS_PATH}/multimedia/network/frame_capture_websocket_params.hpp
    ${PLUGIN_LIBS_PATH}/multimedia/network/frame_capture_websocket.hpp
    ${PLUGIN_LIBS_PATH}/multimedia/network/frame_capture_websocket.cpp
    ${PLUGIN_LIBS_PATH}/multimedia/sound/alert_sound_manager.hpp
    ${PLUGIN_LIBS_PATH}/multimedia/sound/alert_sound_manager.cpp
)

SET(OTHER_PLUGIN_SOURCES ${PLUGIN_LIBS_PATH}/concepts_tools/numerical_concepts.hpp
    ${PLUGIN_LIBS_PATH}/event/thread_safe_event.hpp
    ${PLUGIN_LIBS_PATH}/event/thread_safe_event.cpp
    ${PLUGIN_LIBS_PATH}/json/json_utils.hpp
    ${PLUGIN_LIBS_PATH}/json/json_utils.cpp
    ${PLUGIN_LIBS_PATH}/network/websocket_client_controller.hpp
    ${PLUGIN_LIBS_PATH}/network/websocket_client_controller.cpp
    ${PLUGIN_LIBS_PATH}/network/websocket_client_worker.hpp
    ${PLUGIN_LIBS_PATH}/network/websocket_client_worker.cpp
    ${PLUGIN_LIBS_PATH}/ui/label_select_roi.hpp
    ${PLUGIN_LIBS_PATH}/ui/label_select_roi.cpp
    ${PLUGIN_LIBS_PATH}/ui/utils/qtable_item_utils.hpp ${PLUGIN_LIBS_PATH}/ui/utils/qtable_item_utils.cpp
    ${PLUGIN_LIBS_PATH}/ui/utils/utility.hpp ${PLUGIN_LIBS_PATH}/ui/utils/utility.cpp
    ${PLUGIN_LIBS_PATH}/utils/file_reader.hpp
    ${PLUGIN_LIBS_PATH}/utils/file_reader.cpp
    ${PLUGIN_LIBS_PATH}/utils/image_utils.hpp
    ${PLUGIN_LIBS_PATH}/utils/image_utils.cpp
    ${PLUGIN_LIBS_PATH}/utils/meyers_singleton.hpp
    ${PLUGIN_LIBS_PATH}/utils/qimage_to_cvmat.hpp
    ${PLUGIN_LIBS_PATH}/utils/qimage_to_cvmat.cpp
)

if(NOT EMSCRIPTEN)
    set(MULTIMEDIA_SOURCE ${MULTIMEDIA_SOURCE}
        ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_capture_base_worker.hpp ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_capture_base_worker.cpp        
        ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_capture_opencv.hpp ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_capture_opencv.cpp
        ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_capture_opencv_controller.hpp ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_capture_opencv_controller.cpp
        ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_capture_opencv_worker.hpp ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_capture_opencv_worker.cpp
        ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_capture_opencv_params.hpp
    )
endif()

SET(ACTION_CLASSIFY_SOURCES
    algo/action_classify/action_classify_alert_save.hpp algo/action_classify/action_classify_alert_save.cpp
    algo/action_classify/kinetic_400_labels.hpp algo/action_classify/kinetic_400_labels.cpp
    algo/action_classify/pptsm_v2_worker.hpp algo/action_classify/pptsm_v2_worker.cpp

    config/config_action_classify_alert.hpp
    config/config_action_classify_model_select.hpp
    config/config_pptsm_v2_worker.hpp
    config/config_select_action_to_classify.hpp

    ui/action_classify/widget_action_classify_alert.hpp ui/action_classify/widget_action_classify_alert.cpp ui/action_classify/widget_action_classify_alert.ui
    ui/action_classify/widget_action_classify_model_select.hpp ui/action_classify/widget_action_classify_model_select.cpp ui/action_classify/widget_action_classify_model_select.ui
    ui/action_classify/widget_select_action_to_classify.hpp ui/action_classify/widget_select_action_to_classify.cpp ui/action_classify/widget_select_action_to_classify.ui
    ui/action_classify/widget_stacks_action_classify.hpp ui/action_classify/widget_stacks_action_classify.cpp ui/action_classify/widget_stacks_action_classify.ui)

SET(FALL_DOWN_DETECT_SOURCES
    algo/fall_down_detect/fall_down_det_alert_save.hpp algo/fall_down_detect/fall_down_det_alert_save.cpp
    algo/fall_down_detect/fall_down_obj_det_worker.hpp algo/fall_down_detect/fall_down_obj_det_worker.cpp

    config/config_fall_down_condition.hpp
    config/config_fall_down_obj_det_alert.hpp
    config/config_fall_down_obj_det_worker.hpp

    ui/fall_down_detect/widget_fall_down_obj_det_alert.hpp ui/fall_down_detect/widget_fall_down_obj_det_alert.cpp ui/fall_down_detect/widget_fall_down_obj_det_alert.ui
    ui/fall_down_detect/widget_fall_down_condition.hpp ui/fall_down_detect/widget_fall_down_condition.cpp ui/fall_down_detect/widget_fall_down_condition.ui
    ui/fall_down_detect/widget_stacks_fall_down_detection.hpp ui/fall_down_detect/widget_stacks_fall_down_detection.cpp ui/fall_down_detect/widget_stacks_fall_down_detection.ui)

SET(OBJ_DETECT_SOURCES
    algo/obj_detect/nanodet_alert_save.hpp algo/obj_detect/nanodet_alert_save.cpp
    algo/obj_detect/nanodet_worker.hpp algo/obj_detect/nanodet_worker.cpp

    config/config_nanodet_worker.hpp

    ui/obj_detect/widget_select_object_to_detect.cpp ui/obj_detect/widget_select_object_to_detect.hpp ui/obj_detect/widget_select_object_to_detect.ui
    ui/obj_detect/widget_stacks_object_tracking.hpp ui/obj_detect/widget_stacks_object_tracking.cpp ui/obj_detect/widget_stacks_object_tracking.ui
    ui/obj_detect/widget_tracker_alert.hpp ui/obj_detect/widget_tracker_alert.cpp ui/obj_detect/widget_tracker_alert.ui)

SET(COMMON_MAIN_SOURCES

    main.cpp

    algo/frame_display_worker.hpp algo/frame_display_worker.cpp
    algo/generic_obj_detector.hpp algo/generic_obj_detector.cpp
    algo/generic_worker_results.hpp    

    config/config_alert_sender.hpp
    config/config_generic_obj_detector.hpp    
    config/config_object_detect_model_select.hpp    
    config/config_read_write.hpp config/config_read_write.cpp

    config/config_select_object_to_detect.hpp
    config/config_source_selection.hpp
    config/config_tasks_selection.hpp
    config/config_tracker_alert.hpp
    config/object_detect_model_enum.hpp

    global/global_keywords.hpp global/global_keywords.cpp
    global/global_object.hpp global/global_object.cpp

    ui/dialog_alert_sender_settings.hpp ui/dialog_alert_sender_settings.cpp ui/dialog_alert_sender_settings.ui
    ui/frame_capture_creator.hpp ui/frame_capture_creator.cpp
    ui/mainwindow.cpp ui/mainwindow.hpp ui/mainwindow.ui
    ui/obj_detect/widget_object_detect_model_select.cpp ui/obj_detect/widget_object_detect_model_select.hpp ui/obj_detect/widget_object_detect_model_select.ui
    ui/widget_multi_stream_manager.hpp ui/widget_multi_stream_manager.cpp ui/widget_multi_stream_manager.ui
    ui/widget_roi_selection.hpp ui/widget_roi_selection.cpp
    ui/widget_source_selection.hpp ui/widget_source_selection.cpp ui/widget_source_selection.ui        
    ui/widget_stacks_manager.hpp ui/widget_stacks_manager.cpp ui/widget_stacks_manager.ui    
    ui/widget_stream_player.hpp ui/widget_stream_player.cpp ui/widget_stream_player.ui
    ui/widget_tasks_selection.hpp ui/widget_tasks_selection.cpp ui/widget_tasks_selection.ui    

    ${CV_ALGO_SOURCES}
    ${MULTIMEDIA_SOURCE}
    ${OTHER_PLUGIN_SOURCES}
)

set(FALL_DOWN_DET_ON ON)
set(OBJ_DET_ON OFF)
set(ACTION_CLASSIFY_ON OFF)

set(PROJECT_SOURCES
        ${COMMON_MAIN_SOURCES}
)

if(ACTION_CLASSIFY_ON)
    add_definitions(-DACTION_CLASSIFY_IS_ON)
    set(PROJECT_SOURCES ${PROJECT_SOURCES} ${ACTION_CLASSIFY_SOURCES})
endif()

if(FALL_DOWN_DET_ON)
    add_definitions(-DFALL_DOWN_DET_IS_ON)
    set(PROJECT_SOURCES ${PROJECT_SOURCES} ${FALL_DOWN_DETECT_SOURCES})
endif()

if(OBJ_DET_ON)
    add_definitions(-DOBJ_DET_IS_ON)
    set(PROJECT_SOURCES ${PROJECT_SOURCES} ${OBJ_DETECT_SOURCES})
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}        
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET simple_qt_pro APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
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

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.${PROJECT_NAME})
endif()

if(ANDROID OR WIN32)
    find_package(OpenCV REQUIRED)
    find_package(ncnn REQUIRED)

    target_include_directories(${PROJECT_NAME} PRIVATE ${PLUGIN_LIBS_PATH})
    target_include_directories(${PROJECT_NAME} PRIVATE ${3RDLIBS_PATH}/eigen)
    target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_LIBS_PATH})

    target_link_libraries(${PROJECT_NAME}
                          PRIVATE ${OpenCV_LIBS}
                          PRIVATE ncnn
                          PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
                          PRIVATE Qt${QT_VERSION_MAJOR}::Multimedia
                          PRIVATE Qt${QT_VERSION_MAJOR}::WebSockets
                          )
endif()

if(EMSCRIPTEN)
    add_definitions(-DWASM_BUILD)
    add_compile_definitions(EIGEN_DONT_PARALLELIZE)
    target_include_directories(${PROJECT_NAME} PRIVATE ${NCNN_PATH}/include/ncnn)
    target_include_directories(${PROJECT_NAME} PRIVATE ${OPENCV_CV_PATH}/include/opencv4)
    target_include_directories(${PROJECT_NAME} PRIVATE ${PLUGIN_LIBS_PATH})
    target_include_directories(${PROJECT_NAME} PRIVATE ${3RDLIBS_PATH}/eigen)
    target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_LIBS_PATH})


    target_link_libraries(${PROJECT_NAME}
                          PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
                          PRIVATE Qt${QT_VERSION_MAJOR}::Multimedia
                          PRIVATE Qt${QT_VERSION_MAJOR}::WebSockets
                          PRIVATE ${OPENCV_CV_PATH}/3rdparty/lib/libzlib.a
                          PRIVATE ${OPENCV_CV_PATH}/3rdparty/lib/liblibopenjp2.a
                          PRIVATE ${OPENCV_CV_PATH}/3rdparty/lib/liblibprotobuf.a
                          #${OPENCV_CV_PATH}/3rdparty/libquirc.a
                          PRIVATE ${OPENCV_CV_PATH}/lib/libopencv_world.a
                          PRIVATE ${NCNN_PATH}/lib/libncnn.a
                          )
endif()


set_target_properties(${PROJECT_NAME} PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
