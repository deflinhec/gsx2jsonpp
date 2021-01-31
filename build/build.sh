#!/bin/bash

BUILD_TYPE="$1"

if [[ $BUILD_TYPE == "" ]]; then
  BUILD_TYPE="Release Debug Project"
fi

if [[ $BUILD_TYPE == *"Release"* ]]; then
  cmake -H../ -Bunix \
  -DCMAKE_INSTALL_PREFIX=../ \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_LIBDIR=lib/Release \
  -DCMAKE_OSX_DEPLOYMENT_TARGET="10.11" &&
  cmake --build unix --target install test
fi

if [[ $BUILD_TYPE == *"Debug"* ]]; then
  cmake -H../ -Bunix-d \
  -DCMAKE_INSTALL_PREFIX=../ \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_INSTALL_LIBDIR=lib/Debug \
  -DCMAKE_OSX_DEPLOYMENT_TARGET="10.11" &&
  cmake --build unix-d --target install test
fi

case "$(uname)" in
  Darwin*)

    if [[ $BUILD_TYPE == *"Project"* ]]; then
      cmake -H../ -Bproj -GXcode \
      -DCMAKE_INSTALL_PREFIX=../ \
      -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_INSTALL_LIBDIR=lib/Debug
    fi

    ;;
  *) ;;
esac
