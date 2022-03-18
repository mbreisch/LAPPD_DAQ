#!/bin/bash

mkdir -p ToolDAQ
cd ToolDAQ #into ToolDAQ

git clone https://github.com/yoctopuce/yoctolib_cpp.git YOCTO
cd YOCTO #into YOCTO
./build.sh armhf
cd ../..
