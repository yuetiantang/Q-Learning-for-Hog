//
// Created by LaobaBurger on 6/15/2021.
//

#include <iostream>
#include <windows.h>
#include <cstring>
#include <sstream>
#include <stdlib.h>

/*
 * Require user to make a selection.
 * Example: char options[] = {'0', '1', '2', '3', '4', '5'};
        input = requireSelection(options);
 */
char requireSelection(string optionSet) {
    char input;
    while(true) {
        input = getche();
        for(int i = 0; i < optionSet.length(); i++) {
            if (input == optionSet[i]) {
                return input;
            }
        }
        cout<<"\b \b";
    }
}

/*
 * Return current time as a string.
 */
string localTime() {
    SYSTEMTIME sys;
    GetLocalTime(&sys);
    char str[17];
    sprintf(str, "%4d%02d%02d%02d%02d%02d%03d",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds);
    return str;
}

const char* str_ptr[100];
char str[100];
const char* toPath(string path) {
    int i;
    for(i = 0; i < path.length(); i++) {
        str[i] = path[i];
    }
    str[i] = '\x00';
    str_ptr[0] = &str[0];
    return str_ptr[0];
}

void displayLocalTime() {
    SYSTEMTIME sys;
    GetLocalTime(&sys);
    printf("%4d/%02d/%02d %02d:%02d:%02d.%03d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds);
}