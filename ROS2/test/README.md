# DONE SO FAR

## https://docs.ros.org/en/rolling/Tutorials/Beginner-Client-Libraries/Creating-Your-First-ROS2-Package.html
. /opt/ros/rolling/setup.zsh 
ros2 pkg create --build-type ament_cmake --node-name my_node first_package
cd first_package
. install/local_setup.zsh
colcon build
ros2 run first_package my_node



