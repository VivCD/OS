#include <stdio.h>

int main() {
    int a = 1;
    int b = 2;
    int c = a + b;
    int d = c + 3; // warning: unused variable 'd'
    if (a = b) { // warning: assignment in conditional expression
        printf("a is equal to b\n");
    }
    int e; // warning: unused variable 'e'
    return 0;
}
