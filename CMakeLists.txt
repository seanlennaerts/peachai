cmake_minimum_required(VERSION 2.8)

project( peachai )

# change path depending on your setup
set(OpenCV_DIR C:/OpenCV-3.4.1/opencv/build)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( record_training_data record_training_data.cpp)
target_link_libraries( record_training_data ${OpenCV_LIBS} )

add_executable( read_training_data read_training_data.cpp)
target_link_libraries( read_training_data ${OpenCV_LIBS} )

# add_executable( peachai peachai.cpp)

add_executable( train_model train_model.cpp )
