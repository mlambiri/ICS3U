/*******************************************************************************
 *  Name: Michael Lambiri             Course ICS3U          Date Oct 1 2019    *
 *                                                                             *
 *  Instructor Ms. Lindsay Cullum                                              *
 *                                                                             *
 *   Sticks Pick Up Assignment                                                 *
 ******************************************************************************/

#include<stdio.h>
#include<time.h>
#include<math.h>
#include<stdlib.h>
#include<string.h>

int main () {

    int stickNumber;
    int stickTaker;
    char startGame;
    char answer[80];
    int  minSticks = 10;
    bool pickOrder = false;
    bool playerOneTurn = false;
    bool correctPick = false;
    bool playerOneHuman = false;
    bool playerTwoHuman = false;
    int  playerOneScore = 0;
    int  playerTwoScore = 0;
    int  gameType = 0;

    srand(time(0));


    /*
     * @author   mlambiri
     * @date     Oct 1, 2019
     *  The user has to first pick what type of game he wants
     */
    printf("Welcome to pick up sticks! Wanna play?\n");
    printf("Pick one of the following 4 game types:\n");
    printf("1) Human against Human\n");
    printf("2) Human against Smart Computer\n");
    printf("3) Dumb Computer against Human\n");
    printf("4) Dumb Computer against Smart Computer\n");
    printf(">");

    scanf("%d", &gameType);


    switch(gameType) {
    case 1:
        playerOneHuman = true;
        playerTwoHuman = true;
        break;
    case 2:
        playerOneHuman = true;
        playerTwoHuman = false;
        break;
    case 3:
        playerOneHuman = false;
        playerTwoHuman = true;
        break;
    case 4:
        playerOneHuman = false;
        playerTwoHuman = false;
        break;
    default:
        printf("Invalid Entry! Goodbye!!\n");
        return 22;
    }
    /*
     * @author   mlambiri
     * @date     Oct 1, 2019
     *  I want to play until the user decides that he has had enough
     */

    while (true) {
        printf("Enter y to start or n to quit. > ");

        /*
         * @author   mlambiri
         * @date     Oct 2, 2019
         *   According to https://stackoverflow.com/questions/5240789/scanf-leaves-the-new-line-char-in-the-buffer
         *   I need to leave a space in front of %c because %c does not remove the newline
         *   character
         */

        scanf(" %c", &startGame);
        if (startGame == 'y') {

            pickOrder = false;
            playerOneTurn = false;
            /*
             * @author   mlambiri
             * @date     Oct 1, 2019
             *  Use a coin toss to figure out who starts the game
             *
             */

            playerOneTurn = rand()%2;

            if(playerOneTurn == true) {
                printf("Player One has the first turn.\n");
            }
            else {
                printf("Player Two has the first turn.\n");
            }

            /*
             * @author   mlambiri
             * @date     Oct 1, 2019
             *  We are picking between minSticks to 2*minSticks
             *  minSticks is set to 10. Thus we are picking
             *  at random between 10 to 20 sticks.
             */

            stickNumber = rand() % (minSticks+1) + minSticks;
            printf("\n\n LET THE BATTLE BEGIN!!!\n");

            /*
             * @author   mlambiri
             * @date     Oct 1, 2019
             *  This is the game loop
             *  Every turn either the human or the computer pick 1 to 3 sticks
             *  I check that the human picks only the allowed amount
             *  After each turn I switch the 'humanTurn' variable
             *  to allow the opponent to play
             *  The game ends when ???
             */

            while(true) {
                printf("There are %d sticks remaining\n", stickNumber);
                if(playerOneTurn == true) {
                    /*
                     * @author   mlambiri
                     * @date     Oct 1, 2019
                     *  Once the human has taken a turn it is the computer's turn
                     */
                    playerOneTurn = false;
                    if(playerOneHuman == true) {
                        correctPick = false;
                        while(correctPick == false) {
                            printf("[Player One] Pick some sticks (1-3)! >");
                            scanf("%d", &stickTaker);
                            if((stickTaker >=1) && (stickTaker <= 3)) {
                                correctPick = true;
                            }
                            else {
                                printf("Invalid number Player One! Try again!\n");
                            }
                        }

                    }
                    else {

                        /*
                         * @author   mlambiri
                         * @date     Oct 1, 2019
                         *  Computer picks at random 1 to 3 sticks
                         */
                        stickTaker = rand()%3+1;

                    }
                    if(stickNumber <= stickTaker) {
                        /*
                         * @author   mlambiri
                         * @date     Oct 1, 2019
                         *  The human has picked the last stick. He loses!!
                         */
                        printf("************************************\n");
                        printf("Player One picked the last stick! Player Two wins!!\n\n");
                        printf("************************************\n");
                        playerTwoScore++;
                        break;
                    }
                    else {
                        printf("Player One has picked %d sticks!\n", stickTaker);
                        stickNumber -= stickTaker;
                    }

                }else {
                    /*
                     * @author   mlambiri
                     * @date     Oct 1, 2019
                     *  Once the computer has taken a turn it is the human's turn
                     */
                    playerOneTurn = true;
                    if(playerTwoHuman == true) {
                        correctPick = false;
                        while(correctPick == false) {
                            printf("[Player Two] Pick some sticks (1-3)! >");
                            scanf("%d", &stickTaker);
                            if((stickTaker >=1) && (stickTaker <= 3)) {
                                correctPick = true;
                            }
                            else {
                                printf("Invalid number Player Two! Try again!\n");
                            }
                        }

                    }
                    else {

                        /*
                         * @author   mlambiri
                         * @date     Oct 1, 2019
                         *  To improve the algorithm the computer can check
                         *  how many sticks there are left
                         *  If there are 2 left then pick 1
                         *  If there are 3 left then pick 2
                         *  If there are 4 left then pick 3
                         *  If there are 5 left then pick 1
                         *  If there are 6 left computer loses
                         *  If there are 7 left pick 1
                         *  If there are 8 left pick pick 2
                         *  If there are 9 left pick 3
                         *  If there are 10 left pick 1
                         *  If there are 11 left pick 2
                         *  If there are 12 left pick 3
                         *  If there are 13 left pick 1
                         *  If there are 14 left pick 2
                         *  If there are 15 left pick 3
                         *  If there are 16 left pick 1
                         *  If there are 17 left pick 2
                         *  If there are 18 left pick 3
                         *  If there are 19 left pick 1
                         *  If there are 20 left pick 2
                         *  else pick at random between 1 to 3
                         */
                        switch (stickNumber) {
                        case 2:
                            stickTaker = 1;
                            break;
                        case 3:
                            stickTaker = 2;
                            break;
                        case 4:
                            stickTaker = 3;
                            break;
                        case 5:
                            stickTaker = 1;
                            break;
                        case 7:
                            stickTaker = 1;
                            break;
                        case 8:
                            stickTaker = 2;
                            break;
                        case 9:
                            stickTaker = 3;
                            break;
                        case 10:
                            stickTaker = 1;
                            break;
                        case 11:
                            stickTaker = 2;
                            break;
                        case 12:
                            stickTaker = 3;
                            break;
                        case 13:
                            stickTaker = 1;
                            break;
                        case 14:
                            stickTaker = 2;
                            break;
                        case 15:
                            stickTaker = 3;
                            break;
                        case 16:
                            stickTaker = 1;
                            break;
                        case 17:
                            stickTaker = 2;
                            break;
                        case 18:
                            stickTaker = 3;
                            break;
                        case 19:
                            stickTaker = 1;
                            break;
                        case 20:
                            stickTaker = 2;
                            break;
                        default:
                            stickTaker = rand()%3+1;
                        }
                    }
                    if(stickNumber <= stickTaker) {
                        /*
                         * @author   mlambiri
                         * @date     Oct 1, 2019
                         *  The human has picked the last stick. He loses!!
                         */
                        printf("************************************\n");
                        printf("Player Two has picked the last %d sticks! Player One wins!!\n\n", stickNumber);
                        printf("************************************\n");
                        playerOneScore++;
                        break;
                    }
                    else {
                        printf("Player Two has picked %d sticks!\n", stickTaker);
                        stickNumber -= stickTaker;
                    }
                }
            }
        }
        else if (startGame == 'n') {
            printf("\n\nOkay then. Have a nice day!\n");
            break;
        } else {
            printf("\nI don't understand what you want!\n");
        }
        printf("Score *** Player One: %d          Player Two: %d\n\n", playerOneScore, playerTwoScore);
    }

}




