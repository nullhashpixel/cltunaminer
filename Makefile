CC = gcc

CFLAGS = -lm -Ofast -Wno-deprecated-declarations
TARGET = cltuna

$(TARGET): main.cpp sha256.c
	$(CC) main.cpp sha256.c -lOpenCL $(CFLAGS) -o $(TARGET)

clangbuild: main.cpp sha256.c
	clang main.cpp sha256.c -lOpenCL $(CFLAGS) -o $(TARGET)

clean:
	$(RM) $(TARGET)
