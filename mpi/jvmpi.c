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

int rpentomino(int **grilla){
    grilla[1][2] = 1;
    grilla[2][2] = 1;
    grilla[3][2] = 1;
    grilla[1][3] = 1;
    grilla[2][1] = 1;
}


int pulsar(int **grilla){
    
    
    grilla[4][2] = 1;
    grilla[5][2] = 1;
    grilla[6][2] = 1;

    grilla[4][7] = 1;
    grilla[5][7] = 1;
    grilla[6][7] = 1;
    
    grilla[4][9] = 1;
    grilla[5][9] = 1;
    grilla[6][9] = 1;

    grilla[4][14] = 1;
    grilla[5][14] = 1;
    grilla[6][14] = 1;

//---------
    grilla[2][4] = 1;
    grilla[2][5] = 1;
    grilla[2][6] = 1;
    
    grilla[7][4] = 1;
    grilla[7][5] = 1;
    grilla[7][6] = 1;

    grilla[9][4] = 1;
    grilla[9][5] = 1;
    grilla[9][6] = 1;

    grilla[14][4] = 1;
    grilla[14][5] = 1;
    grilla[14][6] = 1;
    
    grilla[2][10] = 1;
    grilla[2][11] = 1;
    grilla[2][12] = 1;

    grilla[7][10] = 1;
    grilla[7][11] = 1;
    grilla[7][12] = 1;

    grilla[9][10] = 1;
    grilla[9][11] = 1;
    grilla[9][12] = 1;

    grilla[14][10] = 1;
    grilla[14][11] = 1;
    grilla[14][12] = 1;

    grilla[10][2] = 1;
    grilla[11][2] = 1;
    grilla[12][2] = 1;

    grilla[10][7] = 1;
    grilla[11][7] = 1;
    grilla[12][7] = 1;

    grilla[10][9] = 1;
    grilla[11][9] = 1;
    grilla[12][9] = 1;

    grilla[10][14] = 1;
    grilla[11][14] = 1;
    grilla[12][14] = 1;

    return 0;
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

  /* Ubicar array de punteros con punteros a las filas de la grillla */
  result=(int **)malloc(filas*sizeof(int *));

  /* El primer puntero es un puntero a la grilla completa */
  result[0]=(int *)malloc(filas*columnas*sizeof(int));

  /* El resto de los punteros son sólo punteros dentro de este arreglo */
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

    //inicializa toda la grilla en 0

    for(i = 0;i<n;i++){
        for(j = 0; j<n;j++){
            grilla[i][j] = 0;
        }
    }
    return 0;

}

int random_init(int **grilla,int n){
    
    int i,j;
    //Inicializamos semilla aleatoria
    srand(getpid());

    /* Asignamos a cada lugar de la matriz un 1 o un 0
        de forma aleatoria */
    for(i = 0;i<n;i++){
        for(j = 0; j<n;j++){
            grilla[i][j] = rand() % 2;
        }
    }

    return 0;

}
int mostrar_grilla(int **grilla, int n){
    int i,j;    
    
    //realizamos una espera
    usleep(90000);
    //limpiamos la pantalla
    clear();

    //mostramos la grilla
    for(i = 0;i<n;i++){
        for(j = 0; j<n;j++){
            printf("%d ",grilla[i][j]);
        }
        printf("\n");
    }
    return 0;
}

//funcion para hacer el wrapping alrededor de la grilla
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
    if(filas > 1){ //si las filas del strip son mas de una
        if(i == 0){ // si estoy en el borde superior del strip
            noroeste = superior[oj];
            norte = superior[j];
            noreste = superior[ej];
            suroeste = strip[i+1][oj];
            sureste = strip[i+1][ej];
            sur =  strip[i+1][j];
        } else if(i == filas-1){ // si estoy en el borde inferior
            noroeste = strip[i-1][oj];
            norte = strip[i-1][j];
        	noreste = strip[i-1][ej];
            suroeste = inferior[oj];
            sur =  inferior[j];
            sureste= inferior[ej];
        } else { // si estoy en el medio
            suroeste = strip[i+1][oj];
            sur =  strip[i+1][j];
            sureste = strip[i+1][ej];
            noroeste = strip[i-1][oj];
            norte = strip[i-1][j];
            noreste = strip[i-1][ej];
        }
    } else{ //si es solo una fila en el strip
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
    
    //devuelvo
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
    /* 
    * NOTA: este codigo queda acá por si se quieren
    * cambiar las reglas en algun momento
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

// Crear fila inferior del strip
int *crear_inferior(int filas, int columnas, int strip[filas][columnas]){
	int *inferior = crear_strip(columnas);

	int i;
	for(i = 0; i<columnas ; i++){
		inferior[i] = strip[filas-1][i];
	}
	return inferior;
}

// Crear fila superior del strip
int *crear_superior(int filas, int columnas, int strip[filas][columnas]){
	int *superior = crear_strip(columnas);
  	int i;
	for(i = 0; i<columnas ; i++){
		superior[i] = strip[0][i];
	}
	return superior;
}

    
/* Devuelve una grilla una generación adelante 
*  de la que se pasa por parámetro
*/
int **generacion(int **grilla, int n){
    int **nueva_grilla = crear_grilla(n,n);

    //cantidad de elementos que recibe cada proceso
  	int cantidadElementos = (n*n)/size;
 
    //cantidad de filas que tiene el strip recibido
    int cantidadFilas = cantidadElementos/n;

    //declaramos el strip original
  	int strip[cantidadFilas][n];

    //strip de la nueva generación
    int nuevo_strip[cantidadFilas][n];

    //reparto a cada procesos
    MPI_Scatter(*grilla, cantidadElementos, MPI_INT,
                *strip, cantidadElementos ,MPI_INT ,
                0, MPI_COMM_WORLD);


    int *superior = crear_superior(cantidadFilas, n, strip);
    int *inferior = crear_inferior(cantidadFilas, n, strip);
    
    /* 
    * Le envío a mis procesos vecinos la fila superior y la inferior 
    * de cada strip para que puedan trabajar 
    */
    MPI_Send(inferior,n,MPI_INT, modulo(rank+1,size),0,MPI_COMM_WORLD);
    MPI_Send(superior,n,MPI_INT, modulo(rank-1,size),0,MPI_COMM_WORLD);

    //recibo de mis vecinos las filas superior e inferior    
    MPI_Recv(superior, n,MPI_INT, modulo(rank-1,size),0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    MPI_Recv(inferior, n,MPI_INT, modulo(rank+1,size),0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);


    //aplico las reglas a cada elemento del strip
    for(int i = 0; i < cantidadFilas;i++){
	    for (int j = 0; j < n; j++){
	        int vivo = nuevo_estado(cantidadFilas,n,i,j,strip,superior,inferior);
	        nuevo_strip[i][j] = vivo;
	    }
	}

    //reemplazo en el strip original
	for(int i = 0; i < cantidadFilas ; i++){
	    for(int j = 0; j < n ; j++){
	        strip[i][j] = nuevo_strip[i][j];
	    }
	}

    //espero a que todos hayan terminado de procesar
    MPI_Barrier(MPI_COMM_WORLD);
    
    //reuno el trabajo en la grilla
    MPI_Gather(strip, cantidadElementos, MPI_INT, 
        *nueva_grilla, cantidadElementos, MPI_INT, 
        0, MPI_COMM_WORLD);
    
    //muestro la grilla
    if(!rank){
    	mostrar_grilla(nueva_grilla, n);
    }
    //la devuelvo
    return nueva_grilla;

}

int main(int argc, char const *argv[]){
 
    //inicializo
    MPI_Init(NULL, NULL); 

    //cantidad de generaciones   
    int pasos = 100;

    //tamaño de la grilla
    int n = atoi(argv[1]);
	
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if(size <= 1){
        printf("Ingrese valor de procesos mayor a 1\n");
        exit(1);
    }
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //creamos grilla
    int **grilla = crear_grilla(n,n);
    if(rank == 0){
    	if(argumentosInconsistentes(n,size)){
    		printf("Argumentos Inconsistentes\n");
    		exit(1);
    	}
        inicializar_grilla(grilla,n);

        /* ESCENARIOS INICIALES - descomentar el elegido */
        //glider(grilla);
        //palito(grilla);
        //pulsar(grilla);
        //random_init(grilla, n);
        rpentomino(grilla);
    }
    
    while(pasos){
        grilla = generacion(grilla, n);
        pasos--;
    }   
    

    
    MPI_Finalize();
    return 0;
} 