#include "../inc/dc.h"

int main(int argc, char *argv[]) {
    int result;
    int shm_id;
    pid_t dp1_pid, dp2_pid;
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <shm_id> <dp1_pid> <dp2_pid>\n", argv[0]);
        return EXIT_FAILURE;
    }

    shm_id = atoi(argv[1]);
    dp1_pid = atoi(argv[2]);
    dp2_pid = atoi(argv[3]);

    result = dc_init(shm_id, dp1_pid, dp2_pid);
    if (result != 0) {
        fprintf(stderr, "Failed to initialize DC\n");
        return EXIT_FAILURE;
    }
    result = dc_process();
    
    return (result == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}