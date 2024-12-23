/** TEST CASE EXPECTED OUTPUT
0
3
5
6
8
*/

extern void printnum(int);

int main() {
    if (1) {
        printnum(0);
    } else {
        printnum(1);
    }

    if (0) {
        printnum(2);
    } else if (1) {
        printnum(3);
    } else {
        printnum(4);
    }

    if (1) {
        printnum(5);
    }

    printnum(6);

    if (0) {
        printnum(7);
    } else {
        printnum(8);
    }

    return 0;
}