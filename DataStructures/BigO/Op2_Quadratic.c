#include <stdio.h>

// Quadratic time -> O(n^2)
// This happens when 2 loops are nested

void logAllPairsOfArray(int boxes[], int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%d, %d\n", boxes[i], boxes[j]);
        }
    }
}

int main() {
    int boxes[5] = {1, 2, 3, 4, 5};

    logAllPairsOfArray(boxes, 5);

    return 0;
}