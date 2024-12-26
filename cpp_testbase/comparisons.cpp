/** TEST CASE EXPECTED OUTPUT
3
4
5
9
10
11
12
16
17
19
20
24
25
26
27
28
29
33
37
38
42
43
44
45
46
47
51
*/

extern void printnum(int);

int main() {
    // 6 operations, 3 variables
    // => 18 total mutable combinations
    // or 36 total non mutable combinations
    // or 54 total non mutable combinations with self comparison

    int x = 5;
    int y = 4;
    int z = 4;

    // I checked these manually to add the eol comment (Quint)
    if (x == y) { printnum(1); } // false
    if (x == z) { printnum(2); } // false
    if (x == x) { printnum(3); } // true
    if (x != y) { printnum(4); } // true
    if (x != z) { printnum(5); } // true
    if (x != x) { printnum(6); } // false
    if (x <= y) { printnum(7); } // false
    if (x <= z) { printnum(8); } // false
    if (x <= x) { printnum(9); } // true
    if (x >= y) { printnum(10); } // true
    if (x >= z) { printnum(11); } // true
    if (x >= x) { printnum(12); } // true
    if (x <  y) { printnum(13); } // false
    if (x <  z) { printnum(14); } // false
    if (x <  x) { printnum(15); } // false
    if (x >  y) { printnum(16); } // true
    if (x >  z) { printnum(17); } // true
    if (x >  x) { printnum(18); } // false

    if (y == y) { printnum(19); } // true
    if (y == z) { printnum(20); } // true
    if (y == x) { printnum(21); } // false
    if (y != y) { printnum(22); } // false
    if (y != z) { printnum(23); } // false
    if (y != x) { printnum(24); } // true
    if (y <= y) { printnum(25); } // true
    if (y <= z) { printnum(26); } // true
    if (y <= x) { printnum(27); } // true
    if (y >= y) { printnum(28); } // true
    if (y >= z) { printnum(29); } // true
    if (y >= x) { printnum(30); } // false
    if (y <  y) { printnum(31); } // false
    if (y <  z) { printnum(32); } // false
    if (y <  x) { printnum(33); } // true
    if (y >  y) { printnum(34); } // false
    if (y >  z) { printnum(35); } // false
    if (y >  x) { printnum(36); } // false

    if (z == y) { printnum(37); } // true
    if (z == z) { printnum(38); } // true
    if (z == x) { printnum(39); } // false
    if (z != y) { printnum(40); } // false
    if (z != z) { printnum(41); } // false
    if (z != x) { printnum(42); } // true
    if (z <= y) { printnum(43); } // true
    if (z <= z) { printnum(44); } // true
    if (z <= x) { printnum(45); } // true
    if (z >= y) { printnum(46); } // true
    if (z >= z) { printnum(47); } // true
    if (z >= x) { printnum(48); } // false
    if (z <  y) { printnum(49); } // false
    if (z <  z) { printnum(50); } // false
    if (z <  x) { printnum(51); } // true
    if (z >  y) { printnum(52); } // false
    if (z >  z) { printnum(53); } // false
    if (z >  x) { printnum(54); } // false

    return 0;
}