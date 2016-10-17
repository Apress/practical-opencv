# Minimum required CMake version
cmake_minimum_required(VERSION 2.8)

# Project name
project(Panorama)

# Find the OpenCV installation
find_package(OpenCV REQUIRED)

# Find the Boost installation, specifically the components 'system' and 'filesystem'
find_package(Boost COMPONENTS system filesystem REQUIRED)

# ${PROJECT_SOURCE_DIR} is the name of the root directory of the project
# TO_NATIVE_PATH converts the path ${PROJECT_SOURCE_DIR}/data/ to a full path and the file() command stores it in DATA_FOLDER
file(TO_NATIVE_PATH "${PROJECT_SOURCE_DIR}/data/data1/" DATA_FOLDER_1)
file(TO_NATIVE_PATH "${PROJECT_SOURCE_DIR}/data/data2/" DATA_FOLDER_2)
file(TO_NATIVE_PATH "${PROJECT_SOURCE_DIR}/data/data3/" DATA_FOLDER_3)
file(TO_NATIVE_PATH "${PROJECT_SOURCE_DIR}/data/data5/" DATA_FOLDER_5)

# set the configuration input file to ${PROJECT_SOURCE_DIR}/Config.h.in and the includable header file holding configuration information to ${PROJECT_SOURCE_DIR}/include/Config.h
configure_file("${PROJECT_SOURCE_DIR}/Config.h.in" "${PROJECT_SOURCE_DIR}/include/Config.h")

# Other directories where header files for linked libraries can be found
include_directories(${OpenCV_INCLUDE_DIRS} "${PROJECT_SOURCE_DIR}/include" ${Boost_INCLUDE_DIRS})

# executable produced as a result of compilation
add_executable(code11-1 src/code11-1.cpp)
add_executable(code11-2 src/code11-2.cpp)
add_executable(code11-3 src/code11-3.cpp)
add_executable(code11-4 src/code11-4.cpp)
add_executable(code11-5 src/code11-5.cpp)
add_executable(code11-6 src/code11-6.cpp)

# libraries to be linked with this executable - OpenCV
target_link_libraries(code11-1 ${OpenCV_LIBS})
target_link_libraries(code11-2 ${OpenCV_LIBS})
target_link_libraries(code11-3 ${OpenCV_LIBS})
target_link_libraries(code11-4 ${OpenCV_LIBS})
target_link_libraries(code11-5 ${OpenCV_LIBS} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})
target_link_libraries(code11-6 ${OpenCV_LIBS} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})
