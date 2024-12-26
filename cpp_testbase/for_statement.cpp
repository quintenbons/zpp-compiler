/** TEST CASE EXPECTED OUTPUT
5
4
3
2
1
*/

extern void printnum(int);

int main() {
    for (int i = 5; i; i=i-1) {
        printnum(i);
    }

    return 0;
}