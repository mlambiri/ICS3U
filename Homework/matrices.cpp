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
/*Note that the first two numbers in the text file tell the computer
how many numbers to read into the Matrix*/

#include <stdio.h>
#define MAXSIZE 5

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
    printf("Matrix A Values:\n");
    for(int i = 0; i <aRows; i++){
        for(int j = 0; j <aColumns; j++) {
            printf("%3.2f  ", matrixA[i][j]);
        }
        printf("\n");
    }
    //Showing the Values stored in Matrix B
    printf("Matrix B Values:\n");
    for(int i = 0; i <bRows; i++){
        for(int j = 0; j <bColumns; j++) {
            printf("%3.2f  ", matrixB[i][j]);
        }
        printf("\n");
    }

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

            fprintf(file2, "\nOutput %d\n", numberOfUses);

            //Showing the Values stored in Matrix A
            printf("Matrix A Values:\n");
            for(int i = 0; i <aRows; i++){
                for(int j = 0; j <aColumns; j++) {
                    printf("%3.2f  ", matrixA[i][j]);
                    fprintf(file2, "%3.2f  ", matrixA[i][j]);
                }
                printf("\n");
                fprintf(file2, "\n");
            }

            printf("\n+\n\n");
            fprintf(file2, "\n+\n\n");

            //Showing the Values stored in Matrix B
            printf("Matrix B Values:\n");
            for(int i = 0; i <bRows; i++){
                for(int j = 0; j <bColumns; j++) {
                    printf("%3.2f  ", matrixB[i][j]);
                    fprintf(file2, "%3.2f  ", matrixB[i][j]);
                }
                printf("\n");
                fprintf(file2, "\n");
            }

            printf("\n=\n\n");
            fprintf(file2, "\n=\n\n");

            //Printing Output Values
            printf("Matrix C Values:\n");
            for(int i = 0; i <cRows; i++){
                for(int j = 0; j <cColumns; j++) {
                    printf("%3.2f  ", matrixC[i][j]);

                    //Saving Output Values to text file
                    fprintf(file2, "%3.2f ", matrixC[i][j]);

                }
                printf("\n");
                fprintf(file2, "\n");
            }

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

            fprintf(file2, "\nOutput %d\n", numberOfUses);

            //Showing the Values stored in Matrix A
            printf("Matrix A Values:\n");
            for(int i = 0; i <aRows; i++){
                for(int j = 0; j <aColumns; j++) {
                    printf("%3.2f  ", matrixA[i][j]);
                    fprintf(file2, "%3.2f  ", matrixA[i][j]);
                }
                printf("\n");
                fprintf(file2, "\n");
            }

            printf("\nx\n\n");
            fprintf(file2, "\nx\n\n");

            //Showing the Values stored in Matrix B
            printf("Matrix B Values:\n");
            for(int i = 0; i <bRows; i++){
                for(int j = 0; j <bColumns; j++) {
                    printf("%3.2f  ", matrixB[i][j]);
                    fprintf(file2, "%3.2f  ", matrixB[i][j]);
                }
                printf("\n");
                fprintf(file2, "\n");
            }
            printf("\n=\n\n");
            fprintf(file2, "\n=\n\n");

            //Printing Output Values
            printf("Matrix C Values:\n");
            for(int i = 0; i <cRows; i++){
                for(int j = 0; j <cColumns; j++) {
                    printf("%3.2f  ", matrixC[i][j]);

                    //Saving Output Values to text file
                    fprintf(file2, "%3.2f ", matrixC[i][j]);

                }
                printf("\n");
                fprintf(file2, "\n");
            }
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

            fprintf(file2, "\nOutput %d\n", numberOfUses);

            //Showing the Values stored in Matrix B
            printf("Matrix B Values:\n");
            for(int i = 0; i <bRows; i++){
                for(int j = 0; j <bColumns; j++) {
                    printf("%3.2f  ", matrixB[i][j]);
                    fprintf(file2, "%3.2f  ", matrixB[i][j]);
                }
                printf("\n");
                fprintf(file2, "\n");
            }

            printf("\nx\n\n");
            fprintf(file2, "\nx\n\n");

            //Showing the Values stored in Matrix A
            printf("Matrix A Values:\n");
            for(int i = 0; i <aRows; i++){
                for(int j = 0; j <aColumns; j++) {
                    printf("%3.2f  ", matrixA[i][j]);
                    fprintf(file2, "%3.2f  ", matrixA[i][j]);
                }
                printf("\n");
                fprintf(file2, "\n");
            }

            printf("\n=\n\n");
            fprintf(file2, "\n=\n\n");

            //Printing Output Values
            printf("Matrix C Values:\n");
            for(int i = 0; i <cRows; i++){
                for(int j = 0; j <cColumns; j++) {
                    printf("%3.2f  ", matrixC[i][j]);

                    //Saving Output Values to text file
                    fprintf(file2, "%3.2f ", matrixC[i][j]);

                }
                printf("\n");
                fprintf(file2, "\n");
            }

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

            fprintf(file2, "\nOutput %d\n", numberOfUses);

            //Showing the Values stored in Matrix A
            printf("Matrix A Values:\n");
            for(int i = 0; i <aRows; i++){
                for(int j = 0; j <aColumns; j++) {
                    printf("%3.2f  ", matrixA[i][j]);
                    fprintf(file2, "%3.2f  ", matrixA[i][j]);
                }
                printf("\n");
                fprintf(file2, "\n");
            }
            printf("\n-\n\n");
            fprintf(file2, "\n-\n\n");

            //Showing the Values stored in Matrix B
            printf("Matrix B Values:\n");
            for(int i = 0; i <bRows; i++){
                for(int j = 0; j <bColumns; j++) {
                    printf("%3.2f  ", matrixB[i][j]);
                    fprintf(file2, "%3.2f  ", matrixB[i][j]);
                }
                printf("\n");
                fprintf(file2, "\n");
            }
            printf("\n=\n\n");
            fprintf(file2, "\n=\n\n");

            //Printing Output Values
            printf("Matrix C Values:\n");
            for(int i = 0; i <cRows; i++){
                for(int j = 0; j <cColumns; j++) {
                    printf("%3.2f  ", matrixC[i][j]);

                    //Saving Output Values to text file
                    fprintf(file2, "%3.2f ", matrixC[i][j]);

                }
                printf("\n");
                fprintf(file2, "\n");
            }

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

            fprintf(file2, "\nOutput %d\n", numberOfUses);

            //Showing the Values stored in Matrix B
            printf("Matrix B Values:\n");
            for(int i = 0; i <bRows; i++){
                for(int j = 0; j <bColumns; j++) {
                    printf("%3.2f  ", matrixB[i][j]);
                    fprintf(file2, "%3.2f  ", matrixB[i][j]);
                }
                printf("\n");
                fprintf(file2, "\n");
            }

            printf("\n-\n\n");
            fprintf(file2, "\n-\n\n");

            //Showing the Values stored in Matrix A
            printf("Matrix A Values:\n");
            for(int i = 0; i <aRows; i++){
                for(int j = 0; j <aColumns; j++) {
                    printf("%3.2f  ", matrixA[i][j]);
                    fprintf(file2, "%3.2f  ", matrixA[i][j]);
                }
                printf("\n");
                fprintf(file2, "\n");
            }

            printf("\n=\n\n");
            fprintf(file2, "\n=\n\n");

            printf("Matrix C Values:\n");
            for(int i = 0; i <cRows; i++){
                for(int j = 0; j <cColumns; j++) {
                    printf("%3.2f  ", matrixC[i][j]);

                    //Saving Output Values to text file
                    fprintf(file2, "%3.2f ", matrixC[i][j]);

                }
                printf("\n");
                fprintf(file2, "\n");
            }

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

            fprintf(file2, "\nOutput %d\n", numberOfUses);

            //Showing the Values stored in Matrix A
            printf("Matrix A Values:\n");
            for(int i = 0; i <aRows; i++){
                for(int j = 0; j <aColumns; j++) {
                    printf("%3.2f  ", matrixA[i][j]);
                    fprintf(file2, "%3.2f  ", matrixA[i][j]);
                }
                printf("\n");
                fprintf(file2, "\n");
            }

            printf("\nTranspose\n\n");

            //Printing Output Values
            printf("Matrix C Values:\n");
            for(int i = 0; i <cRows; i++){
                for(int j = 0; j <cColumns; j++) {
                    printf("%3.2f  ", matrixC[i][j]);

                    //Saving Output Values to text file
                    fprintf(file2, "%3.2f ", matrixC[i][j]);
                }
                printf("\n");
                fprintf(file2, "\n");
            }

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

            fprintf(file2, "\nOutput %d\n", numberOfUses);

            //Showing the Values stored in Matrix B
            printf("Matrix B Values:\n");
            for(int i = 0; i <bRows; i++){
                for(int j = 0; j <bColumns; j++) {
                    printf("%3.2f  ", matrixB[i][j]);
                    fprintf(file2, "%3.2f  ", matrixB[i][j]);
                }
                printf("\n");
                fprintf(file2, "\n");
            }

            printf("\nTranspose\n\n");

            //Printing output values
            printf("Matrix C Values:\n");
            for(int i = 0; i <cRows; i++){
                for(int j = 0; j <cColumns; j++) {
                    printf("%3.2f  ", matrixC[i][j]);

                    //Saving Output Values to text file
                    fprintf(file2, "%3.2f ", matrixC[i][j]);
                }
                printf("\n");
                fprintf(file2, "\n");
            }

            break;
        case 8:
            //Showing the Values stored in Matrix A
            printf("Matrix A Values:\n");
            for(int i = 0; i <aRows; i++){
                for(int j = 0; j <aColumns; j++) {
                    printf("%3.2f  ", matrixA[i][j]);
                }
                printf("\n");
            }
            break;

        case 9:
            //Showing the Values stored in Matrix B
            printf("Matrix B Values:\n");
            for(int i = 0; i <bRows; i++){
                for(int j = 0; j <bColumns; j++) {
                    printf("%3.2f  ", matrixB[i][j]);
                }
                printf("\n");
            }
            break;

        case 10:
            //Showing the Values stored in Matrix C
            printf("Matrix C Values:\n");
            for(int i = 0; i <cRows; i++){
                for(int j = 0; j <cColumns; j++) {
                    printf("%3.2f  ", matrixC[i][j]);
                }
                printf("\n");
            }
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
