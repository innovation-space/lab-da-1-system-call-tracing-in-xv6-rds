#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
    int pid1, pid2;
    int expected;
    int actual;

    printf("=== Race Condition Test ===\n");

    // Reset shared counter
    race_test(0);

    // First process
    pid1 = fork();

    if(pid1 == 0){
        race_test(10000);
        exit(0);
    }

    // Second process
    pid2 = fork();

    if(pid2 == 0){
        race_test(10000);
        exit(0);
    }

    // Wait for both processes
    wait(0);
    wait(0);

    expected = 20000;
    actual = get_counter();

    printf("Expected counter: %d\n", expected);
    printf("Actual counter:   %d\n", actual);

    if(actual != expected)
        printf("RACE CONDITION DETECTED!\n");
    else
        printf("No race condition observed.\n");

    exit(0);
}