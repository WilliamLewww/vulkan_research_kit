VRK_REPOSITORY_PATH=$(dirname $(realpath "${BASH_SOURCE[0]}"))

if [ -d $VRK_REPOSITORY_PATH/build ]
then
  VRK_RESOURCES_PATH=$VRK_REPOSITORY_PATH/build/resources/
  export VRK_RESOURCES_PATH
fi