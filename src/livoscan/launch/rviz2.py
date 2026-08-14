import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    # Get the path to your package's share directory
    pkg_dir = get_package_share_directory('livoscan')
    
    # Define the path to the .rviz file you saved
    rviz_config_file = os.path.join(pkg_dir, 'rviz2', 'livoscan_rviz2_config.rviz')

    # Create the RViz2 node
    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        arguments=['-d', rviz_config_file],
        output='screen'
    )

    return LaunchDescription([
        rviz_node
    ])
