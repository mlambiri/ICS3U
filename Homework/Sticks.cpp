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

    bool turnPlayer;
    bool winLose;
    int stickNumber;
    int stickTaker;
    char startGame;
    char turnOrder;
    char enterAbsorber;
    int  minSticks = 10;
    bool pickOrder = false;
    bool humanTurn = false;
    bool correctPick = false;

    srand(time(0));

    /*
     * @author   mlambiri
     * @date     Oct 1, 2019
     *  I want to play until the user decides that he has had enough
     */

    while (true) {
        printf("Welcome to pick up sticks? Wanna play? Enter y to start or n to quit. > ");
        scanf("%c", &startGame);
        if (startGame == 'y') {

            pickOrder = false;
            humanTurn = false;
            /*
             * @author   mlambiri
             * @date     Oct 1, 2019
             *  The user has to pick if he wants to go first or second
             */

            while(pickOrder == false) {
                printf("\nWould you like to go first or second? f for first s for second. > ");
                scanf("%c", &turnOrder);
                switch (turnOrder) {
                case 'f':
                    humanTurn = true;
                    pickOrder = true;
                    break;
                case 's':
                    humanTurn = false;
                    pickOrder = true;
                    break;
                default:
                    printf("\nPlease pick a turn order!\n");

                }
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
                if(humanTurn == true) {
                    /*
                     * @author   mlambiri
                     * @date     Oct 1, 2019
                     *  Once the human has taken a turn it is the computer's turn
                     */
                    humanTurn = false;
                    correctPick = false;
                    while(correctPick == false) {
                        printf("Enter how many sticks you will pick up from 1-3. >");
                        scanf("%d", &stickTaker);
                        if((stickTaker >=1) && (stickTaker <= 3)) {
                            correctPick = true;
                        }
                        else {
                            printf("Invalid number! Try again!\n");
                        }
                    }
                    if(stickNumber <= stickTaker) {
                        /*
                         * @author   mlambiri
                         * @date     Oct 1, 2019
                         *  The human has picked the last stick. He loses!!
                         */
                        printf("************************************\n");
                        printf("You have picked the last stick! Computer wins!!\n\n");
                        printf("************************************\n");
                        break;
                    }
                    else {
                        stickNumber -= stickTaker;
                    }

                }else {
                    /*
                     * @author   mlambiri
                     * @date     Oct 1, 2019
                     *  Once the computer has taken a turn it is the human's turn
                     */
                    humanTurn = true;

                    /*
                     * @author   mlambiri
                     * @date     Oct 1, 2019
                     *  Computer picks at random 1 to 3 sticks
                     */
                    stickTaker = rand()%3+1;


                    if(stickNumber <= stickTaker) {
                        /*
                         * @author   mlambiri
                         * @date     Oct 1, 2019
                         *  The human has picked the last stick. He loses!!
                         */
                        printf("************************************\n");
                        printf("Computer has picked the last %d sticks! You win!!\n\n", stickNumber);
                        printf("************************************\n");
                        break;
                    }
                    else {
                        printf("Computer has picked %d sticks!\n", stickTaker);
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
    }

}




