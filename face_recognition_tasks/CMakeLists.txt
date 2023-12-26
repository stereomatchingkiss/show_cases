cmake_minimum_required(VERSION 3.5)

project(face_recognition_tasks VERSION 0.1 LANGUAGES CXX)

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
    set(OPENCV_CV_PATH ${3RDLIBS_PATH}/opencv/opencv_4_8_0/opencv/build)
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

set(CV_ALGO_SOURCES
    ${PLUGIN_LIBS_PATH}/cv_algo/converter/qt_and_cv_rect_converter.hpp
    ${PLUGIN_LIBS_PATH}/cv_algo/converter/qt_and_cv_rect_converter.cpp
    ${PLUGIN_LIBS_PATH}/cv_algo/face/face_utils.hpp
    ${PLUGIN_LIBS_PATH}/cv_algo/face/face_utils.cpp
    ${PLUGIN_LIBS_PATH}/cv_algo/face/det/face_detector_base.hpp
    ${PLUGIN_LIBS_PATH}/cv_algo/face/det/face_detector_base.cpp
    ${PLUGIN_LIBS_PATH}/cv_algo/face/det/face_detector_box.hpp
    ${PLUGIN_LIBS_PATH}/cv_algo/face/det/face_detector_anchor_creator.hpp
    ${PLUGIN_LIBS_PATH}/cv_algo/face/det/face_detector_anchor_creator.cpp
    ${PLUGIN_LIBS_PATH}/cv_algo/face/det/face_detector_retina_face_ncnn.hpp
    ${PLUGIN_LIBS_PATH}/cv_algo/face/det/face_detector_retina_face_ncnn.cpp
    ${PLUGIN_LIBS_PATH}/cv_algo/face/reg/face_recognition_base.hpp
    ${PLUGIN_LIBS_PATH}/cv_algo/face/reg/face_recognition_base.cpp
    ${PLUGIN_LIBS_PATH}/cv_algo/face/reg/face_recognition_arcface_ncnn.hpp
    ${PLUGIN_LIBS_PATH}/cv_algo/face/reg/face_recognition_arcface_ncnn.cpp
    ${PLUGIN_LIBS_PATH}/cv_algo/face/reg/face_warp.hpp
    ${PLUGIN_LIBS_PATH}/cv_algo/face/reg/face_warp.cpp
    ${PLUGIN_LIBS_PATH}/cv_algo/ncnn/ncnn_model_loader.hpp
    ${PLUGIN_LIBS_PATH}/cv_algo/ncnn/ncnn_model_loader.cpp
    ${PLUGIN_LIBS_PATH}/cv_algo/obj_detect/common_obj_det_type.hpp
    ${PLUGIN_LIBS_PATH}/cv_algo/obj_detect/common_obj_det_type.cpp
    ${PLUGIN_LIBS_PATH}/cv_algo/utils/similarity_compare.hpp
    ${PLUGIN_LIBS_PATH}/cv_algo/utils/similarity_compare.cpp
)

set(MULTIMEDIA_SOURCE ${PLUGIN_LIBS_PATH}/multimedia/stream_enum.hpp
    ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_capture_params.hpp
    ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_process_base_worker.hpp
    ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_process_base_worker.cpp
    ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_process_controller.hpp
    ${PLUGIN_LIBS_PATH}/multimedia/camera/frame_process_controller.cpp
    ${PLUGIN_LIBS_PATH}/multimedia/camera/single_frame_with_multi_worker_base.hpp
    ${PLUGIN_LIBS_PATH}/multimedia/camera/single_frame_with_multi_worker_base.cpp
    ${PLUGIN_LIBS_PATH}/multimedia/network/frame_capture_websocket_params.hpp
    ${PLUGIN_LIBS_PATH}/multimedia/network/frame_capture_websocket.hpp
    ${PLUGIN_LIBS_PATH}/multimedia/network/frame_capture_websocket.cpp
)

SET(OTHER_PLUGIN_SOURCES ${PLUGIN_LIBS_PATH}/json/json_utils.hpp
    ${PLUGIN_LIBS_PATH}/json/json_utils.cpp
    ${PLUGIN_LIBS_PATH}/network/websocket_client_controller.hpp
    ${PLUGIN_LIBS_PATH}/network/websocket_client_controller.cpp
    ${PLUGIN_LIBS_PATH}/network/websocket_client_worker.hpp
    ${PLUGIN_LIBS_PATH}/network/websocket_client_worker.cpp
    ${PLUGIN_LIBS_PATH}/utils/image_utils.hpp
    ${PLUGIN_LIBS_PATH}/utils/image_utils.cpp
    ${PLUGIN_LIBS_PATH}/utils/meyers_singleton.hpp
    ${PLUGIN_LIBS_PATH}/utils/qimage_to_cvmat.hpp
    ${PLUGIN_LIBS_PATH}/utils/qimage_to_cvmat.cpp
)

set(PROJECT_SOURCES
        main.cpp
        ui/mainwindow.cpp
        ui/mainwindow.hpp
        ui/mainwindow.ui
        global/global_keywords.hpp global/global_keywords.cpp
        global/global_object.hpp global/global_object.cpp

        ${CV_ALGO_SOURCES}
        ${MULTIMEDIA_SOURCE}
        ${OTHER_PLUGIN_SOURCES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        config/config_source_selection.hpp
        ui/widget_stacks_manager.hpp ui/widget_stacks_manager.cpp ui/widget_stacks_manager.ui
        ui/widget_stacks_faces_search.hpp ui/widget_stacks_faces_search.cpp ui/widget_stacks_faces_search.ui
        ui/widget_faces_register.hpp ui/widget_faces_register.cpp ui/widget_faces_register.ui
        ui/widget_face_det_model_select.hpp ui/widget_face_det_model_select.cpp ui/widget_face_det_model_select.ui
        ui/widget_face_recognition_model_select.hpp ui/widget_face_recognition_model_select.cpp ui/widget_face_recognition_model_select.ui
        ui/widget_source_selection.hpp ui/widget_source_selection.cpp ui/widget_source_selection.ui
        config/config_face_det_model_select.hpp
        config/config_face_recognition_model_select.hpp
        algo/face_search/face_search_worker.hpp algo/face_search/face_search_worker.cpp
        algo/face_register/face_register_worker.hpp algo/face_register/face_register_worker.cpp
        config/config_face_register_worker.hpp
        algo/face_register/face_register_results.hpp
        algo/face_search/face_search_worker_results.hpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET face_recognition_tasks APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
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

if(ANDROID OR WIN32)
    find_package(OpenCV REQUIRED)
    find_package(ncnn REQUIRED)

    target_include_directories(${PROJECT_NAME} PRIVATE ${PLUGIN_LIBS_PATH})
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
    target_include_directories(${PROJECT_NAME} PRIVATE ${NCNN_PATH}/include/ncnn)
    target_include_directories(${PROJECT_NAME} PRIVATE ${OPENCV_CV_PATH}/include/opencv4)
    target_include_directories(${PROJECT_NAME} PRIVATE ${PLUGIN_LIBS_PATH})
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

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.face_recognition_tasks)
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
    qt_finalize_executable(face_recognition_tasks)
endif()
