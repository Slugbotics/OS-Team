#!/bin/sh

sudo apt update
sudo apt install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo

mkdir $HOME/opt
mkdir $HOME/opt/cross
mkdir $HOME/src

export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

cd $HOME/src
wget https://ftp.gnu.org/gnu/binutils/binutils-2.7.tar.gz
tar -xzf binutils-2.7.tar.gz
rm binutils-2.7.tar.gz
mkdir build-binutils
cd build-binutils
../binutils-2.7/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make
make install

cd $HOME/src
wget https://ftp.gnu.org/gnu/gcc/gcc-1.42.tar.gz
tar -xzf gcc-1.42.tar.gz
rm gcc-1.42.tar.gz
mkdir build-gcc
cd build-gcc
../gcc-1.42/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc