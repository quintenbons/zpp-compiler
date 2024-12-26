/** TEST CASE EXPECTED OUTPUT
0
3
5
6
8
*/

extern void printnum(int);

int main() {
    int i = 0;
    while (i < 5) {
        i += 1;
        printnum(0);
    }

    return 0;
}