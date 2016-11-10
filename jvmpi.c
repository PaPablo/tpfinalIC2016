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

int cantidad_vecinos(int **grilla, int n, int i, int j){
    int ni,si;
    int ej,oj;

    //estos quedan como estaban 
    oj = modulo((j-1), n);
    ej = modulo((j+1), n);


   	//arreglo para mandar entorno
   	int entorno[3];

   	//arreglos para entornos de arriba y de abajo
   	int superior[3];
   	int inferior[3];

   	entorno[0] = grilla[i][oj];
   	entorno[1] = grilla[i][j];
   	entorno[2] = grilla[i][ej];

    //le mando mi entorno al vecino de arriba
   	MPI_Send(&entorno, 3, MPI_INT, modulo(i-1,n), 0, MPI_COMM_WORLD);

   	//le mando mi entorno al vecino de abajo
   	MPI_Send(&entorno, 3, MPI_INT, modulo(i+1,n), 0, MPI_COMM_WORLD);

   	//recibo del vecino de arriba;
   	MPI_Recv(&superior, 3, MPI_INT, modulo(i-1,n), 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);

   	//recibo del vecino de abajo;
   	MPI_Recv(&inferior, 3, MPI_INT, modulo(i+1,n), 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);

    //sumamos
    int cant = 
    superior[0]  	+ //vecino noroeste
    superior[1]  	+ //vecino norte
	superior[2]  	+ //vecino noreste
    inferior[0]  	+ //vecino suroeste
    inferior[1]  	+ //vecino sur
	inferior[2]		+ //vecino sureste
    grilla[i][oj]  	+ //vecino oeste
    grilla[i][ej];	  //vecino este
    
    //devolvemos la cantidad
    return cant;
}


int nuevo_estado(int **grilla, int n, int i, int j, int viejo_valor){
    

    int vecinos = cantidad_vecinos(grilla,n,i,j);

    // Para morir de inanicion
    // En este caso, menos de dos vecinos vivos
    if (vecinos < 2){
        return 0;
    } // Para pasar a la siguiente generacion
      // En este caso, dos o tres vecinos vivos
    else if ((vecinos==2)){ // no pongo el de 3 porque no le da vida al que debe
        return viejo_valor*1;
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

void prueba(int rank){
	int valor = rank;

	MPI_Send(&valor, 1, MPI_INT, modulo(rank+1,10), 0, MPI_COMM_WORLD);
	MPI_Recv(&valor, 1, MPI_INT, modulo(rank-1,10), 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);

	printf("soy %d, recibi %d\n", rank, valor);

}
int main(){

	int n = 10;

	int pasos = 10;

	int **grilla = crear_grilla(n); 
	int **nueva_grilla = crear_grilla(n);
    
	inicializar_grilla(grilla, n);
    MPI_Init(NULL, NULL);    

	int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if(size < 1){
        printf("Ingrese valor de procesos mayor a 1\n");
        exit(1);
    }
        
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   	
 	

 	grilla[3][3] = 1;
 	grilla[3][4] = 1;
 	grilla[3][5] = 1;
 	grilla[4][5] = 1;
 	grilla[5][4] = 1;
 	

    while(pasos){

            for (int j = 0; j < n ; j++) {
                nueva_grilla[rank][j] = nuevo_estado(grilla,n,rank,j, grilla[rank][j]);
            }

        
            for (int j = 0; j < n ; j++) {
                grilla[rank][j] = nueva_grilla[rank][j];
            }

    	MPI_Barrier(MPI_COMM_WORLD);    
        if(rank==0){
        	mostrar_arreglo(grilla,n);
        	printf("\n%d\n",pasos);
        	pasos--;
        }
    }

    
    MPI_Finalize();
    return 0;
}