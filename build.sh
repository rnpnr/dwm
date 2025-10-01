#!/bin/sh

version="$(git describe --dirty --always)"

cc=${CC:-cc}

${cc} -march=native -O3 -std=c99 -pedantic \
      -Wall -Wno-deprecated-declarations \
      -D _DEFAULT_SOURCE -D _XOPEN_SOURCE=700L \
      -D XINERAMA \
      -D VERSION=\"${version}\" \
      -I/usr/include/freetype2 \
      -lX11 -lXinerama -lfontconfig -lXft \
      unity.c -o dwm
