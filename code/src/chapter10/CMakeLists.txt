cmake_minimum_required(VERSION 2.8)

project(Chapter10)

SET(COMPILE_DEFINITIONS -Werror)

find_package( OpenCV REQUIRED )

include_directories(/opt/vc/include)
include_directories(/opt/vc/include/interface/vcos/pthreads)
include_directories(/opt/vc/include/interface/vmcs_host)
include_directories(/opt/vc/include/interface/vmcs_host/linux)
include_directories(/home/samarth/libraries/userland)
include_directories(/opt/vc/src/hello_pi/libs/vgfont)
include_directories("${PROJECT_SOURCE_DIR}/include")

link_directories(/opt/vc/lib)
link_directories(/opt/vc/src/hello_pi/libs/vgfont)

add_executable(code10-1 src/code10-1.cpp)
add_executable(code10-2 src/code10-2.cpp src/PiCapture.cpp)
add_executable(code10-3 src/code10-3.cpp src/PiCapture.cpp)
add_executable(code10-4 src/code10-4.cpp src/PiCapture.cpp)

target_link_libraries(code10-1 ${OpenCV_LIBS})
target_link_libraries(code10-2 mmal_core mmal_util mmal_vc_client bcm_host ${OpenCV_LIBS} openmaxil EGL)
target_link_libraries(code10-3 mmal_core mmal_util mmal_vc_client bcm_host ${OpenCV_LIBS} openmaxil EGL)
target_link_libraries(code10-4 mmal_core mmal_util mmal_vc_client bcm_host ${OpenCV_LIBS} openmaxil EGL)
