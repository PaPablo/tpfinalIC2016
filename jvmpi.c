#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <unistd.h>

#define TAM 10

#define clear() printf("\033[H\033[J")

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
            grilla[i][j] = rand() % 2;
        }
    }
    return 0;

}

int mostrar_grilla(int **grilla, int n){
    int i,j;
    
    usleep(60000);
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

int cantidad_vecinos(int *strip, int *superior, int *inferior, int n, int i, int j){

    int oj = modulo(j-1,n);
    int ej = modulo(j+1,n);
    
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
    
    return cant;
}


int nuevo_estado(int *strip, int *superior, int *inferior, int n, int i, int j, int viejo_valor){
    

    int vecinos = cantidad_vecinos(strip,superior,inferior,n,i,j);

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

void mostrar_strip(int *strip, int n) {
    for(int j = 0; j < n; j++){
        printf("%d ",strip[j]);
    }
    printf("\n");
    return;
}

int **generacion(int **grilla, int n){
    int **nueva_grilla = crear_grilla(n);

    int *strip = crear_strip(TAM);
    int *nuevo_strip = crear_strip(TAM);
    int *superior = crear_strip(TAM);
    int *inferior = crear_strip(TAM);
    int size,rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //reparto
    MPI_Scatter(*grilla, TAM, MPI_INT,
                strip, TAM,MPI_INT ,
                0, MPI_COMM_WORLD);
    
    //mostrar_strip(strip,TAM);
    
    MPI_Send(strip,n,MPI_INT, modulo(rank+1,n),0,MPI_COMM_WORLD);
    MPI_Send(strip,n,MPI_INT, modulo(rank-1,n),0,MPI_COMM_WORLD);
    
    MPI_Recv(superior, TAM,MPI_INT, modulo(rank-1,n),0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    MPI_Recv(inferior, TAM,MPI_INT, modulo(rank+1,n),0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

    for (int j = 0; j < TAM; j++){
        int vivo = nuevo_estado(strip,superior,inferior,TAM,rank, j, strip[j]);
        nuevo_strip[j] = vivo;
    }

    for(int j = 0; j < TAM ; j++){
        strip[j] = nuevo_strip[j];
    }

    MPI_Gather(strip, TAM, MPI_INT, 
        *nueva_grilla, TAM, MPI_INT, 
        0, MPI_COMM_WORLD);
  
    MPI_Barrier(MPI_COMM_WORLD);

    return nueva_grilla;

}
int main(){
   
    MPI_Init(NULL, NULL);    

    int pasos = 100;
    int n = 25;
    int **grilla = crear_grilla(TAM);
	int size,rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if(size < 1){
        printf("Ingrese valor de procesos mayor a 1\n");
        exit(1);
    }
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(rank == 0){
        //palito(grilla);
        glider(grilla);
        //inicializar_grilla(grilla,TAM);
        //mostrar_grilla(grilla, TAM);
    }
    
    while(pasos){
        grilla = generacion(grilla, TAM);
        pasos--;
        
    }   
    

    
    MPI_Finalize();
    return 0;
}