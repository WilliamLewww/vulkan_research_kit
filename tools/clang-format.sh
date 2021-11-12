VRK_REPOSITORY_PATH=$(dirname $(dirname $(realpath "${BASH_SOURCE[0]}")))

clang-format --style="{BasedOnStyle: llvm, ColumnLimit: 80}" -i ${VRK_REPOSITORY_PATH}/include/vrk/*.h
clang-format --style="{BasedOnStyle: llvm, ColumnLimit: 80}" -i ${VRK_REPOSITORY_PATH}/include/vrk/*/*.h

clang-format --style="{BasedOnStyle: llvm, ColumnLimit: 80}" -i ${VRK_REPOSITORY_PATH}/src/*.cpp
clang-format --style="{BasedOnStyle: llvm, ColumnLimit: 80}" -i ${VRK_REPOSITORY_PATH}/src/*/*.cpp
