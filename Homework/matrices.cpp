#include <stdio.h>
#define MAXSIZE 5

int main() {
    FILE *file;
    file = fopen("matrixIn.txt","r");
    float matrixA[MAXSIZE][MAXSIZE];
    float matrixB[MAXSIZE][MAXSIZE];
    float matrixC[MAXSIZE][MAXSIZE];
    int aRows = 0, aColumns = 0, bRows = 0, bColumns = 0, cRows =0, cColumns = 0; 

    // step 1
    if(file == NULL) {
        printf("Cannot open matrix file\n");
        return 22;
    }

    fscanf(file,"%d %d", &aRows, &aColumns);
    if(aRows <= 0 || aRows > MAXSIZE) {
        printf("Invalid no rows matrix A\n");
        return 22;
    }
    if(aColumns <= 0 || aColumns > MAXSIZE) {
        printf("Invalid no columns Matrix A\n");
        return 22;
    }
    
    for(int i = 0; i <aRows; i++){
        for(int j = 0; j <aColumns; j++) {
            fscanf(file, "%f", &matrixA[i][j]);
        } 
    }

    fscanf(file,"%d %d", &bRows, &bColumns);
    if(bRows <= 0 || bRows > MAXSIZE) {
        printf("Invalid no rows matrix B\n");
        return 22;
    }
    if(bColumns <= 0 || bColumns > MAXSIZE) {
        printf("Invalid no columns Matrix B\n");
        return 22;
    }
    
    for(int i = 0; i <bRows; i++){
        for(int j = 0; j <bColumns; j++) {
            fscanf(file, "%f", &matrixB[i][j]);
        } 
    }

    printf("Matrix A Values:\n");
    for(int i = 0; i <aRows; i++){
        for(int j = 0; j <aColumns; j++) {
            printf("%f  ", matrixA[i][j]);
        }
        printf("\n");
    }
    printf("Matrix B Values:\n");
    for(int i = 0; i <bRows; i++){
        for(int j = 0; j <bColumns; j++) {
            printf("%f  ", matrixB[i][j]);
        }
        printf("\n");
    }

    //step2

    if((aRows != bRows) || (aColumns != bColumns)) {
        printf("Cannot add, Matrices of different sizes\n");
        return 22;
    } else {
        cRows = aRows;
        cColumns = aColumns;
    }
    
    for(int i = 0; i <cRows; i++){
        for(int j = 0; j <cColumns; j++) {
            matrixC[i][j] = matrixA[i][j] + matrixB[i][j];
        }
    }


    printf("Matrix C Values:\n");
    for(int i = 0; i <cRows; i++){
        for(int j = 0; j <cColumns; j++) {
            printf("%f  ", matrixC[i][j]);
        }
        printf("\n");
    }

    
}
