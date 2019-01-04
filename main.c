#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef struct {
    int n_neu;
    float umbral[5], pesos[5][5];
} CAPA;

typedef struct {
    int n_capas, n_ent;
    float entrada[5], salida[5];
    CAPA capa[5];
} PERCEPTRON;

PERCEPTRON cargaperc(PERCEPTRON perc, FILE *fich){
    int i, j, k;
    fscanf(fich,"%d %d", &perc.n_capas, &perc.n_ent);
    printf("Numero de capas:    %d\n", perc.n_capas);
    printf("Numero de entradas: %d\n", perc.n_ent);

    for(k=0;k<perc.n_capas;k++){
        printf("\n");
        fscanf(fich,"%d", &perc.capa[k].n_neu);
        printf("CARGANDO CAPA %d\n",k);
        printf(" Numero de entradas: %d\n", perc.n_ent);
        printf(" Numero de neuronas: %d\n", perc.capa[k].n_neu);
        printf(" Matriz de pesos:\n");
        for(j=0;j<perc.n_ent;j++){
            for(i=0;i<perc.capa[k].n_neu;i++){
                fscanf(fich,"%f", &perc.capa[k].pesos[j][i]);
                printf("W[%d][%d] : %f\n", j, i, perc.capa[k].pesos[j][i]);
            }

        }
        printf("\nUmbrales\n");
        for(j=0;j<perc.capa[k].n_neu;j++){
            fscanf(fich,"%f",&perc.capa[k].umbral[j]);
            printf(" Umbral[%d] : %f\n", j, perc.capa[k].umbral[j]);
        }
        printf("\n");

    }

    return(perc);

}

float f(float x){ // f(x)=1/(1+e^-20x)
    return(1/(1+expf(-20*x)));
}



int cuentaentradas(FILE *fich){
    int contador=0;
    char linea[26];

    while(fgets(linea,26,fich))
          contador++;

    return(contador);

}


PERCEPTRON proceso(PERCEPTRON perc, int c){
    int i, j, k;
    float suma;
    if(!c)
        for(j=0;j<perc.n_ent;j++)
            perc.salida[j] = perc.entrada[j];

    //printf("CAPA %d :\n", c);
    //printf("=========\n");
    //printf("MULTIPLICACION POR LA MATRIZ DE PESOS :\n");
    for(i=0;i<perc.capa[c].n_neu;i++){ // La i controla los elementos del vector
        for(j=0;j<perc.n_ent;j++){ // La j controla las columnas de la matriz de peso
            for(k=0, suma=0;k<perc.capa[c].n_neu;k++) {// La k controla las filas de la matriz de pesos
                suma += perc.salida[i] * perc.capa[c].pesos[j][k];
            }
        }
        perc.salida[i] = suma;
        //printf("SALIDA [%d]: %f\n", i, perc.salida[i]);
    }

    // SALIDA = SALIDA - UMBRAL
    //printf("RESTA DEL VECTOR DE UMBRALES : \n");
    for(j=0;j<perc.capa[c].n_neu;j++){
        perc.salida[j] = perc.salida[j] - perc.capa[c].umbral[j];
        //printf("VECTOR[%d] : %f . SALIDA[%d] : %f\n", j, perc.capa[c].umbral[j],j,perc.salida[j]);
    }
    //printf("APLICACION DE LA FUNCION f(x)\n");

    for(j=0;j<perc.capa[c].n_neu;j++){
        perc.salida[j] = 1/(1+expf((-20)*perc.salida[j]));
        //printf("SALIDA[%d] = %f\n", j, perc.salida[j]);
    }
    for(k=0;k<5;k++)
        perc.entrada[k] = perc.salida[k];
    return(perc);




}


int main(){
    PERCEPTRON perc;
    FILE *fich;
    int i, j, k, nent;
    if(!(fich=fopen("configuracion.txt","rb"))){
        printf("ERROR EN LA APERTURA DEL FICHERO. (1)\n");
        return(1);
    }

    printf("CARGA DEL PERCEPTRON\n");
    printf("====================\n");
    perc=cargaperc(perc, fich);
    fclose(fich);
    if(!(fich=fopen("entrada.txt","rb"))){
        printf("ERROR EN LA LECTURA DEL FICHERO DE ENTRADAS. (2)");
        return(2);
    }
    nent=cuentaentradas(fich);
    rewind(fich);


    printf("EJECUCION DEL PERCEPTRON\n");
    printf("========================\n");

    for(i=0;i<nent;i++){ // PARA CADA ENTRADA DEL FICHERO...
        for(j=0;j<perc.n_capas;j++){ // PARA CADA CAPA...
            if(!j){ // SI J == 0 - > ESTAMOS EN LA CAPA 0 - > HAY QUE LEER UNA ENTRADA DEL FICHERO
                printf("ENTRADA : (");
                for(k=0;k<perc.n_ent-1;k++){
                    fscanf(fich,"%f", &perc.entrada[k]);
                    printf("%f, ", perc.entrada[k]);
                }
                fscanf(fich,"%f", &perc.entrada[k]);
                printf("%f)\t - > \t", perc.entrada[k]);
            }
            //printf("\n");
            perc=proceso(perc, j);
            if(j==perc.n_capas-1){
                printf("SALIDA : (");
                for(k=0;k<perc.capa[j].n_neu-1;k++)
                    printf("%f, ", perc.salida[k]);
                printf("%f)\n", perc.salida[k]);
            }



        }
    }

}
