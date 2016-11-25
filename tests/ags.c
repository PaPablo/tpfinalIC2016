#include <stdio.h>
#include <stdlib.h>

int asignar(int filas, int columnas, int a[filas][columnas]){

	for(int i = 0; i < filas; i++){
		for(int j = 0; j < columnas ; j++){
			a[i][j] = i+j;
		}
	}
}
int main(int argc, char const *argv[])
{

	int a[2][10];
	return 0;
}