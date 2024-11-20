
SRC_ROOT	= ${PWD}

export SRC_ROOT


PUSCON_VERSION_MAJOR	:= 0
PUSCON_VERSION_MINOR	:= 0
PUSCON_VERSION_PATCH	:= 0

export PUSCON_VERSION_MAJOR PUSCON_VERSION_MINOR PUSCON_VERSION_PATCH


ARCH		?= x86_64

export ARCH


AR		?= ar
CC		?= cc
LD		?= ld

export AR CC LD


CFLAGS	+= -I${PWD}/include/ -I${PWD}/include/arch/${ARCH}/ -fPIC -Wall -Wno-unused

ifdef DEBUG
CFLAGS	+= -g
endif

ifdef ANDROID
CFLAGS	+= -D__android__
endif

export	CFLAGS
