/** TEST CASE EXPECTED OUTPUT
5
4
3
2
1
*/

extern void printnum(int);

int main() {
    int i = 5;
    while (i) {
        printnum(i);
        i = i - 1;
    }

    return 0;
}