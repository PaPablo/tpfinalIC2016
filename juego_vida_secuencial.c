#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

#define MAX_GRILLA 5



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
    
    return (int **)grilla; 
}
void zeroit(int **array, int nrows, int ncolumns)
    {
    int i, j;
    for(i = 0; i < nrows; i++)
        {
        for(j = 0; j < ncolumns; j++)
            array[i][j] = 0;
        }
    }

int mostrar_arreglo(int grilla[MAX_GRILLA][MAX_GRILLA]){
    int i,j;
    
    for(i = 0;i<MAX_GRILLA;i++){
        for(j = 0; j<MAX_GRILLA;j++){
            printf("%d ",grilla[i][j]);
        }
        printf("\n");
    }
    return 0;
}

int cantidad_vecinos(int grilla[MAX_GRILLA][MAX_GRILLA], int i, int j){
    int ni,si;
    int ej,oj;
    
    // coordenadas de los vecinos
    // tienen el modulo para no salirnos del arreglo
    ni = ((i-1) % MAX_GRILLA);
    si = ((i+1) % MAX_GRILLA);
    oj = ((j-1) % MAX_GRILLA);
    ej = ((j+1) % MAX_GRILLA);

    printf("%d %d %d\n", i, j,grilla[si][j]);

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

int nuevo_estado(int grilla[MAX_GRILLA][MAX_GRILLA], int i, int j, int viejo_valor){
    
    int vecinos = cantidad_vecinos(grilla, i,j);
    
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

void cambio(int *n){
    (*n) = (*n) + 1;
}

int main(){
    int n = 10;
    int i,j;
    //grilla con el estado actual
    int **grilla = crear_grilla(n);

    //nuevo estado de la grilla
    int **nueva_grilla = crear_grilla(n);

    
    mostrar_arreglo(grilla);
    while(1){
        for(i = 0; i < MAX_GRILLA; i++) {

            for (j = 0; j < MAX_GRILLA ; j++) {
                nueva_grilla[i][j] = nuevo_estado(grilla, i,j, grilla[i][j]);
            }
        }


        for(i = 0; i < MAX_GRILLA; i++) {
            for (j = 0; j < MAX_GRILLA ; j++) {
                grilla[i][j] = nueva_grilla[i][j];
            }
        }

        printf("\n");
        mostrar_arreglo(grilla);
        usleep(500000);
    }
    
    return 0;
}