
TOOLCHAIN_PATH=${NDK_PATH}/toolchains/llvm/prebuilt/linux-x86_64/bin

: ${TOOLCHAIN_PREFIX:=x86_64-linux-android}

: ${API:=30}

: ${ARCH:=x86_64}

PATH=${PATH}:${TOOLCHAIN_PATH}

make ANDROID=1 ARCH=${ARCH} CC=${TOOLCHAIN_PREFIX}${API}-clang LD=${TOOLCHAIN_PREFIX}-ld $@
