#!/bin/bash
export LIB_PATH=~/.local
cd ~
wget https://invisible-island.net/datafiles/release/ncurses.tar.gz
tar xzf ncurses.tar.gz
cd $(find . -iname 'ncurses*' -type d | head -n 1)
./configure --enable-widec --enable-ext-colors --with-default-terminfo=xterm-256color --with-ncursesw --with-shared  --with-cxx-shared --prefix=$LIB_PATH
make && make install
