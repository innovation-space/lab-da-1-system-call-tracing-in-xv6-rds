#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc != 3){
    fprintf(2, "usage: filetest w|r filename\n");
    exit(1);
  }
  char *msg = "hello from xv6, this should persist\n";
  char buf[128];

  if(argv[1][0] == 'w'){
    int fd = open(argv[2], O_CREATE | O_WRONLY);
    if(fd < 0){ printf("filetest: open failed\n"); exit(1); }
    int n = write(fd, msg, strlen(msg));
    close(fd);
    printf("filetest: wrote %d bytes to %s\n", n, argv[2]);
  } else {
    int fd = open(argv[2], O_RDONLY);
    if(fd < 0){ printf("filetest: open failed\n"); exit(1); }
    int n = read(fd, buf, sizeof(buf) - 1);
    if(n < 0){ printf("filetest: read failed\n"); exit(1); }
    buf[n] = 0;
    close(fd);
    printf("filetest: read %d bytes: %s", n, buf);
  }
  exit(0);
}