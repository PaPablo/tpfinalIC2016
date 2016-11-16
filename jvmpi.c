#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <unistd.h>

int** crear_grilla(int n){
    int i,j;
    int **grilla;
    //pido memoria para filas
    grilla = malloc(n * sizeof(int*));

    //memoria para cada columna
    for(j = 0; j<n;j++){
        grilla[j] = malloc(n * sizeof(int));
    }

    //chequeo si anduvo
    if(grilla[n-1] == NULL){
        fprintf(stderr, "malloc failed\n");
        exit(-1);
    }    
    
    return grilla; 
}

int* crear_strip(int n){
    int i;
    int *strip;

    //pido memoria para celda
    strip = malloc(n * sizeof(int*));

    //chequeo si anduvo
    if(strip[n-1] == NULL){
        fprintf(stderr, "malloc failed\n");
        exit(-1);
    }    
    
    return strip; 
}


int inicializar_grilla(int **grilla, int n){
    int i,j;

    srand(getpid());

    for(i = 0;i<n;i++){
        for(j = 0; j<n;j++){
            grilla[i][j] = rand() % 2;
        }
    }
    return 0;

}

int mostrar_arreglo(int **grilla, int n){
    int i,j;
    
    for(i = 0;i<n;i++){
        for(j = 0; j<n;j++){
            printf("%d ",grilla[i][j]);
        }
        printf("\n");
    }
    return 0;
}

int modulo(int valor, int n){
    int aux = valor % n;

    if(aux<0){
        return n-1;
    }
    else {
        return aux;
    }
}

int cantidad_vecinos(int *strip, int n, int i, int j){

    printf("Vecinos %d %d %d\n",n,i,j);
    int vecino_norte, 
        vecino_noroeste,
        vecino_noreste,
        vecino_sur,
        vecino_suroeste,
        vecino_sureste,
        envio;
    printf("oj\n");
    int oj = modulo(j-1,n);
    printf("ej\n");
    int ej = modulo(j+1,n);
    /* Aca es donde comeinza el intercambio
     * Como estoy trabajando en strips, y se que hay dos strips,
     * de los cuales tres celdas necesitan datos de este strip, 
     * los mando y quedan para usarse cuando se lo requiera
     */
    
    envio = strip[j];
    printf("About to send\n");
    for(int requeridos = 0; requeridos < 3; requeridos++){

        /* Pongo como tag la columna asi al requerir
         * un valor, se lo pide a través del tag
         */

        //le mando mi valor al vecino de arriba
        MPI_Send(&envio, 1, MPI_INT, modulo(i-1,n), j, MPI_COMM_WORLD);

        //le mando mi valor al vecino de abajo
        MPI_Send(&envio, 1, MPI_INT, modulo(i+1,n), j, MPI_COMM_WORLD);
    }   
    printf("sent\n");


   	//recibo vecino noroeste
   	MPI_Recv(&vecino_noroeste, 1, MPI_INT, modulo(i-1,n), modulo(j-1,n), MPI_COMM_WORLD,MPI_STATUS_IGNORE);

    //recibo vecino norte
    MPI_Recv(&vecino_norte, 1, MPI_INT, modulo(i-1,n), j, MPI_COMM_WORLD,MPI_STATUS_IGNORE);

    //recibo vecino noreste
    MPI_Recv(&vecino_noreste, 1, MPI_INT, modulo(i-1,n), modulo(j+1,n), MPI_COMM_WORLD,MPI_STATUS_IGNORE);

    //recibo vecino suroeste
    MPI_Recv(&vecino_suroeste, 1, MPI_INT, modulo(i+1,n), modulo(j-1,n), MPI_COMM_WORLD,MPI_STATUS_IGNORE);

    //recibo vecino sur
    MPI_Recv(&vecino_sur, 1, MPI_INT, modulo(i+1,n), j, MPI_COMM_WORLD,MPI_STATUS_IGNORE);

    //recibo vecino sureste
    MPI_Recv(&vecino_sureste, 1, MPI_INT, modulo(i+1,n), modulo(j+1,n), MPI_COMM_WORLD,MPI_STATUS_IGNORE);

    //sumamos
    int cant = 
    vecino_noroeste + //vecino noroeste
    vecino_norte    + //vecino norte
	vecino_noreste  + //vecino noreste
    vecino_suroeste + //vecino suroeste
    vecino_sur  	+ //vecino sur
	vecino_sureste	+ //vecino sureste
    grilla[i][oj]  	+ //vecino oeste
    grilla[i][ej];	  //vecino este
    
    //devolvemos la cantidad
    return cant;
}


int nuevo_estado(int *strip, int n, int j, int viejo_valor){
    printf("Nuevo estado\n");

    int vecinos = cantidad_vecinos(strip,n,i,j);

    // Para morir de inanicion
    // En este caso, menos de dos vecinos vivos
    if (vecinos < 2){
        return 0;
    } // Para pasar a la siguiente generacion
      // En este caso, dos o tres vecinos vivos
    else if ((vecinos==2)){ // no pongo el de 3 porque no le da vida al que debe
        return viejo_valor;
    }
    // Para que una celda comience a vivir    
    // En este caso 3 vecinos vivos.
    /* NOTA: este codigo no se ejecuta nunca, 
    *  pero queda acá por si se quierem cambiar
    *  las reglas en algun momento
    */
    else if(vecinos==3){
        return 1;
    }
    // Para matar una celda de sobre poblacion
    // En este caso mas de 3 vecinos vivos
    else if(vecinos > 3){
        return 0;
    }
    
}

int main(){
   
    MPI_Init(NULL, NULL);    

    int pasos = 10;
    int n = 10;
    int **grilla; 
    int *strip = crear_strip(n);
    int *nuevo_strip = crear_strip(n);
    int **nueva_grilla; 
	int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if(size < 1){
        printf("Ingrese valor de procesos mayor a 1\n");
        exit(1);
    }
        
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   	

    if(rank == 0){
        printf("Creamos primera grilla\n");
        grilla = crear_grilla(n); 
        nueva_grilla = crear_grilla(n);
        inicializar_grilla(grilla, n);  

        // SCATTERRRR
        MPI

        printf("Asigno glider\n");
        /*Glider*/
        grilla[3][3] = 1;
        grilla[3][4] = 1;
        grilla[3][5] = 1;
        grilla[4][5] = 1;
        grilla[5][4] = 1;
    }


    while(pasos){

            for (int j = 0; j < n ; j++) {
                nueva_grilla[rank][j] = nuevo_estado(grilla,n,rank,j, grilla[rank][j]);
            }

        
            for (int j = 0; j < n ; j++) {
                grilla[rank][j] = nueva_grilla[rank][j];
            }

    	MPI_Barrier(MPI_COMM_WORLD);


        if(rank==0){
            // GATHERRRRR

        	mostrar_arreglo(grilla,n);
        	printf("\n%d\n",pasos);
        	pasos--;
        }
    }

    
    MPI_Finalize();
    return 0;
}