cmake_minimum_required(VERSION 2.8.3)
project(toi_bot_vision)


add_compile_options(-std=c++11)


find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
)




find_package(catkin REQUIRED COMPONENTS
  geometry_msgs
  image_transport
  roscpp
  dynamic_reconfigure
  std_msgs
  cv_bridge
  message_generation

)

find_package(OpenCV REQUIRED)


include_directories(
  include
  ${catkin_INCLUDE_DIRS} 
  ${OpenCV_INCLUDE_DIRS}


)

catkin_package()


include(/home/gal/toibot_ws/src/ToiBot1/src/toi_bot_vision/dlib/cmake)


add_executable(toi_bot_vision_node
    src/main.cpp
    src/FaceTracker.cpp
    src/FoldersManagment.cpp
    src/personManager.cpp
    src/emotionDetection.cpp
		
  )

  add_dependencies(toi_bot_vision_node
    ${${PROJECT_NAME}_EXPORTED_TARGETS}
    ${catkin_EXPORTED_TARGETS}
  )

  target_link_libraries(toi_bot_vision_node
    ${catkin_LIBRARIES}
    ${OpenCV_LIBRARIES}
    dlib 
  )




