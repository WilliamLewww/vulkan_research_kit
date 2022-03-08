VRK_REPOSITORY_PATH=$(dirname $(dirname $(realpath "${BASH_SOURCE[0]}")))

ignored_directories='^.*/(build|vulkan_development)/.*$'
ignored_files='^.*/(stb_image.h|tiny_obj_loader.h)$'
extensions='^.*\.(h|cpp|vert|frag|comp|rgen|rchit|rmiss)$'

for file in $(find ${VRK_REPOSITORY_PATH} -type f)
do
  if [[ ! $file =~ $ignored_directories && ! $file =~ $ignored_files ]]
  then
    if [[ $file =~ $extensions ]]
    then
      clang-format --style="{BasedOnStyle: llvm, ColumnLimit: 80}" -i ${file}
    fi
  fi
done
