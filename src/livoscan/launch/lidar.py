import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():

    livox_lidar_params_file_path = os.path.join(
        get_package_share_directory('livoscan'),
        'livox_ros2_driver_ext',
        'params.yaml'
        # 'livox_lidar_config.json'
    )
    
    return LaunchDescription([
        Node(
            package='livox_ros2_driver_ext',
            executable='livox_ros2_driver_ext_node',
            name='livox_lidar_publisher',
            parameters=[livox_lidar_params_file_path]
        ),
        Node(
            package='livoscan',
            executable='livox_interface_convert',
            name='livox_convert',
        ) 
     ])

