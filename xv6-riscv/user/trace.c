#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(2, "Usage: trace command [args]\n");
        exit(1);
    }

    trace(1);
    exec(argv[1], &argv[1]);

    fprintf(2, "trace: exec failed\n");
    exit(1);
}