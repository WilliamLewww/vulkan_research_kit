VRK_REPOSITORY_PATH=$(dirname $(dirname $(realpath "${BASH_SOURCE[0]}")))

ignored_directories='^.*/(build)$'

mkdir -p ${VRK_REPOSITORY_PATH}/examples/build

for directory in ${VRK_REPOSITORY_PATH}/examples/*
do
  if [[ ! ${directory} =~ ignored_directories ]]
  then
    cmake -H${directory} -B${VRK_REPOSITORY_PATH}/examples/build/$(basename "${directory}")
    make -C ${VRK_REPOSITORY_PATH}/examples/build/$(basename "${directory}")
  fi
done
