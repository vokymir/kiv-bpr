#!/bin/env bash

echo "
#J# run this at the same level CMake is placed
"

echo "
#J# GIT ensure submodules (external libs) are there
"
# download all external libs
git submodule init
git submodule update

# if already exist only update
if [ ! -d "vcpkg/.git" ]; then
  echo "#J# VCPKG cloning for the first time"
  git clone https://github.com/microsoft/vcpkg.git vcpkg
  cd vcpkg
  ./bootstrap-vcpkg.sh -disableMetrics
  cd ..
else
  echo "#J# VCPKG update"
  cd vcpkg
  git pull
  cd ..
fi

echo "
#J# VCPKG symlink to .local/share (fails if already exist)
"

# here it should be, so link it
ln -s "$PWD/vcpkg" "$HOME/.local/share/vcpkg"

echo "
#J#
add this to \$PATH: $PWD/vcpkg
this process differs based on current shell

  BASH: append to ~/.bashrc
    export VCPKG_ROOT=$PWD/vcpkg
    export PATH=\$VCPKG_ROOT:\$PATH

  FISH: create ~/.config/fish/conf.d/vcpkg.fish
    set -xU VCPKG_ROOT $PWD/vcpkg
    set -U fish_user_paths \$VCPKG_ROOT \$fish_user_paths

"

echo "
#J# PACMAN install needed packages for this project
"

# dependencies for vcpkg & external libs
sudo pacman -Syu gcc gcc-fortran autoconf autoconf-archive automake libtool ninja cmake pkg-config 

echo "
#J# AUTO/BUILD script
"

./auto/build.sh
