#include <bits/stdc++.h>
using namespace std;
int main() {
    string str;
    cout << "Enter a string: " ;
    getline(cin, str);

    reverse(str.begin(), str.end());

    cout << "Reversed string is: " << str << endl;
}