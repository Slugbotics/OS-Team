# OS Team
## Setup
In a Linux environment, run `./build_cross_compiler.sh`. This will download the code for Binutils and GCC, configure them to target the i686 architecture with no OS, then compile them. This is the compiler toolchain that will be used to compile the kernel.

Next, add `export PATH="$HOME/opt/cross/bin:$PATH"` to your `.bashrc`. This will add the newly build binaries to your path so that they can be used from the terminal.

## Compiling the kernel
To compile the kernel, run `make` in the root of the project. If you add more files you will likely have to change the `Makefile`.

## Running the kernel in QEMU
QEMU can be installed using:
```
sudo apt-get install qemu-system
```
To run the kernel, use:
```
qemu-system-x86_64 -kernel kernel.o
```
If you have trouble running QEMU in WSL, the Windows version can be downloaded [here](https://www.qemu.org/download/#windows).