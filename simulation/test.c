#include <stdlib.h>
#include <stdio.h>

typedef struct cell {
	int value;
	int test;
} cell;

cell get_cell(cell* tab, int n) {
	return tab[n];
}

int main() {
	cell* tab = (cell*)malloc(2*sizeof(cell));
	cell t1 = { 2, 3};
	cell t2 = { 4, 5};
	tab[0] = t1;
	tab[1] = t2;
	printf("aled : %i, %i\n", get_cell(tab, 1).value, get_cell(tab, 0).test);
	return 0;
}
