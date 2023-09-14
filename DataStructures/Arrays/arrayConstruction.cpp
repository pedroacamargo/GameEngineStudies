/**
 * It is basically a header file that includes every standard library. 
 * In programming contests, using this file is a good idea, when you want to reduce the time wasted in doing chores
 * Especially when your rank is time sensitive. 
*/
#include <bits/stdc++.h>
using namespace std;

class DinamicArray {
public:
    int *arr; // Create array pointer
    int capacity; // Array actual size
    int totalNumbers; // Total numbers in the current array

    DinamicArray() {
        arr = new int[2];
        capacity = 2;
        totalNumbers = 0;
    }

    void push(int value) {
        if (capacity == totalNumbers) {

            // malloc memory for a new array with double capacity
            int * temp = new int[2 * capacity];

            // copy elements from array to the new array
            for (int i = 0; i < capacity; i++) {
                temp[i] = arr[i];
            }

            // Free the memory from older array
            delete[] arr;
            capacity *= 2;
            arr = temp;
        }

        // Push the value to the end
        arr[totalNumbers] = value;
        totalNumbers++;
    }

    void push(int value, int index) {
        // If given an index, replace the number in the index with the value given in parameter
        if (index == capacity) {
            push(value); // check for end case 
        } else {
            arr[index] = value; // insert data
        }
    }

    void insertElements(int value, int index) {
        // in this function we don't replace the original number
        if (index == capacity) {
            push(value);
        }

        for(int i = totalNumbers; i > index; i--) {
            arr[i] = arr[i - 1];
        }

        arr[index] = value;
        totalNumbers++;
    }

    int pop() {
        // Remove the last element of the array
        int elem = arr[totalNumbers - 1];
        totalNumbers--;
        return elem;
    }

    int pop(int index) {
        // Remove an element from a given index
        if (index > -1 && index < totalNumbers) {
            totalNumbers--;
            for (int j = index; j < totalNumbers; j++) {
                arr[j] = arr[j + 1];
            }
        }
    }

    int get(int index) {
        // returns the value in the given index
        if (index > -1 && index < totalNumbers) {
            return arr[index];
        }
    }

    int length() {
        // Get the size of array
        return totalNumbers;
    }

    void print() {
        cout << "[";
        for (int i = 0; i < totalNumbers; i++) {
            if (i == totalNumbers - 1) {
                cout << arr[i] << "]";
            } else {
                cout << arr[i] << ", ";
            }
        }
        cout << endl;
    }
};

int main() {
    DinamicArray dinArr;

    dinArr.push(10);
    dinArr.push(5);
    dinArr.push(9);
    dinArr.print();

    return 0;
}