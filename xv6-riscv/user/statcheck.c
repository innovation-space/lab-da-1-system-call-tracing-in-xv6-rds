#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  struct stat st;
  if(argc != 2){
    fprintf(2, "usage: statcheck path\n");
    exit(1);
  }
  int fd = open(argv[1], O_RDONLY);
  if(fd < 0 || fstat(fd, &st) < 0){
    printf("statcheck: cannot stat %s\n", argv[1]);
    exit(1);
  }
  printf("%s: dev=%d ino=%d type=%d nlink=%d size=%d\n",
         argv[1], st.dev, st.ino, st.type, st.nlink, (int)st.size);
  close(fd);
  exit(0);
}