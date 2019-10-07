#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h> 

int main () {
    /*
    // Problem 1
    float numBer = 1234.5678;
    printf("%.2f", numBer);

    //Problem 2
    int secondsInput;
    printf("\nEnter a value in seconds\n");
    scanf("%d", &secondsInput);
    printf("%ds = %dh:%dm:%ds\n ", secondsInput, secondsInput/3600, (secondsInput%3600)/60, secondsInput%60);

    //Problem 3
    int beingDivided;
    int dividingInto;
    float divisionResult;
    printf("\nEnter in an integer\n");
    scanf("%d", &beingDivided);
    printf("\nEnter another integer\n");
    scanf("%d", &dividingInto);
    divisionResult = (float)beingDivided / dividingInto;
    printf("%f is the division result", divisionResult);

    //Problem 4
    int beingDivided;
    int dividingInto;
    int divisionResult;
    printf("\nEnter in an integer\n");
    scanf("%d", &beingDivided);
    printf("\nEnter another integer\n");
    scanf("%d", &dividingInto);
    divisionResult = beingDivided / dividingInto;
    printf("%d is the division result with %d remainder", divisionResult, beingDivided % dividingInto);

    */

    //Problem 5
    char firstString [31];
    char secondString [31];
    int result;
    printf("Enter in up to 30 characters");
    scanf("%s", firstString);
    printf("Enter in another up to 30 characters");
    scanf("%s", secondString);
   int strLen1 = strlen(firstString);
   int strLen2 = strlen(secondString);
   int minLen = strLen1;
    if (strLen1 > strLen2) {
        printf("%s %s", firstString, secondString);
        minLen = strLen2;
    }
    else if (strLen1 < strLen2) {
        printf("%s %s", secondString, firstString);
    }

    bool done = false;

    result = strcmp(firstString, secondString);
    
    for (int i = 0; i > minLen; i++) {
        if (result > 0) {
            printf("The alphabetical order is %s %s", firstString, secondString);
            done = true;
            break;
        }
        else if (result < 0) {
            printf("The alphabetical order is %s %s", secondString, firstString);
            done = true;
            break;
        }
    }
    if (done == false) {
        if (strLen1 == strLen2) {
            printf("Strings be the same.");
        }
    }
    else {
        printf("\nHow did this happen?");
    }

    /*
    


    //Problem 6
    for (int i = 0; i < 6; i++) {
        int thePower = pow(3, i);
        printf("3 to the power of %d is %d\n", i, thePower);
    }



    //Problem 7
    srand(time(0));
    int randomNumber;
    float bigNumber;
    float averageNumber;
    int numberOfNumbers = 10;
        for (int i = 0; i < numberOfNumbers; i++) {
            randomNumber = rand() %101;
            bigNumber += randomNumber;
            printf("\n%d", randomNumber);
        }
    averageNumber = bigNumber / numberOfNumbers;
    printf("\n%f", averageNumber);


    //Problem 8

    int numberOddEven;
    int modulusThing;
    printf("Gimme an integer guy.");
    scanf("%d", &numberOddEven);
    modulusThing = numberOddEven % 2;
    if (modulusThing == 1) {
        printf("It's an odd number buddy");
    }
    else if (modulusThing == 0) {
        printf("It's an even number fwiend");
    }
    else {
        printf("What in the name of buddy-guy did you enter?");
    }

    int divisThree;
    int modulusThing;
    printf("Gimme an integer guy.");
    scanf("%d", &divisThree);
    modulusThing = divisThree % 3;
    if (modulusThing == 0) {
        printf("It's divisible by 3 fwiend");
    }
    else {
        printf("I'm not your guy buddy!");
    }


    float realNumber;
    float squareRoot;
    printf("Gimme a real number");
    scanf("%f", &realNumber);
    squareRoot = sqrt(realNumber);
    if(realNumber < 0) {
        printf("There is no real square root for a negative number");
    }
    else {
        printf("The square root is %f", squareRoot);
    }

    //Problem 11
    float soManyNumbers;
    printf("Enter in any number");
    scanf("%f", &soManyNumbers);
    if (soManyNumbers > 4.8 && soManyNumbers < 18.2) {
        printf("accept");
    } 
    else {
        printf("reject");
    }

    //Problem 12
    int costGas;
    printf("How much gas did you just get?");
    if (costGas >= 10 && costGas <= 20) {
        printf("Your car wash will cost $3");
    }
    else {
        printf("Your car wash will cost $1");
    }

    //Problem 13
    int agePerson;
    int yearsOfSadness;
    int total;
    printf("How old are you? ");
    scanf("%d", &agePerson);
    printf("How long have you had your job? ");
    scanf("%d", &yearsOfSadness);
    total = yearsOfSadness + agePerson;
    if ((agePerson >= 65) || (total >= 85)) {
        printf("You're retired.\n");
    }
    else {
        printf("Get out of my office nerd\n");
    }

    //Problem 14
    int monthNumber;

    printf("Enter in the place of a month (September is 9, June is 6 etc.)");
    scanf("%d", &monthNumber);
    switch (monthNumber) {
        case 1: 
            printf("\nIt's January");
            break;
        
        case 2:
            printf("\nIt's February");
            break;
        
        case 3: 
            printf("\nIt's March");
            break;

        case 4: 
            printf("\nIt's April");
            break;

        case 5: 
            printf("\nIt's May");
            break;
        
        case 6: 
            printf("\nIt's June");
            break;
        
        case 7: 
            printf("\nIt's July");
            break;
        
        case 8: 
            printf("\nIt's August");
            break;
        
        case 9: 
            printf("\nIt's September");
            break;
        
        case 10: 
            printf("\nIt's October");
            break;

        case 11: 
            printf("\nIt's November");
            break;

        case 12: 
            printf("\nIt's December");
            break;
        
        default:
            printf("What month is that?");
            break;
    }

    int gasLitres;
    char gasType;
    float total;

    printf("What type of gas do you want? (S for Super, P for Premium, R for Regular, and D for Diesel.");
    scanf(" %c", &gasType);
    printf("\nHow many litres of gas do you want?");
    scanf("%d", &gasLitres);
    switch (gasType) {
        
        case 'S':
            total = 1.235 * gasLitres;
            printf("\n%f is the price of your gas", total);
            break;
        
        case 'P':
            total = 1.283 * gasLitres;
            printf("\n%f is the price of your gas", total);
            break;

        case 'D':
            total = 0.983 * gasLitres;
            printf("\n%f is the price of your gas", total);
            break;
        
        case 'R':
            total = 0.824 * gasLitres;
            printf("\n%f is the price of your gas", total);
            break;
        
        default:
            printf("What's the joke?");
    }

    int number;
    char letter;
    printf("Enter a letter");
    scanf(" %c", &letter);
    switch (letter) {
        case 'a':
            number = 1;
            printf("The letter would be %d", number);
            break;
        case 'b':
            number = 2;
            printf("The letter would be %d", number);
            break;
        case 'c':
            number = 3;
            printf("The letter would be %d", number);
            break;
        case 'd':
            number = 4;
            printf("The letter would be %d", number);
            break;
        case 'e':
            number = 5;
            printf("The letter would be %d", number);
            break;
        case 'f':
            number = 6;
            printf("The letter would be %d", number);
            break;
        case 'g':
            number = 7;
            printf("The letter would be %d", number);
            break;
        case 'h':
            number = 8;
            printf("The letter would be %d", number);
            break;
        case 'i':
            number = 9;
            printf("The letter would be %d", number);
            break;
        case 'j':
            number = 1;
            printf("The letter would be %d", number);
            break;
        case 'l':
            number = 2;
            printf("The letter would be %d", number);
            break;
        case 'm':
            number = 3;
            printf("The letter would be %d", number);
            break;
        case 'n':
            number = 4;
            printf("The letter would be %d", number);
            break;
        case 'o':
            number = 5;
            printf("The letter would be %d", number);
            break;
        case 'p':
            number = 6;
            printf("The letter would be %d", number);
            break;
        case 'q':
            number = 7;
            printf("The letter would be %d", number);
            break;
        case 'r':
            number = 8;
            printf("The letter would be %d", number);
            break;
        case 's':
            number = 9;
            printf("The letter would be %d", number);
            break;
        case 't':
            number = 1;
            printf("The letter would be %d", number);
            break;
        case 'u':
            number = 2;
            printf("The letter would be %d", number);
            break;
        case 'v':
            number = 3;
            printf("The letter would be %d", number);
            break;
        case 'w':
            number = 4;
            printf("The letter would be %d", number);
            break;
        case 'x':
            number = 5;
            printf("The letter would be %d", number);
            break;
        case 'y':
            number = 6;
            printf("The letter would be %d", number);
            break;
        case 'z':
            number = 7;
            printf("The letter would be %d", number);
            break;
        case 'k':
            number = 8;
            printf("The letter would be %d", number);
            break;
    }

    float squareRooter;
    for (int i = 0; i < 21; i++) {
       printf("%d   ", i); 
       squareRooter = sqrt(i);
       printf("%f\n", squareRooter);
    }

    for (int i = 10; i < 21; i+=2) {
        printf("%d\n", i);
    }

    int coolInteger[9];
    int coolMin = 400000000;
    int coolMax = -400000000;
    int coolRange;
    for (int i = 0; i < 9; i++) {
        printf("Input an integer");
        scanf("%d", &(coolInteger[i]));
        if (coolInteger[i] > coolMax) {
            coolMax = coolInteger[i];
        }
        if(coolInteger[i] < coolMin) {
            coolMin = coolInteger[i];
        }
    }
    coolRange = coolMax - coolMin;
    printf("The range is %d", coolRange);

    int imUnoriginal = 3;
    while (imUnoriginal%2 > 0) {
        printf("\n\nEnter in an even integer");
        scanf("%d", &imUnoriginal);
        if (imUnoriginal % 2 > 0) {
            printf("\n\n That's an odd number.");
        }
    }

    */

}
