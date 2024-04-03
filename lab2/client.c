#include <stdio.h>
#include <dlfcn.h>

#ifndef DYNAMIC_LINK
    #include "collatz.h"
#endif


int main() {
    int numbers[] = {312, 4312, 23123, 33, 123, 312, 4, 3, 223, 2};
    int size = sizeof(numbers) / sizeof(numbers[0]);

    #ifdef DYNAMIC_LINK
        void *lib_handle;
        int (*test_collatz_convergence)(int, int);

        lib_handle = dlopen("./libcollatz.so", RTLD_LAZY);

        test_collatz_convergence = dlsym(lib_handle, "test_collatz_convergence");
    #endif

    for (int i = 0; i < size; i++) {
        printf("Zadana liczba: %d, ilość iteracji: %d\n", numbers[i], test_collatz_convergence(numbers[i], 1000));
    }

    #ifdef DYNAMIC_LINK
        dlclose(lib_handle);
    #endif

    return 0;
}