#!/bin/bash
set -e
prev=$(pwd)

if [ ! -d "~/.local/include/ncursesw" ]; then
    export LIB_PATH=~/.local
    cd ~
    wget https://invisible-island.net/datafiles/release/ncurses.tar.gz
    tar xzf ncurses.tar.gz
    cd $(find . -maxdepth 1 -iname 'ncurses*' -type d  | head -n 1)
    ./configure --enable-widec --enable-ext-colors --with-default-terminfo=xterm-256color --with-ncursesw --with-shared  --with-cxx-shared --prefix=$LIB_PATH
    make && make install
fi;
if [ ! -d "~/FreeRTOSv202212.01" ]; then
    cd ~
    if [ ! -f "~/FreeRTOSv202212.01.zip" ]; then
        wget https://github.com/FreeRTOS/FreeRTOS/releases/download/202212.01/FreeRTOSv202212.01.zip
    fi;
    unzip -q FreeRTOSv202212.01.zip
    echo "export FREERTOS_PATH=~/FreeRTOSv202212.01" >> ~/.profile
fi;
cd $prev
. ./setenv.sh
