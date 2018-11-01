#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/mman.h>
#include "sheap.h"

typedef void (*benchmark_fun)(char*);

unsigned long measure(benchmark_fun f, void* arg)
{
    struct timespec before, after;

    clock_gettime(CLOCK_MONOTONIC, &before);
    f(arg);
    clock_gettime(CLOCK_MONOTONIC, &after);

    unsigned long duration = (after.tv_sec - before.tv_sec) * 1000000000;
    duration += after.tv_nsec - before.tv_nsec;
    return duration;
}

void empty_func()
{
}

#define MEM_SIZE            (1024UL*1024UL*1024UL*32UL)
#define STEP_SIZE           (1024UL*1024UL*32UL)
#define NUM_ITERATIONS      (MEM_SIZE / STEP_SIZE)


void mem_write_test(char* mem)
{
    for (size_t i = 0; i < MEM_SIZE; i += STEP_SIZE) {
        mem[i] = 1;
    }
}

void sheap_write_test(const void* mem) {
    for (size_t i = 0; i < NUM_ITERATIONS; i++) {
        char *mem_w = sh_unlock(mem);
        mem_w[0] = 1;
        sh_lock(mem_w);
    }
}

size_t dummy;
void mem_read_test(char* mem)
{
    size_t counter = 0;
    for (size_t i = 0; i < MEM_SIZE; i += STEP_SIZE) {
        counter += mem[i];
    }
    dummy = counter;
}

void sheap_read_test(char* mem)
{
    size_t counter = 0;
    for (size_t i = 0; i < MEM_SIZE; i += STEP_SIZE) {
        sh_validate(mem);
        counter += mem[i];
    }
    dummy = counter;
}

char* chunks[1024*1024];

void malloc_test()
{
    for (int i = 0; i < 1024*1024; i++) {
        chunks[i] = malloc(64);
        chunks[i][0] = 42;
    }
}



void free_test()
{
    for (int i = 0; i < 1024*1024; i++) {
        free(chunks[i]);
    }
}

void shalloc_test() {
    for (int i = 0; i < 1024; i++) {
        chunks[i] = shalloc(64);
    }
}

void shfree_test()
{
    for (int i = 0; i < 1024; i++) {
        shfree(chunks[i]);
    }
}

void shallocshfree_test() {
    for (int i = 0; i < 1024*1024; i++) {
        chunks[i] = shalloc(64);
        shfree(chunks[i]);
    }
}


void mmap_test()
{
    for (int i = 0; i < 1024; i++) {
        chunks[i] = mmap(0, 0x1000, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_POPULATE, -1, 0);
        //chunks[i][0] = 42;
    }

}

void munmap_test() {
    for (int i = 0; i < 1024; i++) {
        munmap(chunks[i], 0x1000);
    }
}



#define NUM_TESTS 16

int main()
{
    struct timespec t;
    clock_getres(CLOCK_MONOTONIC, &t);
    printf("Timer resolution: %lu ns\n", t.tv_nsec);

    unsigned long duration;
    unsigned long avg = 0;

    duration = measure(empty_func, NULL);
    printf("Doing nothing took %lu ns\n", duration);

    duration = measure(malloc_test, NULL);
    printf("malloc took %lu ns on average\n", duration / 1024 / 1024);

    duration = measure(free_test, NULL);
    printf("free took %lu ns on average\n", duration / 1024 / 1024);


    unsigned long avga = 0;
    unsigned long avgf = 0;
    for (int i = 0; i < NUM_TESTS; i++) {
        avga += measure(shalloc_test, NULL);

        avgf += measure(shfree_test, NULL);
    }
    printf("shalloc took %lu ns on average\n", avga / NUM_TESTS / 1024);
    printf("shfree took %lu ns on average\n", avgf / NUM_TESTS / 1024);

    unsigned long avgm = 0;
    unsigned long avgu = 0;
    for (int i = 0; i < NUM_TESTS; i++) {
        avgm += measure(mmap_test, NULL);
        avgu += measure(munmap_test, NULL);
    }
    printf("Mmap took %lu ns on average\n", avgm / NUM_TESTS / 1024);
    printf("Munmap took %lu ns on average\n", avgu / NUM_TESTS / 1024);


    char* chunk = mmap(0, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    memset(chunk, 0, MEM_SIZE);

    char* cacheflush = mmap(0, 1024*1024*1024, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    avg = 0;
    for (int i = 0; i < NUM_TESTS; i++) {
        memset(cacheflush, 0, 1024*1024*1024);
        avg += measure(mem_read_test, chunk);
    }
    printf("Reading memory took %lu ns on average\n", avg / NUM_TESTS / NUM_ITERATIONS);

    avg = 0;
    for (int i = 0; i < NUM_TESTS; i++) {
        memset(cacheflush, 0, 1024*1024*1024);
        avg += measure(mem_write_test, chunk);
    }
    printf("Writing memory took %lu ns on average\n", avg / NUM_TESTS/ NUM_ITERATIONS);

    munmap(chunk, MEM_SIZE);




    const void* sh_mem = shalloc(64);
    measure(sheap_write_test, sh_mem);
    duration = measure(sheap_write_test, sh_mem);
    shfree(sh_mem);
    printf("Secure write took %lu ns\n", duration / NUM_ITERATIONS);


    avg = 0;
    sh_mem = shalloc(MEM_SIZE);
    // map pages
    measure(sheap_read_test, sh_mem);
    for (int i = 0; i < NUM_TESTS; i++) {
        memset(cacheflush, 0, 1024*1024*1024);
        avg += measure(sheap_read_test, sh_mem);
    }
    shfree(sh_mem);
    printf("Secure read took %lu ns on average\n", avg / NUM_TESTS / NUM_ITERATIONS);

    return 0;
}