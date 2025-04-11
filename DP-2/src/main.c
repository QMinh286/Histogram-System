#include "../inc/dp2.h"

int main(int argc, char *argv[]) {
    int result;
    int shm_id;
    
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <shm_id>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    shm_id = atoi(argv[1]);
    
    result = dp2_init(shm_id);
    if (result != 0) {
        fprintf(stderr, "Failed to initialize DP-2\n");
        return EXIT_FAILURE;
    }
    
    result = dp2_process();
    dp2_cleanup();
    
    return (result == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}