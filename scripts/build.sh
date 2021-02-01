#!/bin/bash

BUILD_TYPE="$1"

if [[ ${BUILD_TYPE} == "" ]]; then
  BUILD_TYPE="Release"
fi

if [[ ${BUILD_TYPE} == "*" ]]; then
  BUILD_TYPE="Release Debug Project"
fi

if [ ! -f "${PWD}/build" ]; then
  mkdir -p "${PWD}/build"
fi

if [[ ${BUILD_TYPE} == *"Release"* ]]; then
  cmake -H${PWD} \
  -B${PWD}/build/unix \
  -DCMAKE_INSTALL_PREFIX=${PWD} \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_LIBDIR=${PWD}/lib/Release
  cmake --build ${PWD}/build/unix --target install test
fi

if [[ ${BUILD_TYPE} == *"Debug"* ]]; then
  cmake -H${PWD} \
  -B${PWD}/build/unix-d \
  -DCMAKE_INSTALL_PREFIX=${PWD}/ \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_INSTALL_LIBDIR=${PWD}/lib/Debug
  cmake --build ${PWD}/build/unix-d --target install test
fi

case "$(uname)" in
  Darwin*)

    if [[ ${BUILD_TYPE} == *"Project"* ]]; then
      cmake -H${PWD}/ \
      -B${PWD}/build/proj -GXcode \
      -DCMAKE_INSTALL_PREFIX=${PWD}/ \
      -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_INSTALL_LIBDIR=${PWD}/lib/Debug
    fi

    ;;
  *) ;;
esac
