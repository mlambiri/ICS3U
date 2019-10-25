/*******************************************************************************
 *  Name: Michael Lambiri             Course ICS3U          Date Oct 22 2019   *
 *                                                                             *
 *  Instructor Ms. Cullum                                                      *
 *                                                                             *
 *  Matrix Operations Assignment                                               *
 *                                                                             *
 *  https://www.khanacademy.org/math/precalculus/                              *
 *  x9e81a4f98389efdf:matrices/x9e81a4f98389efdf:multiplying-matrices-by-      *
 *  matrices/v/multiplying-a-matrix-by-a-matrix                                *
 *  Big thanks to Khan Academy for the help                                    *
 ******************************************************************************/

#include <stdio.h>
#define MAXSIZE 5

void printMatrix(char ch, int rows, int columns, float matrix[MAXSIZE][MAXSIZE]) {

    printf("Matrix %c Values:\n", ch);
    for(int i = 0; i <rows; i++){
        for(int j = 0; j <columns; j++) {
            printf("%f  ", matrix[i][j]);
        }
        printf("\n");
    }
}

void printMatrixToFile(FILE*file, char ch, int rows, int columns, float matrix[MAXSIZE][MAXSIZE]) {

    //Showing the Values stored in Matrix
    fprintf(file, "Matrix %c Values:\n", ch);
    for(int i = 0; i <rows; i++){
        for(int j = 0; j <columns; j++) {
            fprintf(file, "%f  ", matrix[i][j]);
        }
        fprintf(file, "\n");
    }
    fprintf(file, "\n");
}


int main() {
    //Opening File
    FILE *file = fopen("matrixIn.txt","r");

    //Set space for Matrices A,B,C
    float matrixA[MAXSIZE][MAXSIZE];
    float matrixB[MAXSIZE][MAXSIZE];
    float matrixC[MAXSIZE][MAXSIZE];
    int aRows = 0, aColumns = 0, bRows = 0, bColumns = 0, cRows =0, cColumns = 0;

    /* Step 1
     * Checking if file is open
     * If not, nothing else to do, we return
     */
    if(file == NULL) {
        printf("Cannot open matrix file\n");
        return 22;
    }

    //Reading rows and column sizes of Matrix A
    fscanf(file,"%d %d", &aRows, &aColumns);

    //If the user specifies negative row size, or row size is greater than 5, program will return
    if(aRows <= 0 || aRows > MAXSIZE) {
        printf("Invalid number of rows matrix A\n");
        return 22;
    }
    //Same idea, but for columns of Matrix A
    if(aColumns <= 0 || aColumns > MAXSIZE) {
        printf("Invalid number of columns Matrix A\n");
        return 22;
    }

    //Reading in the values of Matrix A
    for(int i = 0; i <aRows; i++){
        for(int j = 0; j <aColumns; j++) {
            fscanf(file, "%f", &matrixA[i][j]);
        }
    }

    //Reading rows and column sizes of Matrix B
    fscanf(file,"%d %d", &bRows, &bColumns);

    //If the user specifies negative row size, or row size is greater than 5, program will return
    if(bRows <= 0 || bRows > MAXSIZE) {
        printf("Invalid number of rows matrix B\n");
        return 22;
    }
    //Again, same thing but Matrix B's columns
    if(bColumns <= 0 || bColumns > MAXSIZE) {
        printf("Invalid number of columns Matrix B\n");
        return 22;
    }

    //Reading in values of Matrix B
    for(int i = 0; i <bRows; i++){
        for(int j = 0; j <bColumns; j++) {
            fscanf(file, "%f", &matrixB[i][j]);
        }
    }

    //Showing the Values stored in Matrix A

    printMatrix('A', aRows, aColumns, matrixA);

    //Showing the Values stored in Matrix B
    printMatrix('B', bRows, bColumns, matrixB);

    // The user has to pick the operation
    int opType = 0;

    //Opening file to write output values
    fclose(file);
    FILE *file2 = fopen("matrixOut.txt","w");
    int numberOfUses = 0;

    //Program loops until user decides to exit
    while(true) {
        printf("Welcome to matrix algebra. What would you like to do?\n");
        printf("1) A + B\n");
        printf("2) A * B\n");
        printf("3) A * B\n");
        printf("4) A - B\n");
        printf("5) B - A\n");
        printf("6) Transpose(A)\n");
        printf("7) Transpose(B)\n");
        printf("8) print(A)\n");
        printf("9) print(B)\n");
        printf("10) print(C)\n");
        printf("20) Exit\n");
        printf(">");

        //Reading in user input
        scanf("%d", &opType);

        switch(opType) {

        case 1: //Addition

            if((aRows != bRows) || (aColumns != bColumns)) {
                printf("Cannot add, Matrices of different sizes\n");
                return 22;
            } else {
                cRows = aRows;
                cColumns = aColumns;
            }

            numberOfUses++;
            for(int i = 0; i <cRows; i++){
                for(int j = 0; j <cColumns; j++) {
                    matrixC[i][j] = matrixA[i][j] + matrixB[i][j];
                }
            }
            //Showing the Values stored in Matrix A
            printMatrix('A', aRows, aColumns, matrixA);
            printMatrixToFile(file2, 'A', aRows, aColumns, matrixA);
            printf("\n+\n\n");
            fprintf(file2, "\n+\n\n");
            //Showing the Values stored in Matrix B
            printMatrix('B', bRows, bColumns, matrixB);
            printMatrixToFile(file2, 'B', bRows, bColumns, matrixB);
            printf("\n");
            fprintf(file2, "\n");
            printf("\n=\n\n");
            fprintf(file2, "\n=\n\n");
            //Printing Output Values
            printMatrix('C', cRows, cColumns, matrixC);
            printMatrixToFile(file2, 'C', cRows, cColumns, matrixC);

            break;

        case 2: //Multiplication AxB
            if(aColumns != bRows) {
                printf("Cannot multiply Matrices\n");
                return 22;
            } else {
                cRows = aRows;
                cColumns = bColumns;
            }

            numberOfUses++;
            for(int i = 0; i <cRows; i++){
                for(int j = 0; j <cColumns; j++) {
                    matrixC[i][j] = 0;
                    for(int k = 0; k <aColumns; k++) {
                        matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
                    }
                }
            }
            //Showing the Values stored in Matrix A
            printMatrix('A', aRows, aColumns, matrixA);
            printMatrixToFile(file2, 'A', aRows, aColumns, matrixA);

            printf("\nx\n\n");
            fprintf(file2, "\nx\n\n");

            //Showing the Values stored in Matrix B
            printMatrix('B', bRows, bColumns, matrixB);
            printMatrixToFile(file2, 'B', bRows, bColumns, matrixB);

            printf("\n=\n\n");
            fprintf(file2, "\n=\n\n");

            //Printing Output Values
            printMatrix('C', cRows, cColumns, matrixC);
            printMatrixToFile(file2, 'C', cRows, cColumns, matrixC);

            break;

        case 3: //Multiplication BxA
            if(aColumns != bRows) {
                printf("Cannot multiply Matrices\n");
                return 22;
            } else {
                cRows = aRows;
                cColumns = bColumns;
            }

            numberOfUses++;
            for(int i = 0; i <cRows; i++){
                for(int j = 0; j <cColumns; j++) {
                    matrixC[i][j] = 0;
                    for(int k = 0; k <aColumns; k++) {
                        matrixC[i][j] += matrixB[i][k] * matrixA[k][j];
                    }
                }
            }
            //Showing the Values stored in Matrix B
            printMatrix('B', bRows, bColumns, matrixB);
            printMatrixToFile(file2, 'B', bRows, bColumns, matrixB);

            printf("\nx\n\n");
            fprintf(file2, "\nx\n\n");

            //Showing the Values stored in Matrix A
            printMatrix('A', aRows, aColumns, matrixA);
            printMatrixToFile(file2, 'A', aRows, aColumns, matrixA);

            printf("\n=\n\n");
            fprintf(file2, "\n=\n\n");

            //Printing Output Values
            printMatrix('C', cRows, cColumns, matrixC);
            printMatrixToFile(file2, 'C', cRows, cColumns, matrixC);

            break;
        case 4: //A - B substraction
            if((aRows != bRows) || (aColumns != bColumns)) {
                printf("Cannot subtract Matrices of different sizes\n");
                return 22;
            } else {
                cRows = aRows;
                cColumns = aColumns;
            }

            numberOfUses++;
            for(int i = 0; i <cRows; i++){
                for(int j = 0; j <cColumns; j++) {
                    matrixC[i][j] = matrixA[i][j] - matrixB[i][j];
                }
            }
            //Showing the Values stored in Matrix A
            printMatrix('A', aRows, aColumns, matrixA);
            printMatrixToFile(file2, 'A', aRows, aColumns, matrixA);

            printf("\n-\n\n");
            fprintf(file2, "\n-\n\n");

            //Showing the Values stored in Matrix B
            printMatrix('B', bRows, bColumns, matrixB);
            printMatrixToFile(file2, 'B', bRows, bColumns, matrixB);

            printf("\n=\n\n");
            fprintf(file2, "\n=\n\n");

            //Printing Output Values
            printMatrix('C', cRows, cColumns, matrixC);
            printMatrixToFile(file2, 'C', cRows, cColumns, matrixC);

            break;
        case 5: //B - A substraction
            if((aRows != bRows) || (aColumns != bColumns)) {
                printf("Cannot subtract Matrices of different sizes\n");
                return 22;
            } else {
                cRows = aRows;
                cColumns = aColumns;
            }

            numberOfUses++;
            for(int i = 0; i <cRows; i++){
                for(int j = 0; j <cColumns; j++) {
                    matrixC[i][j] = matrixB[i][j] - matrixA[i][j];
                }
            }

            //Showing the Values stored in Matrix B
            printMatrix('B', bRows, bColumns, matrixB);
            printMatrixToFile(file2, 'B', bRows, bColumns, matrixB);

            printf("\n-\n\n");
            fprintf(file2, "\n-\n\n");

            //Showing the Values stored in Matrix A
            printMatrix('A', aRows, aColumns, matrixA);
            printMatrixToFile(file2, 'A', aRows, aColumns, matrixA);

            printf("\n=\n\n");
            fprintf(file2, "\n=\n\n");

            printMatrix('C', cRows, cColumns, matrixC);
            printMatrixToFile(file2, 'C', cRows, cColumns, matrixC);

            break;

        case 6: //transpose(A)
            cRows = aColumns;
            cColumns = aRows;

            numberOfUses++;
            for(int i = 0; i <cRows; i++){
                for(int j = 0; j <cColumns; j++) {
                    matrixC[i][j] = matrixA[j][i];
                }
            }
            //Showing the Values stored in Matrix A
            printMatrix('A', aRows, aColumns, matrixA);
            printMatrixToFile(file2, 'A', aRows, aColumns, matrixA);

            printf("\nTranspose\n\n");
            fprintf(file2, "\nTranspose\n\n");

            //Printing Output Values
            printMatrix('C', cRows, cColumns, matrixC);
            printMatrixToFile(file2, 'C', cRows, cColumns, matrixC);

            break;

        case 7: //transpose(B)
            cRows = bColumns;
            cColumns = bRows;

            numberOfUses++;
            for(int i = 0; i <cRows; i++){
                for(int j = 0; j <cColumns; j++) {
                    matrixC[i][j] = matrixB[j][i];
                }
            }

            printMatrix('B', bRows, bColumns, matrixB);
            printMatrixToFile(file2, 'B', bRows, bColumns, matrixB);

            printf("\nTranspose\n\n");
            fprintf(file2, "\nTranspose\n\n");

            printMatrix('C', cRows, cColumns, matrixC);
            printMatrixToFile(file2, 'C', cRows, cColumns, matrixC);

            break;
        case 8:
            //Showing the Values stored in Matrix A
            printMatrix('A', aRows, aColumns, matrixA);
            break;

        case 9:
            //Showing the Values stored in Matrix B
            printMatrix('B', bRows, bColumns, matrixB);
            break;

        case 10:
            //Showing the Values stored in Matrix C
            printMatrix('C', cRows, cColumns, matrixC);
            break;
        case 20: //If they decide to exit
            printf("Goodbye!\n");
            fclose(file2);
            return 22;

        default: //If entry does not match any of the requested inputs
            printf("Invalid Entry! Try Again!!\n");
        }
    }

    return 0;
}
