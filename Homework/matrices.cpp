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

    // The user has to pick the operation
    int opType = 0;
    while(true) {
        printf("Welcome to matrix algebra. What would you like to do?\n");
        printf("1) A + B\n");
        printf("2) A * B\n");
        printf("3) A - B\n");
        printf("4) B - A\n");   
        printf("5) A/B\n");
        printf("6) B/A\n");
        printf("7) Exit\n");
        printf(">");
        
        scanf("%d", &opType);

        switch(opType) {
        case 1: //add
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

            break;
        case 2: //multiply
            if(aColumns != bRows) {    
                printf("Cannot multiply Matrices\n");
                return 22;
            } else {
                cRows = aRows;
                cColumns = bColumns;
            }
            
            for(int i = 0; i <cRows; i++){
                for(int j = 0; j <cColumns; j++) {
                    matrixC[i][j] = 0;
                    for(int k = 0; k <aColumns; k++) {
                        matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
                    }
                }
            }


            printf("Matrix C Values:\n");
            for(int i = 0; i <cRows; i++){
                for(int j = 0; j <cColumns; j++) {
                    printf("%f  ", matrixC[i][j]);
                }
                printf("\n");
            }

            break;
        case 3: //substract
            if((aRows != bRows) || (aColumns != bColumns)) {
                printf("Cannot subtract Matrices of different sizes\n");
                return 22;
            } else {
                cRows = aRows;
                cColumns = aColumns;
            }
            
            for(int i = 0; i <cRows; i++){
                for(int j = 0; j <cColumns; j++) {
                    matrixC[i][j] = matrixA[i][j] - matrixB[i][j];
                }
            }


            printf("Matrix C Values:\n");
            for(int i = 0; i <cRows; i++){
                for(int j = 0; j <cColumns; j++) {
                    printf("%f  ", matrixC[i][j]);
                }
                printf("\n");
            }


            break;
        case 4:
            if((aRows != bRows) || (aColumns != bColumns)) {
                printf("Cannot subtract Matrices of different sizes\n");
                return 22;
            } else {
                cRows = aRows;
                cColumns = aColumns;
            }
            
            for(int i = 0; i <cRows; i++){
                for(int j = 0; j <cColumns; j++) {
                    matrixC[i][j] = matrixB[i][j] - matrixA[i][j];
                }
            }


            printf("Matrix C Values:\n");
            for(int i = 0; i <cRows; i++){
                for(int j = 0; j <cColumns; j++) {
                    printf("%f  ", matrixC[i][j]);
                }
                printf("\n");
            }


            break;
        case 5:

            break;
        case 6:

            break;
        case 7:
            printf("Goodbye!\n");
            return 22;
        default:    
            printf("Invalid Entry! Try Again!!\n");
        }
    }

    
}
