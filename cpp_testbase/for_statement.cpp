/** TEST CASE EXPECTED OUTPUT
0
0
0
0
0
*/

extern void printnum(int);

int main() {
    for (int i = 5; i; i=i-1) {
        printnum(0);
    }

    return 0;
}