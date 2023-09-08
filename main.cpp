#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "OpenCL.lib")
#pragma comment(lib, "Ws2_32.lib")
#endif

#include "sha256.h"
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <chrono>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdint.h>
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)  


uint64_t microseconds() {
    LARGE_INTEGER fq, t;
    QueryPerformanceFrequency(&fq);
    QueryPerformanceCounter(&t);
    return (1000000 * t.QuadPart) / fq.QuadPart;
}

#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#endif


const int nloops = 256; // the same number needs to be set in the kernel!

// https://stackoverflow.com/a/23898449/266720
uint8_t tallymarker_hextobin(const char * str, char * bytes, size_t blen) {
   uint8_t  pos;
   uint8_t  idx0;
   uint8_t  idx1;

   // mapping of ASCII characters to hex values
   const uint8_t hashmap[] = {
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  !"#$%&'
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ()*+,-./
     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, // 01234567
     0x08, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 89:;<=>?
     0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x00, // @ABCDEFG
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // HIJKLMNO
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // PQRSTUVW
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // XYZ[\]^_
     0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x00, // `abcdefg
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // hijklmno
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // pqrstuvw
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // xyz{|}~.
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // ........
   };

   bzero(bytes, blen);
   for (pos = 0; ((pos < (blen*2)) && (pos < strlen(str))); pos += 2) {
      idx0 = (uint8_t)str[pos+0];
      idx1 = (uint8_t)str[pos+1];
      bytes[pos/2] = (uint8_t)(hashmap[idx0] << 4) | hashmap[idx1];
   };

   return(0);
}


int main(int argc, char *argv[]) {
    printf("CLTUNA OpenCL core v1.4\n");
    printf("usage:    ./cltuna [hostname] [port] [platform] [device]\n");
    printf("examples: ./cltuna                 (default)\n");
    printf("          ./cltuna 0.0.0.0 12345   (run exposed to network on port 12345)\n");
    printf("          ./cltuna benchmark       (run benchmark)\n");
    printf("          ./cltuna list            (show list of platform and device IDs)\n");

    char hostname[256] = "127.0.0.1"; // only accessible on the same machine, use 0.0.0.0 to make it available to the network
    uint16_t port = 2023;
    int platform_idx = 0;
    int device_idx = 0;

    if (argc > 1) {
        strncpy(hostname, argv[1], 255);
        hostname[255] = 0;
    }
    if (argc > 2) {
        port = atoi(argv[2]);
    }
    if (argc > 3) {
        platform_idx = atoi(argv[3]);
    } 
    if (argc > 4) {
        device_idx = atoi(argv[4]);
    } 

    // result string which will be returned
    char result[256];

    // input plaintext
    char bytes[68];

    // list all devices
    if (argc > 1 && strncmp(hostname, "list", 4) == 0) {
        listDevices();
        exit(0);
    }

    sha256_init(platform_idx, device_idx);
    srand(time(NULL));

    // run with any argument to start benchmark mode
    if (argc > 1 && strncmp(hostname, "benchmark", 9) == 0) {
        auto begin = std::chrono::high_resolution_clock::now();
        uint64_t n_hashes = 0;
        while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - begin).count() < 1100) {
            memset(result, 0, 256);
            memset(bytes,0,67);
            sha256_crypt(bytes, 67, 2, 6, 1, result);
            n_hashes += npar*nloops;
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
        double hash_rate = (double)(n_hashes)/(elapsed.count() * 0.001);
        printf("hash rate: %f/s\n", hash_rate);
        exit(0);
    } else if (argc > 1 && strncmp(hostname, "find", 4) == 0) {
        if (argc > 6) {
            int LZ = atoi(argv[5]);
            int DN = atoi(argv[6]);
            int num = 1;
            if (argc > 7) {
                num = atoi(argv[7]);
            }
            int num_total = num;
            printf("find %d hashes with LZ=%d DN=%d...\n", num_total, LZ, DN);
            auto begin = std::chrono::high_resolution_clock::now();
            while (num > 0) {
                memset(result, 0, 256);
                memset(bytes,0,67);
                sha256_crypt(bytes, 67, 0, LZ, DN, result);
                if (result[0] != 0) {
                    printf("\x1b[32mfound: %s\x1b[0m\n", result);
                    num -= 1;
                }
            }
            auto end = std::chrono::high_resolution_clock::now();
            printf("found %d hashes with LZ=%d DN=%d in %lu seconds.\n", num_total, LZ, DN, std::chrono::duration_cast<std::chrono::seconds>(end - begin).count());
            exit(0);
        } else {
            printf("find needs at least 6 arguments.\n");
            exit(7);
        }

    }

    printf("starting...\n");

#ifdef _WIN32
    int iResult;
    WSADATA wsaData;
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }
#endif

    // Socket communication
    printf("Creating socket...\n");
    int socket_desc, client_sock;
    socklen_t client_size;
    struct sockaddr_in server_addr, client_addr;
    char server_message[2000], client_message[2000], tmp_message[2000];
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_desc < 0){
        printf("Error while creating socket\n");
        return -1;
    }
    printf("Socket created successfully\n");

    // Set port and IP:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(hostname);

    // Bind to the set port and IP:
    if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
        printf("Couldn't bind to the port\n");
        return -1;
    }
    printf("Done with binding\n");

    // Run a single test of SHA256
    memset(result,0,256);
    memset(bytes,0,68);
    sha256_crypt(bytes, 67, 2, 8, 65532, result);
    printf("TEST: %s\n", result);
    printf("--------\n");

    // Wait for work
    while(true) {    
        // Listen for clients
        if(listen(socket_desc, 1) < 0){
            printf("Error while listening\n");
            return -1;
        }
        printf("\nListening for incoming connections on %s:%d.....\n", hostname, port);
        
        // Accept an incoming connection
        client_size = sizeof(client_addr);
        client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);
        
        if (client_sock < 0){
            printf("Can't accept\n");
            break;
        }
        printf("Client connected at IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        while(true) { 

            auto time_0 = std::chrono::high_resolution_clock::now();
            // Receive client's message
            memset(client_message, '\0', sizeof(client_message));
            memset(tmp_message, '\0', sizeof(tmp_message));

            while( strlen(tmp_message) < 134) {
                memset(client_message, '\0', sizeof(client_message));
                memset(tmp_message, '\0', sizeof(tmp_message));
                if (recv(client_sock, tmp_message, sizeof(tmp_message), 0) < 0){
                    printf("Couldn't receive\n");
                    break;
                }
                if (strlen(tmp_message) < 134) {
                    printf("\x1b[31merror: invalid message.\x1b[0m\n");
                    exit(8);
                }
            }
            
            // Delimited by \n, only take last message and discard all previous ones
            char *token;
            const char s[2] = "\n";
            token = strtok(tmp_message, s);
            while( token != NULL && strlen(token) > 2) {
               strncpy(client_message, token, sizeof(client_message));
               client_message[strlen(token)] = 0;
               token = strtok(NULL, s);
            }
            printf("Msg from client [%lu]: %s \n", strlen(client_message), client_message);


            char * txbytes_hex = strtok(client_message, " "); // DATUM
            char * token2 = strtok(NULL, " ");                // LZ
            char * token3 = strtok(NULL, " ");                // DN

            int LZ = atoi(token2);
            int DN = atoi(token3);

            printf("TX: %s, LZ: %d, DN: %d\n", txbytes_hex, LZ, DN);

            // HEX to bytes
            tallymarker_hextobin(txbytes_hex, bytes, 67);
            bytes[67]=0;

#ifdef _WIN32
            srand( microseconds() + rand());
#else
            struct timeval tv;
            gettimeofday(&tv,NULL);
            unsigned long time_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;
            srand(time_in_micros + rand());
#endif
            auto time_1 = std::chrono::high_resolution_clock::now();
            uint64_t n_hashes = 0;
            bool found = false;
            while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - time_1).count() < 3100) {
                memset(result, 0, 256);

                // Do the magic and get some fish!
                sha256_crypt(bytes, 67, 0, LZ, DN, result);

                n_hashes += npar*nloops;

                // If hash has been found
                if (result[0] != 0) {
                    printf("\n\n\x1b[32mfound: %s\x1b[0m\n\n\n", result);
                    strcpy(server_message, result);

                    // Send message back to client
                    if (send(client_sock, server_message, strlen(server_message), 0) < 0){
                        printf("Can't send\n");
                        break;
                    }
                    found = true;
                    break;
                }
            }
            auto end = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - time_0);
            double hash_rate = (double)(n_hashes)/(elapsed.count() * 0.001);

            uint64_t seconds = (pow(2,4*LZ)*65536./(1+DN)) / hash_rate;
            uint64_t weeks   = seconds / (86400*7);
            seconds -= weeks * 86400*7;
            uint64_t days    = seconds / 86400;
            seconds -= days * 86400;
            uint64_t hours   = seconds / 3600;
            seconds -= hours * 3600;
            uint64_t minutes = seconds / 60;
            seconds -= minutes*60;
            
            if (weeks > 4) {
                printf("hash rate: %f, expect block every %ld weeks %ld days. \n", hash_rate, weeks, days);
            } else if (weeks > 0 || days > 0) {
                printf("hash rate: %f, expect block every %ld days %ld hours. \n", hash_rate, weeks*7+days, hours);
            } else if (hours > 0) {
                printf("hash rate: %f, expect block every %ld hours %ld minutes. \n", hash_rate, hours, minutes);
            } else {
                printf("hash rate: %f, expect block every %ld minutes %ld seconds. \n", hash_rate, minutes, seconds);
            }

            // Respond to client with ping (single "." char to signal alive and ready)
            strcpy(server_message, ".");
            if (!found) {
                if (send(client_sock, server_message, strlen(server_message), 0) < 0){
                    printf("Can't send\n");
                    break;
                }
            }

            printf("done, waiting for client...\n");
        }
    }

}
