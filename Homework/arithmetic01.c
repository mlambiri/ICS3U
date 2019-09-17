/****************************
 * By: Michael Lambiri       *
 * Class: ICS3U              *
 * Teacher: Ms.Cullum        *
 * Float Division Assignment *
 *****************************/
//Basic stuff.
#include<stdio.h>
#include<math.h>

int main() {

    //Variables declared and initialized
    int numBer = 0;
    int veryAble = 0;

    //Recording numbers.
    printf("Enter a number. ");
    scanf("%d", &numBer);
    printf("Enter a non-zero number. ");
    scanf("%d", &veryAble);

    if(numBer == 0 && veryAble == 0) {
        printf("The result can be anything you want ... Say 42? \n");
    }
    if(veryAble == 0) {
        printf("Cannot divide by Zero. Please pick another number\n");
        return 22;
    }

    //DIVIDING TIME! Isn't math amazing?
    int integerDivision = numBer / veryAble;
    int integerDvision = numBer % veryAble;
    float floatDivision = (float) numBer/veryAble;

    //Results
    printf("Integer Division result %d remainder %d", integerDivision, integerDvision);
    printf("\nFloat mode. Check this number %.1f\n", floatDivision);

}
