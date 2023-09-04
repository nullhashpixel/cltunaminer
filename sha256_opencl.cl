#ifndef uint32_t
#define uint32_t unsigned int
#endif

#define F1(x,y,z)   (bitselect(z,y,x))
#define F0(x,y,z)   (bitselect (x, y, ((x) ^ (z))))
#define mod(x,y) ((x)-((x)/(y)*(y)))
#define shr32(x,n) ((x) >> (n))
#define rotl32(a,n) rotate ((a), (n))

#define S0(x) (rotl32 ((x), 25u) ^ rotl32 ((x), 14u) ^ shr32 ((x),  3u))
#define S1(x) (rotl32 ((x), 15u) ^ rotl32 ((x), 13u) ^ shr32 ((x), 10u))
#define S2(x) (rotl32 ((x), 30u) ^ rotl32 ((x), 19u) ^ rotl32 ((x), 10u))
#define S3(x) (rotl32 ((x), 26u) ^ rotl32 ((x), 21u) ^ rotl32 ((x),  7u))

#define SHA256C00 0x428a2f98u
#define SHA256C01 0x71374491u
#define SHA256C02 0xb5c0fbcfu
#define SHA256C03 0xe9b5dba5u
#define SHA256C04 0x3956c25bu
#define SHA256C05 0x59f111f1u
#define SHA256C06 0x923f82a4u
#define SHA256C07 0xab1c5ed5u
#define SHA256C08 0xd807aa98u
#define SHA256C09 0x12835b01u
#define SHA256C0a 0x243185beu
#define SHA256C0b 0x550c7dc3u
#define SHA256C0c 0x72be5d74u
#define SHA256C0d 0x80deb1feu
#define SHA256C0e 0x9bdc06a7u
#define SHA256C0f 0xc19bf174u
#define SHA256C10 0xe49b69c1u
#define SHA256C11 0xefbe4786u
#define SHA256C12 0x0fc19dc6u
#define SHA256C13 0x240ca1ccu
#define SHA256C14 0x2de92c6fu
#define SHA256C15 0x4a7484aau
#define SHA256C16 0x5cb0a9dcu
#define SHA256C17 0x76f988dau
#define SHA256C18 0x983e5152u
#define SHA256C19 0xa831c66du
#define SHA256C1a 0xb00327c8u
#define SHA256C1b 0xbf597fc7u
#define SHA256C1c 0xc6e00bf3u
#define SHA256C1d 0xd5a79147u
#define SHA256C1e 0x06ca6351u
#define SHA256C1f 0x14292967u
#define SHA256C20 0x27b70a85u
#define SHA256C21 0x2e1b2138u
#define SHA256C22 0x4d2c6dfcu
#define SHA256C23 0x53380d13u
#define SHA256C24 0x650a7354u
#define SHA256C25 0x766a0abbu
#define SHA256C26 0x81c2c92eu
#define SHA256C27 0x92722c85u
#define SHA256C28 0xa2bfe8a1u
#define SHA256C29 0xa81a664bu
#define SHA256C2a 0xc24b8b70u
#define SHA256C2b 0xc76c51a3u
#define SHA256C2c 0xd192e819u
#define SHA256C2d 0xd6990624u
#define SHA256C2e 0xf40e3585u
#define SHA256C2f 0x106aa070u
#define SHA256C30 0x19a4c116u
#define SHA256C31 0x1e376c08u
#define SHA256C32 0x2748774cu
#define SHA256C33 0x34b0bcb5u
#define SHA256C34 0x391c0cb3u
#define SHA256C35 0x4ed8aa4au
#define SHA256C36 0x5b9cca4fu
#define SHA256C37 0x682e6ff3u
#define SHA256C38 0x748f82eeu
#define SHA256C39 0x78a5636fu
#define SHA256C3a 0x84c87814u
#define SHA256C3b 0x8cc70208u
#define SHA256C3c 0x90befffau
#define SHA256C3d 0xa4506cebu
#define SHA256C3e 0xbef9a3f7u
#define SHA256C3f 0xc67178f2u 

__constant uint k_sha256[64] =
{
  SHA256C00, SHA256C01, SHA256C02, SHA256C03,
  SHA256C04, SHA256C05, SHA256C06, SHA256C07,
  SHA256C08, SHA256C09, SHA256C0a, SHA256C0b,
  SHA256C0c, SHA256C0d, SHA256C0e, SHA256C0f,
  SHA256C10, SHA256C11, SHA256C12, SHA256C13,
  SHA256C14, SHA256C15, SHA256C16, SHA256C17,
  SHA256C18, SHA256C19, SHA256C1a, SHA256C1b,
  SHA256C1c, SHA256C1d, SHA256C1e, SHA256C1f,
  SHA256C20, SHA256C21, SHA256C22, SHA256C23,
  SHA256C24, SHA256C25, SHA256C26, SHA256C27,
  SHA256C28, SHA256C29, SHA256C2a, SHA256C2b,
  SHA256C2c, SHA256C2d, SHA256C2e, SHA256C2f,
  SHA256C30, SHA256C31, SHA256C32, SHA256C33,
  SHA256C34, SHA256C35, SHA256C36, SHA256C37,
  SHA256C38, SHA256C39, SHA256C3a, SHA256C3b,
  SHA256C3c, SHA256C3d, SHA256C3e, SHA256C3f,
};

#define SHA256_STEP(F0a,F1a,a,b,c,d,e,f,g,h,x,K)  \
{                                               \
  h += K;                                       \
  h += x;                                       \
  h += S3 (e);                           \
  h += F1a (e,f,g);                              \
  d += h;                                       \
  h += S2 (a);                           \
  h += F0a (a,b,c);                              \
}

#define SHA256_EXPAND(x,y,z,w) (S1 (x) + y + S0 (z) + w) 

static void sha256_process2 (const unsigned int *W, unsigned int *digest)
{
  unsigned int a = digest[0];
  unsigned int b = digest[1];
  unsigned int c = digest[2];
  unsigned int d = digest[3];
  unsigned int e = digest[4];
  unsigned int f = digest[5];
  unsigned int g = digest[6];
  unsigned int h = digest[7];

  unsigned int w0_t = W[0];
  unsigned int w1_t = W[1];
  unsigned int w2_t = W[2];
  unsigned int w3_t = W[3];
  unsigned int w4_t = W[4];
  unsigned int w5_t = W[5];
  unsigned int w6_t = W[6];
  unsigned int w7_t = W[7];
  unsigned int w8_t = W[8];
  unsigned int w9_t = W[9];
  unsigned int wa_t = W[10];
  unsigned int wb_t = W[11];
  unsigned int wc_t = W[12];
  unsigned int wd_t = W[13];
  unsigned int we_t = W[14];
  unsigned int wf_t = W[15];

  #define ROUND_EXPAND(i)                           \
  {                                                 \
    w0_t = SHA256_EXPAND (we_t, w9_t, w1_t, w0_t);  \
    w1_t = SHA256_EXPAND (wf_t, wa_t, w2_t, w1_t);  \
    w2_t = SHA256_EXPAND (w0_t, wb_t, w3_t, w2_t);  \
    w3_t = SHA256_EXPAND (w1_t, wc_t, w4_t, w3_t);  \
    w4_t = SHA256_EXPAND (w2_t, wd_t, w5_t, w4_t);  \
    w5_t = SHA256_EXPAND (w3_t, we_t, w6_t, w5_t);  \
    w6_t = SHA256_EXPAND (w4_t, wf_t, w7_t, w6_t);  \
    w7_t = SHA256_EXPAND (w5_t, w0_t, w8_t, w7_t);  \
    w8_t = SHA256_EXPAND (w6_t, w1_t, w9_t, w8_t);  \
    w9_t = SHA256_EXPAND (w7_t, w2_t, wa_t, w9_t);  \
    wa_t = SHA256_EXPAND (w8_t, w3_t, wb_t, wa_t);  \
    wb_t = SHA256_EXPAND (w9_t, w4_t, wc_t, wb_t);  \
    wc_t = SHA256_EXPAND (wa_t, w5_t, wd_t, wc_t);  \
    wd_t = SHA256_EXPAND (wb_t, w6_t, we_t, wd_t);  \
    we_t = SHA256_EXPAND (wc_t, w7_t, wf_t, we_t);  \
    wf_t = SHA256_EXPAND (wd_t, w8_t, w0_t, wf_t);  \
  }

  #define ROUND_STEP(i)                                                                   \
  {                                                                                       \
    SHA256_STEP (F0, F1, a, b, c, d, e, f, g, h, w0_t, k_sha256[i +  0]); \
    SHA256_STEP (F0, F1, h, a, b, c, d, e, f, g, w1_t, k_sha256[i +  1]); \
    SHA256_STEP (F0, F1, g, h, a, b, c, d, e, f, w2_t, k_sha256[i +  2]); \
    SHA256_STEP (F0, F1, f, g, h, a, b, c, d, e, w3_t, k_sha256[i +  3]); \
    SHA256_STEP (F0, F1, e, f, g, h, a, b, c, d, w4_t, k_sha256[i +  4]); \
    SHA256_STEP (F0, F1, d, e, f, g, h, a, b, c, w5_t, k_sha256[i +  5]); \
    SHA256_STEP (F0, F1, c, d, e, f, g, h, a, b, w6_t, k_sha256[i +  6]); \
    SHA256_STEP (F0, F1, b, c, d, e, f, g, h, a, w7_t, k_sha256[i +  7]); \
    SHA256_STEP (F0, F1, a, b, c, d, e, f, g, h, w8_t, k_sha256[i +  8]); \
    SHA256_STEP (F0, F1, h, a, b, c, d, e, f, g, w9_t, k_sha256[i +  9]); \
    SHA256_STEP (F0, F1, g, h, a, b, c, d, e, f, wa_t, k_sha256[i + 10]); \
    SHA256_STEP (F0, F1, f, g, h, a, b, c, d, e, wb_t, k_sha256[i + 11]); \
    SHA256_STEP (F0, F1, e, f, g, h, a, b, c, d, wc_t, k_sha256[i + 12]); \
    SHA256_STEP (F0, F1, d, e, f, g, h, a, b, c, wd_t, k_sha256[i + 13]); \
    SHA256_STEP (F0, F1, c, d, e, f, g, h, a, b, we_t, k_sha256[i + 14]); \
    SHA256_STEP (F0, F1, b, c, d, e, f, g, h, a, wf_t, k_sha256[i + 15]); \
  }

  ROUND_STEP (0);

  ROUND_EXPAND();
  ROUND_STEP(16);

  ROUND_EXPAND();
  ROUND_STEP(32);

  ROUND_EXPAND();
  ROUND_STEP(48);

  digest[0] += a;
  digest[1] += b;
  digest[2] += c;
  digest[3] += d;
  digest[4] += e;
  digest[5] += f;
  digest[6] += g;
  digest[7] += h;
}

#define H0 0x6a09e667
#define H1 0xbb67ae85
#define H2 0x3c6ef372
#define H3 0xa54ff53a
#define H4 0x510e527f
#define H5 0x9b05688c
#define H6 0x1f83d9ab
#define H7 0x5be0cd19

// each outupt vector is 16 bytes
#define result_size 16


__kernel void sha256_crypt_kernel(__global uint *data_info,__global char *plain_key0,  __global uint *digest2){
  int t, gid, msg_pad;
  int stop, mmod;
  uint i, ulen, item, total;
  uint W[16], temp;
  int current_pad;

  const int ix = get_global_id(0);
  char plain_key[68];

  uint32_t mdigest[result_size];
  mdigest[0] = 0xffffffff;
  mdigest[8] = 0;
  mdigest[9] = 0;


  for(int loop_i=0;loop_i<64;loop_i++) {
  #pragma unroll
  for (t = 0; t < 13; t++){
     plain_key[t] = plain_key0[t];
  }
  plain_key[13] = loop_i & 0xff;
  plain_key[14] = (ix & 0xff0000000000) >> 40;
  plain_key[15] = (ix & 0xff00000000) >> 32;
  plain_key[16] = (ix & 0xff000000) >> 24;
  plain_key[17] = (ix & 0xff0000) >> 16;
  plain_key[18] = (ix & 0xff00) >> 8;
  plain_key[19] = (ix & 0xff);
  #pragma unroll
  for (t = 20; t < 68; t++){
     plain_key[t] = plain_key0[t];
  }

      uint digest[8];
      ulen = 67; 
      total = 2;
      msg_pad=0;

      digest[0] = H0;
      digest[1] = H1;
      digest[2] = H2;
      digest[3] = H3;
      digest[4] = H4;
      digest[5] = H5;
      digest[6] = H6;
      digest[7] = H7;

      for(item=0; item<total; item++)
      {

    #pragma unroll
        for (t = 0; t < 16; t++){
        W[t] = 0x00000000;
        }
        msg_pad=item*64;
        if(ulen > msg_pad)
        {
          current_pad = (ulen-msg_pad)>64?64:(ulen-msg_pad);
        }
        else
        {
          current_pad =-1;    
        }

        if(current_pad>0)
        {
          i=current_pad;

          stop =  i/4;
          for (t = 0 ; t < stop ; t++){
            W[t] = ((uchar)  plain_key[msg_pad + t * 4]) << 24;
            W[t] |= ((uchar) plain_key[msg_pad + t * 4 + 1]) << 16;
            W[t] |= ((uchar) plain_key[msg_pad + t * 4 + 2]) << 8;
            W[t] |= (uchar)  plain_key[msg_pad + t * 4 + 3];
            //printf("W[%u]: %u\n",t,W[t]);
          }
          mmod = i % 4;
          if ( mmod == 3){
            W[t] = ((uchar)  plain_key[msg_pad + t * 4]) << 24;
            W[t] |= ((uchar) plain_key[msg_pad + t * 4 + 1]) << 16;
            W[t] |= ((uchar) plain_key[msg_pad + t * 4 + 2]) << 8;
            W[t] |=  ((uchar) 0x80) ;
          } else if (mmod == 2) {
            W[t] = ((uchar)  plain_key[msg_pad + t * 4]) << 24;
            W[t] |= ((uchar) plain_key[msg_pad + t * 4 + 1]) << 16;
            W[t] |=  0x8000 ;
          } else if (mmod == 1) {
            W[t] = ((uchar)  plain_key[msg_pad + t * 4]) << 24;
            W[t] |=  0x800000 ;
          } else /*if (mmod == 0)*/ {
            W[t] =  0x80000000 ;
          }
          
          if (current_pad<56)
          {
            W[15] =  ulen*8 ;
          }
        }
        else if(current_pad <0)
        {
          if( ulen%64==0)
            W[0]=0x80000000;
            W[15]=ulen*8;
        }

        sha256_process2(W, digest);
      }


      //2nd pass

      W[0] = digest[0];
      W[1] = digest[1];
      W[2] = digest[2];
      W[3] = digest[3];
      W[4] = digest[4];
      W[5] = digest[5];
      W[6] = digest[6];
      W[7] = digest[7];
      W[8] = 0x80000000;
      W[9] = 0;
      W[10] = 0;
      W[11] = 0;
      W[12] = 0;
      W[13] = 0;
      W[14] = 0; 
      W[15] = 0x00000100; 

      digest[0] = H0;
      digest[1] = H1;
      digest[2] = H2;
      digest[3] = H3;
      digest[4] = H4;
      digest[5] = H5;
      digest[6] = H6;
      digest[7] = H7;

      sha256_process2(W, digest);

      if (digest[0] < mdigest[0] || (digest[0]==mdigest[0] && digest[1] < mdigest[1]) || (digest[0]==mdigest[0] && digest[1]==mdigest[1] && digest[2] < mdigest[2])) {
        for(t=0;t<8;t++) {
            mdigest[t] = digest[t];
        }
        mdigest[8] = loop_i;
      }
    }
    digest2[ix*result_size+0] = mdigest[0];
    digest2[ix*result_size+1] = mdigest[1]; 
    digest2[ix*result_size+2] = mdigest[2];
    digest2[ix*result_size+3] = mdigest[3];
    digest2[ix*result_size+4] = mdigest[4];
    digest2[ix*result_size+5] = mdigest[5];
    digest2[ix*result_size+6] = mdigest[6];
    digest2[ix*result_size+7] = mdigest[7];
    digest2[ix*result_size+8] = mdigest[8];

}
