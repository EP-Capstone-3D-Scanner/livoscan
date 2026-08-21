#!/bin/bash
set -e

# 1. Set the OS UDP buffers (works because of your NOPASSWD sudo rule)
sudo sysctl -w net.core.rmem_max=2147483647
sudo sysctl -w net.core.rmem_default=2147483647

# 2. Source the ROS environment (replacing the default ROS entrypoint)
source /opt/ros/humble/setup.bash

# 3. Hand off control to the command specified in docker-compose (e.g., ./colcon_build.sh)
exec "$@"
