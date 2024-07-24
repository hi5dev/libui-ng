#!/usr/bin/env bash

# ensure system is upgraded in the event that the same command run by the PowerShell script had to restart MSYS2
pacman --noconfirm --needed -Suy

# https://www.msys2.org/docs/environments/
menv="mingw-w64-x86_64"

# install build tools and libraries required by this project
pacman --noconfirm --needed -S "$menv"-cmake "$menv"-toolchain

# for whatever reason, the developers of Make expect you to symlink it manually
if [ ! -f "/usr/bin/make.exe" ]; then
  ln -s "$(which mingw32-make.exe)" "/usr/bin/make.exe"
fi
