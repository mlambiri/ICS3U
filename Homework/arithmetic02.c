/****************************
 * By: Michael Lambiri       *
 * Class: ICS3U              *
 * Teacher: Ms.Cullum        *
 * Float Division Assignment *
 *****************************/
//Basic stuff.
#include<stdio.h>
#include<math.h>
#include <stdlib.h>
#include <time.h>

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
        return 33;
    }
    if(veryAble == 0) {
        printf("Cannot divide by Zero. Please pick another number\n");
        return 22;
    }

    printf("Number = %d\n", numBer);
    numBer++;
    printf("Number = %d\n", numBer);
    numBer +=1;
    printf("Number = %d\n", numBer);
    numBer = numBer +1;
    printf("Number = %d\n", numBer);

    printf("Abs(numBer) = %d\n", abs(numBer));

    //float a = 23;
    //float b = 12;
    float c = 3;

    printf("quad x1 = %f\n", (-numBer+sqrt(numBer*numBer-4*veryAble*c))/(2*veryAble));
    printf("quad x2 = %f\n", (-numBer-sqrt(numBer*numBer-4*veryAble*c))/(2*veryAble));

    srand(time(0));
    for (int i = 0; i < 12; ++i) {
        printf("Rand = %d\n", rand()%11-5);
    }

    //DIVIDING TIME! Isn't math amazing?
    int integerDivision = numBer / veryAble;
    int integerDvision = numBer % veryAble;
    float floatDivision = (float) numBer/veryAble;

    //Results
    printf("Integer Division result %d remainder %d\n", integerDivision, integerDvision);
    printf("\nFloat mode. Check this number %.1f\n", floatDivision);

}
