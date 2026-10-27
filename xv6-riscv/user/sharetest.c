#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

static void
readshow(char *label, int fd, int n)
{
  char buf[16];
  int r = read(fd, buf, n);
  if(r < 0){
    printf("%s: read failed\n", label);
    exit(1);
  }
  buf[r] = 0;
  printf("%s read %d bytes: %s\n", label, r, buf);
}

int
main(void)
{
  char *data = "0123456789ABCDEFGHIJ";
  int fd = open("share.txt", O_CREATE | O_WRONLY);
  if(fd < 0){
    printf("sharetest: create failed\n");
    exit(1);
  }
  write(fd, data, 20);
  close(fd);

  printf("--- Test 1: two open() calls = two struct files, separate offsets\n");
  int a = open("share.txt", O_RDONLY);
  int b = open("share.txt", O_RDONLY);
  struct stat sa, sb;
  fstat(a, &sa);
  fstat(b, &sb);
  printf("fd %d -> inode %d, fd %d -> inode %d (same inode)\n",
         a, sa.ino, b, sb.ino);
  readshow("fd a", a, 5);
  readshow("fd b", b, 5);
  readshow("fd a", a, 5);
  close(b);

  printf("--- Test 2: dup() = same struct file, shared offset\n");
  int c = dup(a);
  readshow("fd c (dup of a)", c, 5);
  readshow("fd a", a, 5);
  close(a);
  close(c);

  printf("--- Test 3: fork() = child shares the parent's struct file\n");
  int d = open("share.txt", O_RDONLY);
  if(fork() == 0){
    readshow("child ", d, 5);
    exit(0);
  }
  wait(0);
  readshow("parent", d, 5);
  close(d);

  exit(0);
}