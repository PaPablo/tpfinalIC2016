#include <stdio.h>
#include <stdlib.h>


int **crear_grilla(int filas, int columnas) {
  int **result;
  int i;

  /* Allocate an array of pointers to hold pointers to the rows of the
     array */
  result=(int **)malloc(filas*sizeof(int *));

  /* The first pointer is in fact a pointer to the entire array */
  result[0]=(int *)malloc(filas*columnas*sizeof(int));

  /* The remaining pointers are just pointers into this array, offset
     in units of col_dim */
  for(i=1; i<filas; i++)
    result[i]=result[i-1]+columnas;

  return result;
}

int main(){

	int **grilla = crear_grilla(10,10);
	int fila[2] = {1,1};

	grilla[0][0] = 1;
	grilla[0][1] = 1;
	grilla[1][1] = 1;
	grilla[1][0] = 1;
	printf("%0x\n",&fila[0]);
	printf("%0x\n",&fila[1]);
	
	printf("%0x\n",&grilla[0][0]);
	printf("%0x\n",&grilla[0][1]);
	printf("%0x\n",&grilla[1][0]);
	printf("%0x\n",&grilla[1][1]);

	return 0;

}