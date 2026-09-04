import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource

def generate_launch_description():
    # 1. Get the path to your custom package
    my_pkg_dir = get_package_share_directory('livoscan')
    
    # 2. Define the path to your custom YAML config file
    custom_config_path = os.path.join(my_pkg_dir, 'zed_ros2_wrapper', 'zed2i.yaml')
    
    # 3. Get the path to the official zed_wrapper package
    zed_wrapper_dir = get_package_share_directory('zed_wrapper')
    
    # 4. Include the official ZED launch file and pass your config
    zed_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(zed_wrapper_dir, 'launch', 'zed_camera.launch.py')
        ),
        launch_arguments={
            'config_path': custom_config_path
        }.items()
    )

    return LaunchDescription([
        zed_launch
    ])
