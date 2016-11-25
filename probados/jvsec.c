#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

#include<omp.h>


#define MAX_GRILLA 5
#define clear() printf("\033[H\033[J")


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
    
    // coordenadas de los vecinos
    // tienen el modulo para no salirnos del arreglo
    ni = modulo((i-1), n);
    si = modulo((i+1), n);
    oj = modulo((j-1), n);
    ej = modulo((j+1), n);

    //sumamos
    int cant = 
    grilla[ni][j]  + //vecino norte
    grilla[ni][ej] + //vecino noreste
    grilla[i][ej]  + //vecino este
    grilla[si][ej] + //vecino sureste
    grilla[si][j]  + //vecino sur
    grilla[si][oj] + //vecino suroeste
    grilla[i][oj]  + //vecino oeste
    grilla[ni][oj];  //vecino noroeste
    
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

int glider(int **grilla){

    grilla[1][3] = 1;
    grilla[1][4] = 1;
    grilla[1][5] = 1;
    grilla[2][3] = 1;
    grilla[3][4] = 1;

    return 0;
}

int main(){
    int n = 25;
    int pasos = 100;

    int i,j;
    //grilla con el estado actual
    int **grilla = crear_grilla(n);

    //nuevo estado de la grilla
    int **nueva_grilla = crear_grilla(n);

    glider(grilla);

    //inicializar_grilla(grilla, n);
    while(pasos){

        for(i = 0; i < n; i++) {
            for (j = 0; j < n ; j++) {
                nueva_grilla[i][j] = nuevo_estado(grilla,n,i,j, grilla[i][j]);
            }
        }

        for(i = 0; i < n; i++) {
            for (j = 0; j < n ; j++) {
                grilla[i][j] = nueva_grilla[i][j];
            }
        }
        mostrar_arreglo(grilla, n);
        pasos--;
    }
    
    return 0;
}