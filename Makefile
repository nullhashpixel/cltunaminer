CC = g++

CFLAGS = -std=c++11 -lm -Ofast -Wno-deprecated-declarations
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

$(TARGET): main.cpp sha256.cpp
	$(CC) main.cpp sha256.cpp $(LDFLAGS) $(CFLAGS) -o $(TARGET)

clangbuild: main.cpp sha256.cpp
	clang main.cpp sha256.cpp $(LDFLAGS) $(CFLAGS) -lstdc++ -o $(TARGET)

clean:
	$(RM) $(TARGET)
