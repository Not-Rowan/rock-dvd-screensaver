#!/bin/bash

# copy the framework to the library folder
sudo cp -r SDL2.framework /Library/Frameworks/

# 64-bit macOS
sudo gcc main.c -o therock_64Mac -Wall -F/Library/Frameworks -framework SDL2 -rpath /Library/Frameworks
