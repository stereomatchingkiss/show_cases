cmake_minimum_required(VERSION 3.5)

project(pose_tasks VERSION 0.1 LANGUAGES CXX)

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

set(CV_ALGO_SOURCES ${PLUGIN_LIBS_PATH}/cv_algo/pose/movenet_single_pose_estimate.hpp ${PLUGIN_LIBS_PATH}/cv_algo/pose/movenet_single_pose_estimate.cpp
    ${PLUGIN_LIBS_PATH}/cv_algo/pose/pose_estimation_results.hpp
    ${PLUGIN_LIBS_PATH}/cv_algo/pose/pose_estimation_utils.hpp ${PLUGIN_LIBS_PATH}/cv_algo/pose/pose_estimation_utils.cpp
    ${PLUGIN_LIBS_PATH}/cv_algo/pose/pose_similarity_estimation.hpp ${PLUGIN_LIBS_PATH}/cv_algo/pose/pose_similarity_estimation.cpp
    ${PLUGIN_LIBS_PATH}/cv_algo/pose/pose_similarity_search.hpp ${PLUGIN_LIBS_PATH}/cv_algo/pose/pose_similarity_search.cpp
    ${PLUGIN_LIBS_PATH}/cv_algo/pose/pose_similarity_fast_search.hpp ${PLUGIN_LIBS_PATH}/cv_algo/pose/pose_similarity_fast_search.cpp
    ${PLUGIN_LIBS_PATH}/cv_algo/pose/pose_similarity_search_base.hpp ${PLUGIN_LIBS_PATH}/cv_algo/pose/pose_similarity_search_base.cpp
    ${PLUGIN_LIBS_PATH}/cv_algo/pose/pose_similarity_search_results.hpp
    ${PLUGIN_LIBS_PATH}/utils/image_utils.hpp ${PLUGIN_LIBS_PATH}/utils/image_utils.cpp
    ${PLUGIN_LIBS_PATH}/utils/qimage_to_cvmat.hpp ${PLUGIN_LIBS_PATH}/utils/qimage_to_cvmat.cpp    
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

set(PROJECT_SOURCES
        main.cpp
        algo/estimate_many_pose_similarity_worker.hpp algo/estimate_many_pose_similarity_worker.cpp
        algo/estimate_many_pose_similarity_worker_results.hpp
        algo/estimate_pose_similarity_worker_input.hpp
        algo/estimate_pose_similarity_worker_results.hpp
        algo/estimate_pose_similarity_worker.hpp algo/estimate_pose_similarity_worker.cpp
        algo/pose_estimation_common_func.hpp algo/pose_estimation_common_func.cpp
        algo/pose_estimation_worker.hpp algo/pose_estimation_worker.cpp
        algo/pose_estimation_worker_results.hpp
        config/config_estimate_many_pose_similarity_worker.hpp config/config_estimate_many_pose_similarity_worker.cpp
        config/config_pose_estimation_worker.hpp
        config/config_source_selection.hpp
        config/config_tasks_selection.hpp
        config/pose_estimation_model_enum.hpp        
        global/global_object.hpp global/global_object.cpp
        ui/mainwindow.hpp ui/mainwindow.cpp ui/mainwindow.ui        
        ui/widget_estimate_many_pose_similarity_params.hpp ui/widget_estimate_many_pose_similarity_params.cpp ui/widget_estimate_many_pose_similarity_params.ui
        ui/widget_estimate_similar_poses_player.hpp ui/widget_estimate_similar_poses_player.cpp ui/widget_estimate_similar_poses_player.ui
        ui/widget_image_pair_player.hpp ui/widget_image_pair_player.cpp ui/widget_image_pair_player.ui
        ui/widget_image_player.hpp ui/widget_image_player.cpp ui/widget_image_player.ui
        ui/widget_pose_estimation_params.hpp ui/widget_pose_estimation_params.cpp ui/widget_pose_estimation_params.ui
        ui/widget_source_selection.hpp ui/widget_source_selection.cpp ui/widget_source_selection.ui
        ui/widget_stacks_estimate_pose_similarity.hpp ui/widget_stacks_estimate_pose_similarity.cpp ui/widget_stacks_estimate_pose_similarity.ui
        ui/widget_stacks_manager.hpp ui/widget_stacks_manager.cpp ui/widget_stacks_manager.ui
        ui/widget_stacks_pose_estimation.hpp ui/widget_stacks_pose_estimation.cpp ui/widget_stacks_pose_estimation.ui
        ui/widget_stream_player.hpp ui/widget_stream_player.cpp ui/widget_stream_player.ui
        ui/widget_tasks_selection.hpp ui/widget_tasks_selection.cpp ui/widget_tasks_selection.ui        
        ${CV_ALGO_SOURCES}
        ${MULTIMEDIA_SOURCE}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}        
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
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

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.${PROJECT_NAME})
endif()

#add_definitions(-DANNOYLIB_MULTITHREADED_BUILD)
target_include_directories(${PROJECT_NAME} PRIVATE ${3RDLIBS_PATH}/annoy/src/)
target_include_directories(${PROJECT_NAME} PRIVATE ${PLUGIN_LIBS_PATH})
target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_LIBS_PATH})

if(ANDROID OR WIN32)
    find_package(OpenCV REQUIRED)
    find_package(ncnn REQUIRED)               

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
    #add_compile_definitions(EIGEN_DONT_PARALLELIZE)    
    target_include_directories(${PROJECT_NAME} PRIVATE ${NCNN_PATH}/include/ncnn)
    target_include_directories(${PROJECT_NAME} PRIVATE ${OPENCV_CV_PATH}/include/opencv4)

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
