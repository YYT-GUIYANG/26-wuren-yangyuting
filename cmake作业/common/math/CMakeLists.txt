find_package(OpenCV REQUIRED)
add_library(math STATIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Math.cpp")
target_include_directories(math PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" ${OpenCV_INCLUDE_DIRS})
target_link_libraries(math PUBLIC ${OpenCV_LIBS})
