#!/bin/bash
set -e

echo "=======Building packages======="

colcon build \
    --symlink-install \
    --cmake-args -DCMAKE_BUILD_TYPE=Release

exec "$@"
