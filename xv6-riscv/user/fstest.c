
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define BSIZE 1024
#define TEST_SIZE 12287
#define CHUNK_SIZE 777

char writebuf[CHUNK_SIZE];
char readbuf[CHUNK_SIZE];

int
main(void)
{
  int fd, i, j;
  int written = 0;
  int total_read = 0;
  int n;
  int ok = 1;

  printf("Starting file system block test\n");
  printf("Target size: %d bytes\n", TEST_SIZE);
  printf("Chunk size: %d bytes\n", CHUNK_SIZE);

  fd = open("fsdata", O_CREATE | O_RDWR);
  if(fd < 0){
    printf("ERROR: cannot create file\n");
    exit(1);
  }

  // Write a predictable pattern in chunks.
  while(written < TEST_SIZE){
    n = TEST_SIZE - written;
    if(n > CHUNK_SIZE)
      n = CHUNK_SIZE;

    for(i = 0; i < n; i++)
      writebuf[i] = (char)((written + i) % 251);

    if(write(fd, writebuf, n) != n){
      printf("ERROR: write failed\n");
      close(fd);
      exit(1);
    }

    written += n;
  }

  close(fd);
  printf("Bytes written: %d\n", written);

  // Reopen the file and verify every byte.
  fd = open("fsdata", O_RDONLY);
  if(fd < 0){
    printf("ERROR: cannot reopen file\n");
    exit(1);
  }

  while((n = read(fd, readbuf, CHUNK_SIZE)) > 0){
    for(j = 0; j < n; j++){
      char expected = (char)((total_read + j) % 251);
      if(readbuf[j] != expected){
        printf("ERROR: mismatch at byte %d\n", total_read + j);
        ok = 0;
        break;
      }
    }

    total_read += n;

    if(!ok)
      break;
  }

  close(fd);

  printf("Bytes read: %d\n", total_read);

  if(ok && total_read == TEST_SIZE)
    printf("PASS: every byte verified successfully\n");
  else
    printf("FAIL: file verification failed\n");

  exit(0);
}
