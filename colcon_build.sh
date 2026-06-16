#!/bin/bash
set -e

_Bold_Green='\e[1;32m'
_None='\e[0m'

printf "%b|==========Colcon Build Start==========|%b\n" "$_Bold_Green" "$_None"

colcon build \
    --symlink-install \
    --cmake-args -DCMAKE_BUILD_TYPE=Release

printf "%b|==========Colcon Build End============|%b\n" "$_Bold_Green" "$_None"

exec "$@"
