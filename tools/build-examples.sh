#!/bin/bash
set -e

VRK_REPOSITORY_PATH=$(dirname $(dirname $(realpath "${BASH_SOURCE[0]}")))

for i in "$@"
do
case $i in
  -DCMAKE_PREFIX_PATH=*)
  CMAKE_PREFIX_PATH="${i#*=}"
  ;;
  *)
  ;;
esac
done

ignored_directories='^.*/(build)$'
mkdir -p ${VRK_REPOSITORY_PATH}/examples/build
for directory in ${VRK_REPOSITORY_PATH}/examples/*
do
  if [[ ! ${directory} =~ ignored_directories ]]
  then
    if [[ ${CMAKE_PREFIX_PATH} == "" ]]
    then
      cmake -H${directory} -B${VRK_REPOSITORY_PATH}/examples/build/$(basename "${directory}")
    else
      cmake -H${directory} -B${VRK_REPOSITORY_PATH}/examples/build/$(basename "${directory}") -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
    fi
    make -C ${VRK_REPOSITORY_PATH}/examples/build/$(basename "${directory}")
  fi
done
