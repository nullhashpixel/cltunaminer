#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable:4996)
#endif

#include "sha256.h"
#include <random>

static cl_platform_id platform_id = NULL;
static cl_device_id device_id = NULL;  
static cl_uint ret_num_devices;
static cl_uint ret_num_platforms;
static cl_context context;

static cl_int ret;

static char* source_str;
static size_t source_size;

static cl_program program;
static cl_kernel kernel;
static cl_command_queue command_queue;
cl_device_id *devices;

static cl_mem pinned_saved_keys, pinned_partial_hashes, buffer_out, buffer_keys, data_info;
static cl_uint *partial_hashes;
static cl_uint *res_hashes;
static char *saved_plain;
static unsigned int datai[3];
static int have_full_hashes;

static size_t input_stride = 68;
static int result_size = 16;

static size_t local_work_size=1;
static size_t string_len;

void load_source();
void createDevice(int, int);
void createkernel();
void create_clobj();

void crypt_all();

std::mt19937 mt{ std::random_device{}() };

void sha256_init(int platform_idx, int device_idx) {
	load_source();
	createDevice(platform_idx, device_idx);
	createkernel();
	create_clobj();
}

void prepare_result(char* output, uint64_t j) {
    for(int i=0; i<67;i++) {
        if(i==16) {
            sprintf(output+i*2, "%02x", (unsigned char)(saved_plain[i] + partial_hashes[result_size*j+8] & 0xff)); //inner-loop 
        } else if(i==17) {
            sprintf(output+i*2, "%02x", (unsigned char)((j & 0xff0000) >> 16));                                    //outer loop
        } else if(i==18) {
            sprintf(output+i*2, "%02x", (unsigned char)((j & 0xff00) >> 8));                                       //|
        } else if(i==19) {
            sprintf(output+i*2, "%02x", (unsigned char)((j & 0xff)));                                              //|
        } else {
            sprintf(output+i*2, "%02x", (unsigned char)(saved_plain[i]));
        }
    }   
    sprintf(output+67*2, ":");
    for(int i=0; i<8; i++)
    {
        sprintf(output+67*2+1+i*8, "%08x", partial_hashes[result_size*j+i]);
    }
}

void sha256_crypt(char* input, int string_len, int j_offset, int LZ, int DN, char* output)
{
	int i;

	datai[0] = SHA256_PLAINTEXT_LENGTH;
	datai[1] = 1; 
	datai[2] = string_len;

    uint32_t r1 = mt();
    uint32_t r2 = mt();
    uint32_t r3 = mt();

    memcpy(saved_plain, input, string_len+1);
    memset(saved_plain+4,0,16);

    // prepare NONCE

    saved_plain[4]  = (r1>>24)&0xff;                          // random nonce, randomized for each call to sha256_crypt
    saved_plain[5]  = (r1>>16)&0xff; 
    saved_plain[6]  = (r1>>8)&0xff; 
    saved_plain[7]  = (r1)&0xff; 
    saved_plain[8]  = (r2>>24)&0xff; 
    saved_plain[9]  = (r2>>16)&0xff; 
    saved_plain[10] = (r2>>8)&0xff; 
    saved_plain[11] = (r2)&0xff;
    saved_plain[12] = (r3>>24)&0xff;
    saved_plain[13] = (r3>>16)&0xff;
    saved_plain[14] = (r3>>8)&0xff;
    saved_plain[15] = (r3)&0xff;
    saved_plain[16] = 0;                           // |inner-most loop counter will be incremented on GPU in each thread
    saved_plain[17] = 0;                           // |outer loop will be taken from GPU global work index
    saved_plain[18] = 0;                           // |
    saved_plain[19] = 0;                           // |

	crypt_all();

    if (LZ == 5) {
        for (int j=0;j<npar;j++) {
        if (partial_hashes[result_size*j] < ((DN & 0xfff0)>>4) && partial_hashes[result_size*j+1] < ((DN & 0x000f)<<28)) {
                prepare_result(output, j);
            }
        }
    } else if (LZ == 6) {
        for (int j=0;j<npar;j++) {
        if (partial_hashes[result_size*j] < ((DN & 0xff00)>>8) && partial_hashes[result_size*j+1] < ((DN & 0x00ff)<<24)) {
                prepare_result(output, j);
            }
        }
    } else if (LZ == 7) {
        for (int j=0;j<npar;j++) {
        if (partial_hashes[result_size*j] < ((DN & 0xf000)>>12) && partial_hashes[result_size*j+1] < ((DN & 0x0fff)<<20)) {
                prepare_result(output, j);
            }
        }
    } else if (LZ == 8) {
        for (int j=0;j<npar;j++) {
            if (partial_hashes[result_size*j] == 0 && partial_hashes[result_size*j+1] < ((DN & 0xffff)<<16)) {
                prepare_result(output, j);
            }
        }
    } else if (LZ == 9) {
        for (int j=0;j<npar;j++) {
            if (partial_hashes[result_size*j] == 0 && partial_hashes[result_size*j+1] < ((DN & 0xffff)<<12)) {
                prepare_result(output, j);
            }
        }
    } else if (LZ == 10) {
        for (int j=0;j<npar;j++) {
            if (partial_hashes[result_size*j] == 0 && partial_hashes[result_size*j+1] < ((DN & 0xffff)<<8)) {
                prepare_result(output, j);
            }
        }
    } else if (LZ == 11) {
        for (int j=0;j<npar;j++) {
            if (partial_hashes[result_size*j] == 0 && partial_hashes[result_size*j+1] < ((DN & 0xffff)<<4)) {
                prepare_result(output, j);
            }
        }
    } else if (LZ == 12) {
        for (int j=0;j<npar;j++) {
            if (partial_hashes[result_size*j] == 0 && partial_hashes[result_size*j+1] < ((DN & 0xffff))) {
                prepare_result(output, j);
            }
        }
    } else if (LZ == 13) {
        for (int j=0;j<npar;j++) {
            if (partial_hashes[result_size*j] == 0 && partial_hashes[result_size*j+1] < ((DN & 0xfff0)>>4) && partial_hashes[result_size*j+2] < ((DN & 0x000f)<<28)) {
                prepare_result(output, j);
            }
        }
    } else if (LZ == 14) {
        for (int j=0;j<npar;j++) {
            if (partial_hashes[result_size*j] == 0 && partial_hashes[result_size*j+1] < ((DN & 0xff00)>>8) && partial_hashes[result_size*j+2] < ((DN & 0x00ff)<<24)) {
                prepare_result(output, j);
            }
        }
    } else if (LZ == 15) {
        for (int j=0;j<npar;j++) {
            if (partial_hashes[result_size*j] == 0 && partial_hashes[result_size*j+1] < ((DN & 0xf000)>>12) && partial_hashes[result_size*j+2] < ((DN & 0x0fff)<<20)) {
                prepare_result(output, j);
            }
        }
    } else if (LZ == 16) {
        for (int j=0;j<npar;j++) {
            if (partial_hashes[result_size*j] == 0 && partial_hashes[result_size*j+1] == 0 && partial_hashes[result_size*j+2] < ((DN & 0xffff)<<16)) {
                prepare_result(output, j);
            }
        }
    } else if (LZ == 17) {
        for (int j=0;j<npar;j++) {
            if (partial_hashes[result_size*j] == 0 && partial_hashes[result_size*j+1] == 0 && partial_hashes[result_size*j+2] < ((DN & 0xffff)<<12)) {
                prepare_result(output, j);
            }
        }
    } else if (LZ == 18) {
        for (int j=0;j<npar;j++) {
            if (partial_hashes[result_size*j] == 0 && partial_hashes[result_size*j+1] == 0 && partial_hashes[result_size*j+2] < ((DN & 0xffff)<<8)) {
                prepare_result(output, j);
            }
        }
    }
    // ... TODO

    if (j_offset > 0) {
        prepare_result(output, 0);
        printf("%s\n", output);
        prepare_result(output, 1);
        printf("%s\n", output);
        prepare_result(output, npar-2);
        printf("%s\n", output);
        prepare_result(output, npar-1);
        printf("%s\n", output);
    }

    return;
}

void crypt_all() {
	ret = clEnqueueWriteBuffer(command_queue, data_info, CL_TRUE, 0, sizeof(unsigned int) * 3, datai, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(command_queue, buffer_keys, CL_TRUE, 0, input_stride, saved_plain, 0, NULL, NULL);
    size_t globalws[1] = {npar};
    size_t localws[1] = {256}; // <---- you might want to tweak this here
	ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, globalws, localws, 0, NULL, NULL);
	ret = clFinish(command_queue);
	ret = clEnqueueReadBuffer(command_queue, buffer_out, CL_TRUE, 0, sizeof(cl_uint) * result_size * npar, partial_hashes, 0, NULL, NULL);
	have_full_hashes = 0;
}

void load_source() {
	FILE *fp;
	fp = fopen("sha256_opencl.cl", "r");
	if (!fp) {
		fprintf(stderr, "Could not load kernel, please place the sha256_opencl.cl in the same directory.\n");
		exit(1);
	}
	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);
}

void create_clobj(){
	pinned_saved_keys = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, input_stride, NULL, &ret);
	saved_plain = (char*)clEnqueueMapBuffer(command_queue, pinned_saved_keys, CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, 0, input_stride, 0, NULL, NULL, &ret);

	memset(saved_plain, 0, input_stride);
	res_hashes = (cl_uint *)malloc(sizeof(cl_uint) * result_size * npar);
	memset(res_hashes, 0, sizeof(cl_uint) * result_size * npar);

	pinned_partial_hashes = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, sizeof(cl_uint) * result_size * npar, NULL, &ret);
	partial_hashes = (cl_uint *) clEnqueueMapBuffer(command_queue, pinned_partial_hashes, CL_TRUE, CL_MAP_READ, 0, sizeof(cl_uint) * result_size * npar, 0, NULL, NULL, &ret);
	memset(partial_hashes, 0, sizeof(cl_uint) * result_size * npar);

	buffer_keys = clCreateBuffer(context, CL_MEM_READ_ONLY, input_stride, NULL, &ret);
	buffer_out = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_uint) * result_size * npar, NULL, &ret);
	data_info = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(unsigned int) * 3, NULL, &ret);

	clSetKernelArg(kernel, 0, sizeof(data_info), (void *) &data_info);
	clSetKernelArg(kernel, 1, sizeof(buffer_keys), (void *) &buffer_keys);
	clSetKernelArg(kernel, 2, sizeof(buffer_out), (void *) &buffer_out);
}

void listDevices() {
    ret = clGetPlatformIDs(0, NULL, &ret_num_platforms);
    if (ret_num_platforms < 1) {
        printf("error: no platforms available, make sure graphic drivers and OpenCL are installed.\n");
        exit(1);
    }
    cl_platform_id *platforms = new cl_platform_id[ret_num_platforms]; 
	ret = clGetPlatformIDs(ret_num_platforms, platforms, &ret_num_platforms);
    if (ret != CL_SUCCESS) {
        printf("error: could not obtain platforms.\n");
        exit(1);
    }
    for( int i=0; i<(int)ret_num_platforms; i++ ) {
        ret = clGetDeviceIDs( platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &ret_num_devices);
        if (ret_num_devices < 0) {
            printf("info: platform has no devices.\n");
        } else {
            cl_device_id *devices = new cl_device_id[ ret_num_platforms ];
            ret = clGetDeviceIDs( platforms[i], CL_DEVICE_TYPE_ALL, ret_num_devices, devices, &ret_num_devices);

            cl_device_type type;
            cl_uint ui;
            cl_uint freq;
            for (int j=0; j< (int)ret_num_devices; j++) {
                clGetDeviceInfo( devices[j], CL_DEVICE_TYPE, sizeof(type), &type, NULL );
                clGetDeviceInfo( devices[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(ui), &ui, NULL );
                clGetDeviceInfo( devices[j], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(freq), &freq, NULL);
                if (type == CL_DEVICE_TYPE_CPU) {
                    printf("device CL_DEVICE_TYPE_CPU: platform %d device %d, has %d compute units, %d MHz\n", i, j, ui, freq); 
                } else if (type == CL_DEVICE_TYPE_GPU) {
                    printf("device CL_DEVICE_TYPE_GPU: platform %d device %d, has %d compute units, %d MHz\n", i, j, ui, freq); 
                } else if (type == CL_DEVICE_TYPE_ACCELERATOR) {
                    printf("device CL_DEVICE_TYPE_ACCELERATOR: platform %d device %d, has %d compute units, %d MHz\n", i, j, ui, freq); 
                } else {
                    printf("unknown device: platform %d device %d, has %d compute units, %d MHz\n", i, j, ui, freq);
                }
            }
        }
    }
}

void createDevice(int platform_idx, int device_idx) {
    ret = clGetPlatformIDs(0, NULL, &ret_num_platforms);
    if (ret_num_platforms < 1) {
        printf("error: no platforms available, make sure graphic drivers and OpenCL are installed.\n");
        exit(1);
    }
    cl_platform_id *platforms = new cl_platform_id[ret_num_platforms];
    ret = clGetPlatformIDs(ret_num_platforms, platforms, &ret_num_platforms);
    if (ret != CL_SUCCESS) {
        printf("error: could not obtain platforms.\n");
        exit(1);
    }
    if (platform_idx > ret_num_platforms -1) {
        printf("error: platform invalid: %d\n", platform_idx);
        exit(3);
    }
    for( int i=0; i<(int)ret_num_platforms; i++ ) {
        ret = clGetDeviceIDs( platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &ret_num_devices);
        if (ret_num_devices < 0) {
            printf("info: platform has no devices.\n");
        } else {
            devices = new cl_device_id[ ret_num_platforms ];
            ret = clGetDeviceIDs( platforms[i], CL_DEVICE_TYPE_ALL, ret_num_devices, devices, &ret_num_devices);
            if (device_idx > ret_num_devices-1) {
                printf("error: device invalid: %d\n", device_idx);
                exit(4);
            }
            for (int j=0; j< (int)ret_num_devices; j++) {
                if (i==platform_idx && j==device_idx) {
                    printf("using device: platform %d device %d.\n", i, j);
                    device_id = devices[j];
                    context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);
                    printf("context created. ret=%d\n", ret);
                    if (ret != CL_SUCCESS) {
                        printf("error: could not create context for platform %d device %d.\n", i, j);
                        exit(2);
                    }
                    return;
                }
            }
        }
    }
}

void createkernel() {
    printf("Loading kernel code..\n");
	program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
    const char options[] = "-cl-mad-enable";
    printf("Building kernel code..\n");
	ret = clBuildProgram(program, 1, &device_id, options, NULL, NULL);

    if (ret != CL_SUCCESS) {
        printf("ERROR: build failed, make sure OpenCL is installed and the code is correct.\n");
        size_t len = 0;
        cl_int ret = CL_SUCCESS;
        ret = clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &len);
        char *buffer = (char*)calloc(len, sizeof(char));
        ret = clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, len, buffer, NULL);

        // CL compiler log
        printf("BUILD LOG:%s\n", buffer);
    }

    // TODO: if compilation fails, abort.

    printf("Creating kernel...\n");
	kernel = clCreateKernel(program, "sha256_crypt_kernel", &ret);

    printf("Creating queue...\n");
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
}
