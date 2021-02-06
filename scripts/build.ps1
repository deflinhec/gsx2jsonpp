$VCPKG_ARGS= Get-Content('../dependencies/vcpkg-x64-windows.txt')

# 1. run-vcpkg
vcpkg.exe --vcpkg-root ..\vcpkg install ${VCPKG_ARGS}

# 2. configure cmake
cmake -DCMAKE_TOOLCHAIN_FILE='vcpkg/scripts/buildsystems/vcpkg.cmake' `
-DCMAKE_INSTALL_PREFIX='..\' `
-B ..\build ..\ 

# 3. cmake build
cmake --build ..\build --config Release --target install