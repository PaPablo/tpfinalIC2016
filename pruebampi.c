#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <unistd.h>

#define TAM 10

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

int mostrar_grilla(int grilla[10][10], int n){
    int i,j;
    
    for(i = 0;i<n;i++){
        for(j = 0; j<n;j++){
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
   	
    MPI_Datatype filaGrilla;
    MPI_Type_vector(TAM, 1, TAM, MPI_INT, &filaGrilla);
    MPI_Type_commit(&filaGrilla);

    int strip[TAM];


    if(!rank){
        //inicializo
        int numero = 0; 
        for(int i = 0; i < TAM; i++){
            for(int j = 0; j < TAM; j++){
                n[i][j] = numero++;
            }
        }

        /*
        int envio[TAM];
        for(int i = 0; i < TAM ; i++){
            for(int j = 0; j<TAM; j++){
                envio[j] = n[i][j];
            }
            MPI_Send(&envio, TAM, MPI_INT, i+1, 0, MPI_COMM_WORLD);
        }*/
    }
    else{
        /*int recibo[TAM];
        printf("Soy %d y voy a recibir\n", rank);
        MPI_Recv(&recibo, TAM, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int i = 0; i < TAM ; i++){
            printf("Soy %d y digo %d\n", rank, recibo[i]);
        }
        printf("Soy %d y ya mostre\n",rank);*/
    }

    MPI_Scatter(n, TAM, MPI_INT,
                strip, TAM,MPI_INT ,
                0, MPI_COMM_WORLD);

    /*for(int j = 0; j < TAM ; j++){
        printf("Soy %d y muestro %d\n", rank, strip[j]);
    }*/
    
    if(rank ==1){
        for (int j = 0; j < TAM ; j++){
            printf("%d ", strip[j]);
        }
        printf("\n");
    }
    MPI_Gather(&strip, 1, filaGrilla, 
        n, 1, filaGrilla, 
        0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    if(!rank){
        mostrar_grilla(n,TAM);
    }
    MPI_Finalize();
    return 0;

}