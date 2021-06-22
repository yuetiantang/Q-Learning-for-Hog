//
// Created by LaobaBurger on 17-5-28 23:42.
//

#include <iostream>
#include <windows.h>

void UpdateSeed() {
    srand(GetCurrentTime() - rand());
}

// 0 or 1, probability of 1 is nominator/dn
bool trueOrFalse (int nominator,int denominator) {
    UpdateSeed();
    if((GetCurrentTime() - rand()) % denominator < nominator) return true;
    else return false;
}

// generate an integer within [0, limit)
int randomNumber(int limit) {
    UpdateSeed();
    return (int)((GetCurrentTime()-rand()) % limit);
}

// 1 or -1
int positiveOrNegative(int nominator,int denominator) {
    UpdateSeed();
    if((GetCurrentTime() - rand()) % denominator < nominator) return 1;
    else return -1;
}


