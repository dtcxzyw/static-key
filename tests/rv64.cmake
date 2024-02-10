set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_C_COMPILER riscv64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER riscv64-linux-gnu-g++)
set(STATIC_KEY_RUNNER qemu-riscv64 -L /usr/riscv64-linux-gnu)