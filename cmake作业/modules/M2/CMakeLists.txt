find_package(OpenCV REQUIRED)
add_library(M2 STATIC "${CMAKE_CURRENT_SOURCE_DIR}/src/M2.cpp")
target_include_directories(M2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" ${OpenCV_INCLUDE_DIRS})
target_link_libraries(M2 PUBLIC A1 A2 kalman ${OpenCV_LIBS})
