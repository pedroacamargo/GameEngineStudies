#include <stdio.h>
#include <time.h>

int main() {
    // char strings[3][10] = {"Pedro", "Augusto", "Camargo"}; -> This case occupies more memory
    char * strings[] = {"ola", "hello", "dory","ola", "hello", "dory","ola", "hello", "dory","ola", "hello", "dory","ola", "hello", "dory","ola", "hello", "dory","ola", "hello", "dory","ola", "hello", "dory","ola", "hello", "dory","ola", "hello", "dory", "Nemo"};
 
    // Since different types of PCs can handle the array data in dissimilar time, 
    // the timer isn't a good choice to test any algorithm performance.
    clock_t timer = clock();


    // Loop -> O(n), linear time
    // For each element in an array, the amount of actions is incremented
    // 6 elements -> 6 operations
    // This creates a linear line in a graphic
    for (int i = 0; i < sizeof(strings)/sizeof(strings[0]); i++) {
        if (strings[i] == "Nemo") {
            printf("Found nemo!\n");
        }
    }

    // Timer Output
    timer = clock() - timer;
    double timeTaken = ((double)timer)/CLOCKS_PER_SEC;


    
    printf("The loop took %f seconds to execute \n", timeTaken);

    return 0;
}