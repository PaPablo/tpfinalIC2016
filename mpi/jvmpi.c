#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <unistd.h>

#define clear() printf("\033[H\033[J")
int size,rank;

int argumentosInconsistentes(int n, int size){
	if(((n*n)/size)%n != 0){
		return 1;
	}
	else return 0;
}

int palito(int **grilla){
    grilla[1][3] = 1;
    grilla[2][3] = 1;
    grilla[3][3] = 1;
}

int glider(int **grilla){

    grilla[1][3] = 1;
    grilla[1][4] = 1;
    grilla[1][5] = 1;
    grilla[2][3] = 1;
    grilla[3][4] = 1;

    return 0;
}

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

    for(i = 0;i<n;i++){
        for(j = 0; j<n;j++){
            grilla[i][j] = 0;
        }
    }
    return 0;

}

int random_init(int **grilla,int n){
    
    int i,j;
    srand(getpid());

    for(i = 0;i<n;i++){
        for(j = 0; j<n;j++){
            grilla[i][j] = rand() % 2;
        }
    }

    return 0;

}
int mostrar_grilla(int **grilla, int n){
    int i,j;
   
    usleep(90000);
    clear();

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

int cantidad_vecinos(int filas, int columnas, int i, int j,int strip[filas][columnas], int *superior, int *inferior){

    int oj = modulo(j-1,columnas);
    int ej = modulo(j+1,columnas);
    int norte,noreste, este, sureste, sur, suroeste, oeste, noroeste;
    este = strip[i][ej];
    oeste = strip[i][oj];
    if(filas > 1){
        if(i == 0){
            noroeste = superior[oj];
            norte = superior[j];
            noreste = superior[ej];
            suroeste = strip[i+1][oj];
            sureste = strip[i+1][ej];
            sur =  strip[i+1][j];
        } else if(i == filas-1){
            noroeste = strip[i-1][oj];
            norte = strip[i-1][j];
        	noreste = strip[i-1][ej];
            suroeste = inferior[oj];
            sur =  inferior[j];
            sureste= inferior[ej];
        } else {
            suroeste = strip[i+1][oj];
            sur =  strip[i+1][j];
            sureste = strip[i+1][ej];
            noroeste = strip[i-1][oj];
            norte = strip[i-1][j];
            noreste = strip[i-1][ej];
        }
    } else{
        noroeste = superior[oj];
        norte = superior[j];
        noreste = superior[ej];
        suroeste = inferior[oj];
        sur =  inferior[j];
        sureste = inferior[ej];
    }
    //sumamos
    int cant = 
    norte + 
    noreste +
    este +
    sureste + 
    sur + 
    suroeste + 
    oeste + 
    noroeste;
    
    return cant;
}


int nuevo_estado(int filas, int columnas, int i, int j, int strip[filas][columnas], int *superior, int *inferior){
    int vivo;
    int vecinos = cantidad_vecinos(filas,columnas,i,j,strip,superior,inferior);

    // Para morir de inanicion
    // En este caso, menos de dos vecinos vivos
    if (vecinos < 2){
        vivo = 0;
    } // Para pasar a la siguiente generacion
      // En este caso, dos o tres vecinos vivos
    else if ((vecinos==2)){ // no pongo el de 3 porque no le da vida al que debe
        vivo =  strip[i][j];
    }
    // Para que una celda comience a vivir    
    // En este caso 3 vecinos vivos.
    /* NOTA: este codigo no se ejecuta nunca, 
    *  pero queda acá por si se quierem cambiar
    *  las reglas en algun momento
    */
    else if(vecinos==3){
        vivo = 1;
    }
    // Para matar una celda de sobre poblacion
    // En este caso mas de 3 vecinos vivos
    else if(vecinos > 3){
        vivo = 0;
    }


    return vivo;
    
}

void mostrar_strip(int *strip, int n) {
    for(int j = 0; j < n; j++){
        printf("%d ",strip[j]);
    }
    printf("\n");
    return;
}

int *crear_inferior(int filas, int columnas, int strip[filas][columnas]){
	int *inferior = crear_strip(columnas);

	int i;
	for(i = 0; i<columnas ; i++){
		inferior[i] = strip[filas-1][i];
	}
	return inferior;
}

int *crear_superior(int filas, int columnas, int strip[filas][columnas]){
	int *superior = crear_strip(columnas);
  	int i;
	for(i = 0; i<columnas ; i++){
		superior[i] = strip[0][i];
	}
	return superior;
}

int **generacion(int **grilla, int n){
    int **nueva_grilla = crear_grilla(n,n);
  
    int size,rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  	int cantidadElementos = (n*n)/size;
    int cantidadFilas = cantidadElementos/n;
  	int strip[cantidadFilas][n];
    int nuevo_strip[cantidadFilas][n];
 

    //Tipo de dato derivado para hacer el scatter
    MPI_Datatype filaGrilla;
    MPI_Type_vector(n, 1, n, MPI_INT, &filaGrilla);
    MPI_Type_commit(&filaGrilla);



    //reparto
    MPI_Scatter(*grilla, cantidadElementos, MPI_INT,
                *strip, cantidadElementos ,MPI_INT ,
                0, MPI_COMM_WORLD);


    //mostrar_strip(strip,n);

    int *superior = crear_superior(cantidadFilas, n, strip);

    int *inferior = crear_inferior(cantidadFilas, n, strip);


    MPI_Send(inferior,n,MPI_INT, modulo(rank+1,size),0,MPI_COMM_WORLD);
    MPI_Send(superior,n,MPI_INT, modulo(rank-1,size),0,MPI_COMM_WORLD);
    
    MPI_Recv(superior, n,MPI_INT, modulo(rank-1,size),0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    MPI_Recv(inferior, n,MPI_INT, modulo(rank+1,size),0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    

    /*printf("R: %d Superior:\n",rank);
    for(int t = 0; t < n;t++){
        printf("%d", superior[t]);
    }
    printf("\n");
    printf("R: %d Strip:\n",rank);
    for(int t = 0; t < cantidadFilas;t++){
        for(int q = 0; q < n;q++){
           printf("%d", strip[t][q]); 
        }
        printf("\n");
    }
    printf("R: %d Inferior:\n",rank);
    for(int t = 0; t < n;t++){
        printf("%d", inferior[t]);
    }*/

    for(int i = 0; i < cantidadFilas;i++){
	    for (int j = 0; j < n; j++){
	        int vivo = nuevo_estado(cantidadFilas,n,i,j,strip,superior,inferior);
	        nuevo_strip[i][j] = vivo;
	    }
	}

	for(int i = 0; i < cantidadFilas ; i++){
	    for(int j = 0; j < n ; j++){
	        strip[i][j] = nuevo_strip[i][j];
	    }
	}

    MPI_Gather(strip, cantidadElementos, MPI_INT, 
        *nueva_grilla, cantidadElementos, MPI_INT, 
        0, MPI_COMM_WORLD);
    
    MPI_Barrier(MPI_COMM_WORLD);
    if(!rank){
    	mostrar_grilla(nueva_grilla, n);
    }
    return nueva_grilla;

}

int main(int argc, char const *argv[]){
   
    MPI_Init(NULL, NULL);    
    int pasos = 100;
    int n = atoi(argv[1]);
    



	
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if(size < 1){
        printf("Ingrese valor de procesos mayor a 1\n");
        exit(1);
    }
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int **grilla = crear_grilla(n,n);
    if(rank == 0){
    	if(argumentosInconsistentes(n,size)){
    		printf("Argumentos Inconsistentes\n");
    		exit(1);
    	}
    	printf("Asigno glider\n");
        inicializar_grilla(grilla,n);
        //glider(grilla);
        random_init(grilla, n);
    }
    
    while(pasos){
        grilla = generacion(grilla, n);
        pasos--;
    }   
    

    
    MPI_Finalize();
    return 0;
} 