#include <stdio.h>

// Given 2 arrays, create a function that let's a user know (true/false) whether these two arrays contains any common items
// For example: 
// const array1 = ['a', 'b', 'c', 'x']
// const array2 = ['z', 'y', 'i']
// should return false
// --------------
// const array1 = ['a', 'b', 'c', 'x']
// const array2 = ['z', 'y', 'x']
// should return false


// This is an unoptimized manner of handling the data
// O(N^2)
void unoptimized() {
    const char array[] = {'a', 'b', 'c', 'x'};    
    const char array2[] = {'z', 'y', 'x'};    

    for (int i = 0; i < sizeof(array)/sizeof(array[0]); i++) {
        for (int j = 0; j < sizeof(array2)/sizeof(array2[0]); j++) {
            if (array[i] == array2[j]) {
                printf("TRUE\n");
                return;
            }
        }
    }   

    printf("FALSE\n");
}

int main() {

    // unoptimized();
    

    return 0;
}