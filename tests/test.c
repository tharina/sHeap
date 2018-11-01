#include "sheap.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>

int main(int argc, char* argv[]) {
    time_t t;
    srand((unsigned) time(&t));

    size_t max_size = 16 * 1024 * 1024;

    for(int j = 0; j < 100; ++j) {
        const void* ptr = shalloc((rand() % (max_size)) + 1);;
        for (int i = 0; i < 100; ++i) {
            char* ptr_w = sh_unlock(ptr);
            ptr_w[0] = 'K';
            sh_lock(ptr_w);
            if (rand() % 2) {
                const void* p1 = shalloc((rand() % (max_size)) + 1);
                shalloc((rand() % (max_size)) + 1);
                const void* p3 = shalloc((rand() % (max_size)) + 1);
                const void* p4 = shalloc((rand() % (max_size)) + 1);
                sh_validate(p3);
                shfree(p3);
                sh_validate(p1);
                shfree(p1);
                sh_validate(p4);
                shfree(p4);

            } else {
                sh_validate(ptr);
                shfree(ptr);
            }
            ptr = shalloc((rand() % (1024)) + 1);
        }
    }

    shalloc(1024);

    printf("\nSuccess :)\n");
    return 0;
}