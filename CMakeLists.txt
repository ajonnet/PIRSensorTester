cmake_minimum_required(VERSION 2.8)
SET(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH "/usr/local/lib/cmake/;${CMAKE_MODULE_PATH}")

project(PIRSensorTester)

find_package(OpenCV REQUIRED)
find_package(pigpio REQUIRED)
find_package(raspicam REQUIRED)

file(GLOB SOURCES "PIRSensorTester/*.cpp" "src/irslinger.h")

add_executable(PIRSensorTester ${SOURCES})
target_link_libraries(PIRSensorTester ${OpenCV_LIBS})
target_link_libraries(PIRSensorTester ${pigpio_LIBRARY})
target_link_libraries(PIRSensorTester ${raspicam_CV_LIBS})