import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess

def generate_launch_description():

    livox_lidar_params_file_path = os.path.join(
        get_package_share_directory('livoscan'),
        'livox_ros2_driver_ext',
        'params.yaml'
        # 'livox_lidar_config.json'
    )

    start_ptp4l = ExecuteProcess(
        cmd=['ptp4l', '-i', 'eno1', '-l', '6', '-m'],
        output='screen'
    )

    start_phc2sys = ExecuteProcess(
        cmd=['phc2sys', '-s', 'CLOCK_REALTIME', '-c', 'eno1', '-m', '-O', '0'],
        output='screen'
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
        ),
        start_ptp4l,
        start_phc2sys
     ])

