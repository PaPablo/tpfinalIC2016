#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <unistd.h>

#define TAM 10

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
    strip = malloc(n * sizeof(int *));

    //chequeo si anduvo
    if(strip == NULL){
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
            grilla[i][j] =0;
        }
    }
    return 0;

}

int mostrar_grilla(int **grilla, int n){
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

    int oj = modulo(j-1,n);
    int ej = modulo(j+1,n);
    /* Aca es donde comeinza el intercambio
     * Como estoy trabajando en strips, y se que hay dos strips,
     * de los cuales tres celdas necesitan datos de este strip, 
     * los mando y quedan para usarse cuando se lo requiera
     */

    printf("About to send\n");

    /* Pongo como tag la columna asi al requerir
     * un valor, se lo pide a través del tag
     */

    //le mando mi valor al vecino de arriba
    MPI_Send(&strip, n, MPI_INT, modulo(i-1,n), 0, MPI_COMM_WORLD);

    //le mando mi valor al vecino de abajo
    MPI_Send(&strip, n, MPI_INT, modulo(i+1,n), 0, MPI_COMM_WORLD);

    printf("sent\n");

    int superior[n];
    int inferior[n];

    MPI_Recv(&superior, n, MPI_INT, modulo(i-1,n), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 

    MPI_Recv(&inferior, n, MPI_INT, modulo(i+1,n), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    //sumamos
    int cant = 
    superior[oj] + //vecino noroeste
    superior[j]    + //vecino norte
	superior[ej]  + //vecino noreste
    inferior[oj] + //vecino suroeste
    inferior[j]  	+ //vecino sur
	inferior[ej]	+ //vecino sureste
    strip[oj]  	+ //vecino oeste
    strip[ej];	  //vecino este
    
    //devolvemos la cantidad
    return cant;
}


int nuevo_estado(int *strip, int n, int i, int j, int viejo_valor){
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
    int **grilla = crear_grilla(TAM);
    int *strip = crear_strip(TAM);
	int size,rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if(size < 1){
        printf("Ingrese valor de procesos mayor a 1\n");
        exit(1);
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   	
    //Declaro tipo Grilla
    MPI_Datatype filaGrilla;
    MPI_Type_vector(TAM, 1, TAM, MPI_INT, &filaGrilla);
    MPI_Type_commit(&filaGrilla);

    if(rank == 0){
        printf("Creamos primera grilla\n"); 
        printf("Asigno glider\n");
        inicializar_grilla(grilla,TAM);
        mostrar_grilla(grilla, TAM);
    }

    MPI_Scatter(grilla, TAM, MPI_INT,
            strip, TAM, MPI_INT,
            0, MPI_COMM_WORLD);
    /*
    for (int j = 0; j < TAM ; j++){
        nuevo_strip[j] = nuevo_estado(strip,TAM,rank, j, strip[j]);
    }
    for(int j = 0; j < TAM ; j++){
        strip[j] = nuevo_strip[j];
    }*/
    for(int i = 0; i < TAM ; i++){
        printf("Soy %d, muestro en %d el valor %d\n", rank, i,strip[i]);
    }

    MPI_Gather(&strip, 1, filaGrilla, 
        grilla, 1, filaGrilla, 
        0, MPI_COMM_WORLD);
  
    MPI_Barrier(MPI_COMM_WORLD);
    if(!rank){
        printf("bout to show\n");
        mostrar_grilla(grilla, TAM);
    }

    

    
    MPI_Finalize();
    return 0;
}