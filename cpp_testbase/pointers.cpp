/** Expected
1
2 -> not yet
*/

extern void printnum(int);

int main()
{
    int v = 1;
    int *ptr = &v;
    printnum(v);
    // *ptr = 2;
    // printnum(v);
    return 0;
}