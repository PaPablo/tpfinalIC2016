#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <unistd.h>

#define TAM 10

int **crear_grilla(int n) {
  int **result;
  int i;

  /* Allocate an array of pointers to hold pointers to the rows of the
     array */
  result=(int **)malloc(n*sizeof(int *));

  /* The first pointer is in fact a pointer to the entire array */
  result[0]=(int *)malloc(n*n*sizeof(int));

  /* The remaining pointers are just pointers into this array, offset
     in units of col_dim */
  for(i=1; i<n; i++)
    result[i]=result[i-1]+n;

  return result;
}

int mostrar_grilla(int **grilla){
    int i,j;
    
    for(i = 0;i<10;i++){
        for(j = 0; j<10;j++){
            printf("%.2d ",grilla[i][j]);
        }
        printf("\n");
    }
    return 0;
}

int mostrar_strip(int *strip, int n) {
    for(int j = 0; j < n; j++){
        printf("%d ",strip[j]);
    }
    printf("\n");
    return 0;
}

int* crear_strip(int n){
    int i;
    int *strip;

    //pido memoria para celda
    strip = malloc(n * sizeof(int *));

    //chequeo si anduvo
    if(strip == NULL){
        fprintf(stderr, "malloc failed\n");
        exit(-1);
    }    
    
    return strip; 
}

int main(){
    //creo arreglo para mandar
    int **n = crear_grilla(TAM);
    int size,rank;

    MPI_Init(NULL, NULL);    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   	
    //Tipo de dato derivado para hacer el scatter
    MPI_Datatype filaGrilla;
    MPI_Type_vector(TAM, 1, TAM, MPI_INT, &filaGrilla);
    MPI_Type_commit(&filaGrilla);

    int *strip = crear_strip(TAM);


    if(!rank){
        //inicializo matriz
        int numero = 0,i,j; 
        for(i = 0; i < TAM; i++){
            for(j = 0; j < TAM; j++){
                n[i][j] = numero++;
            }
        }
        mostrar_grilla(n);
    }


    
    //reparto
    MPI_Scatter(*n, TAM, MPI_INT,
                strip, TAM,MPI_INT ,
                0, MPI_COMM_WORLD);
    
    mostrar_strip(strip, TAM);


    MPI_Gather(strip, TAM, MPI_INT, 
        *n, TAM, MPI_INT, 
        0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    if(!rank){
        printf("Muestro toda la matriz despues del gather desde rank: %d\n", rank);
        
        mostrar_grilla(n);
	printf("\n\n");
    }
    MPI_Finalize();
    return 0;

}
