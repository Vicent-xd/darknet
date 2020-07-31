SHELL = /bin/bash

CC           := gcc
CPP          := g++
LD           := ld
AR           := ar
STRIP        := strip


CFLAGS += -c -g -Wall -DWITH_DOM -DWITH_OPENSSL -DDEBUG
CFLAGS += $(INCLUDE)

# openssl目录名
OPENSSL_DIR = /usr/local/openssl
# 源文件
SOURCES_ONVIF += \
           ../onvif/soapC.c                          \
           ../onvif/soapClient.c                     \
           ../onvif/stdsoap2.c                       \
           ../onvif/wsaapi.c                         \
           ../onvif/dom.c                            \
           ../onvif/mecevp.c                         \
           ../onvif/smdevp.c                         \
           ../onvif/threads.c                        \
           ../onvif/wsseapi.c                        \
           ../onvif/duration.c                       \


# 目标文件
OBJECTS_ONVIF := $(patsubst %.c,$(TEMPDIR)%.o,$(filter %.c, $(SOURCES_ONVIF)))

# 头文件路径
INCLUDE += -I../onvif/                               \
           -I$(OPENSSL_DIR)/include                  \

# 静态库链接OpenSSL
LDLIBS += $(OPENSSL_DIR)/lib/libssl.a                \
          $(OPENSSL_DIR)/lib/libcrypto.a             \
          -ldl                                       \

# 链接库(其他)
LDLIBS += -lpthread

%.o: %.cpp
	@echo "  CPP     " $@;
	@$(CPP) $(CFLAGS) -c -o $@ $<

%.o: %.c
	@echo "  CC      " $@;
	@$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: all clean
