#!/bin/bash
set -e

_Bold_Green='\e[1;32m'
_None='\e[0m'

printf "%b|==========Clearing Cache Start==========|%b\n" "$_Bold_Green" "$_None"

rm -rf build/ install/ log/

printf "%b|==========Clearing Cache End============|%b\n" "$_Bold_Green" "$_None"

exec "$@"
