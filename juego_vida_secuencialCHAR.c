#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

#define MAX_GRILLA 5
#define clear() printf("\033[H\033[J")


char** crear_grilla(int n){
    int i,j;
    char **grilla;
    //pido memoria para filas
    grilla = malloc(n * sizeof(char*));

    //memoria para cada columna
    for(j = 0; j<n;j++){
        grilla[j] = malloc(n * sizeof(char));
    }

    //chequeo si anducharvo
    if(grilla[n-1] == NULL){
        fprintf(stderr, "malloc failed\n");
        exit(-1);
    }    
    
    return grilla; 
}

int inicializar_grilla(char **grilla, int n){
    int i,j,decido;

    srand(getpid());

    for(i = 0;i<n;i++){
        for(j = 0; j<n;j++){
            decido = rand() % 2;
            if(decido){
                grilla[i][j] = '@';
            }
            else{
                grilla[i][j] = '-';
            }
        }
    }
    return 0;

}

int mostrar_arreglo(char **grilla, int n){
    int i,j;
    
    for(i = 0;i<n;i++){
        for(j = 0; j<n;j++){
            printf("%c ",grilla[i][j]);
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

int viva(char c){
    return (c == '@');
}

int cantidad_vecinos(char **grilla, int n, int i, int j){
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
    viva(grilla[ni][j])  + //vecino norte
    viva(grilla[ni][ej]) + //vecino noreste
    viva(grilla[i][ej])  + //vecino este
    viva(grilla[si][ej]) + //vecino sureste
    viva(grilla[si][j])  + //vecino sur
    viva(grilla[si][oj]) + //vecino suroeste
    viva(grilla[i][oj])  + //vecino oeste
    viva(grilla[ni][oj]);  //vecino noroeste
    
    //devolvemos la cantidad
    return cant;
}


char nuevo_estado(char **grilla, int n, int i, int j, char viejo_valor){
    
    int vecinos = cantidad_vecinos(grilla,n,i,j);
    
    // Para morir de inanicion
    // En este caso, menos de dos vecinos vivos
    if (vecinos < 2){
        return '-';
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
        return '@';
    }
    // Para matar una celda de sobre poblacion
    // En este caso mas de 3 vecinos vivos
    else if(vecinos > 3){
        return '-';
    }
    
}

int main(){
    int n = 20;

    int i,j;
    printf("grilla\n");
    //grilla con el estado actual
    char **grilla = crear_grilla(n);

    printf("nueva grilla\n");
    //nuevo estado de la grilla
    char **nueva_grilla = crear_grilla(n);

    /*Glider*/
    grilla[7][3] = '@';
    grilla[7][4] = '@';
    grilla[7][5] = '@';
    grilla[8][5] = '@';
    grilla[9][4] = '@';

    //inicializar_grilla(grilla, n);
    int pasos = 200;
    mostrar_arreglo(grilla,n);
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
        pasos--;
        printf("\n");
        mostrar_arreglo(grilla,n);
        usleep(100000);
        clear();

    }
    
    return 0;
}