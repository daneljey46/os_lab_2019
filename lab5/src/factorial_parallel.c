#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <getopt.h>

struct FactorialArgs {
    int start;
    int end;
    int mod;
};

long long total_result = 1;
pthread_mutex_t result_mutex = PTHREAD_MUTEX_INITIALIZER;

void* ThreadFactorial(void* args) {
    struct FactorialArgs* f_args = (struct FactorialArgs*)args;
    long long local_res = 1;

    for (int i = f_args->start; i <= f_args->end; i++) {
        local_res = (local_res * i) % f_args->mod;
    }

    pthread_mutex_lock(&result_mutex);
    total_result = (total_result * local_res) % f_args->mod;
    pthread_mutex_unlock(&result_mutex);

    return NULL;
}

int main(int argc, char** argv) {
    int k = -1;
    int pnum = -1;
    int mod = -1;

    static struct option options[] = {
        {"k", required_argument, 0, 'k'},
        {"pnum", required_argument, 0, 0},
        {"mod", required_argument, 0, 0},
        {0, 0, 0, 0}
    };

    int option_index = 0;
    int c;
    while ((c = getopt_long(argc, argv, "k:", options, &option_index)) != -1) {
        switch (c) {
            case 'k': k = atoi(optarg); break;
            case 0:
                if (option_index == 1) pnum = atoi(optarg);
                if (option_index == 2) mod = atoi(optarg);
                break;
        }
    }

    if (k < 0 || pnum <= 0 || mod <= 0) {
        printf("Usage: %s -k 10 --pnum=4 --mod=10\n", argv[0]);
        return 1;
    }

    pthread_t threads[pnum];
    struct FactorialArgs args[pnum];

    int chunk_size = k / pnum;
    for (int i = 0; i < pnum; i++) {
        args[i].start = i * chunk_size + 1;
        args[i].end = (i == pnum - 1) ? k : (i + 1) * chunk_size;
        args[i].mod = mod;

        if (pthread_create(&threads[i], NULL, ThreadFactorial, &args[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    for (int i = 0; i < pnum; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("%d! mod %d = %lld\n", k, mod, total_result);

    return 0;
}