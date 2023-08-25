#include <stdio.h>
#include <time.h>

void logFirstTwoName(char ** strings) {
    printf(*strings);
    printf("\n");
    printf(*(strings + 1));
    printf("\n");
}

int main() {
    // char strings[3][10] = {"Pedro", "Augusto", "Camargo"}; -> This case occupies more memory
    char * strings[] = {"ola", "hello", "dory","ola", "hello", "dory","ola", "hello", "dory","ola", "hello", "dory","ola", "hello", "dory","ola", "hello", "dory","ola", "hello", "dory","ola", "hello", "dory","ola", "hello", "dory","ola", "hello", "dory", "Nemo"};

    // O(2) -> No matter how big the string array get, the number of operations won't increase, because we are printing
    // just the first 2 elements of the array.
    // It's a constant time, a Y axis straight line.
    logFirstTwoName(strings);


    return 0;
}