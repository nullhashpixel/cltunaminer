#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#define CL_TARGET_OPENCL_VERSION    220

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define npar (1<<21)


#define SHA256_FORMAT_LABEL			"sha256-opencl"
#define SHA256_FORMAT_NAME			"SHA-256"

#define SHA256_PLAINTEXT_LENGTH		64

#define SHA256_BINARY_SIZE			32
#define SHA256_RESULT_SIZE			16

#define MAX_SOURCE_SIZE 0x10000000

#ifdef __cplusplus
extern "C" {
#endif

void listDevices();
void sha256_init(int,int);
void sha256_crypt(char*,int,int,int,int, char*);

#ifdef __cplusplus
}
#endif
