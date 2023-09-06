CC = gcc

CFLAGS = -lm -Ofast -Wno-deprecated-declarations
TARGET = cltuna

# Detect the OS
UNAME_S := $(shell uname -s)

# Linux specific flags and libraries
ifeq ($(UNAME_S),Linux)
    LDFLAGS += -lOpenCL
endif

# Mac specific flags and libraries
ifeq ($(UNAME_S),Darwin)
    LDFLAGS += -framework OpenCL
endif

$(TARGET): main.cpp sha256.c
	$(CC) main.cpp sha256.c $(LDFLAGS) $(CFLAGS) -o $(TARGET)

clangbuild: main.cpp sha256.c
	clang main.cpp sha256.c $(LDFLAGS) $(CFLAGS) -o $(TARGET)

clean:
	$(RM) $(TARGET)
