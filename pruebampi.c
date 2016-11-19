#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <unistd.h>

#define TAM 10

int mostrar_grilla(int grilla[10][10]){
    int i,j;
    
    for(i = 0;i<10;i++){
        for(j = 0; j<10;j++){
            printf("%d ",grilla[i][j]);
        }
        printf("\n");
    }
    return 0;
}

int main(){
    //creo arreglo para mandar
    int n[TAM][TAM];
    int nn[TAM][TAM];
    int size,rank;

    MPI_Init(NULL, NULL);    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   	
    //Tipo de dato derivado para hacer el scatter
    MPI_Datatype filaGrilla;
    MPI_Type_vector(TAM, 1, TAM, MPI_INT, &filaGrilla);
    MPI_Type_commit(&filaGrilla);

    int strip[TAM];


    if(!rank){
        //inicializo matriz
        int numero = 0,i,j; 
        for(i = 0; i < TAM; i++){
            for(j = 0; j < TAM; j++){
                n[i][j] = numero++;
            }
        }
       // mostrar_grilla(n, TAM);
    }


    
    //reparto
    MPI_Scatter(n, TAM, MPI_INT,
                strip, TAM,MPI_INT ,
                0, MPI_COMM_WORLD);
    
    if(rank ==1){
        printf("Muestro fila %d desde rank: %d\n", rank, rank);
        int j;
        for (j = 0; j < TAM ; j++){
            printf("%d ", strip[j]);
        }
        printf("\n\n");

    }

    MPI_Gather(strip, 1, filaGrilla, 
        n, 1, filaGrilla, 
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
