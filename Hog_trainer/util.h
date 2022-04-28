//
// Created by LaobaBurger on 6/15/2021.
//

#include <iostream>
#include <windows.h>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <conio.h>


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

void bar(int nom, int denom,int color1,int color2)
{
    if(denom==0)denom+=1;
    if(nom<=0)nom=0;
    cout<<"                             ";//29
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),color1);
    for(int i=1;i<=(nom*40/denom+1)/2;i++)cout<<" ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),color2);
    for(int i=(nom*40/denom+1)/2+1;i<=20;i++)cout<<" ";
    cout<<"\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";
}

void GoToXY(int column, int line) {
    // Create a COORD structure and fill in its members.
    // This specifies the new position of the cursor that we will set.
    COORD coord;
    coord.X = column;
    coord.Y = line;

    // Obtain a handle to the console screen buffer.
    // (You're just using the standard console, so you can use STD_OUTPUT_HANDLE
    // in conjunction with the GetStdHandle() to retrieve the handle.)
    // Note that because it is a standard handle, we don't need to close it.
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Finally, call the SetConsoleCursorPosition function.
    if (!SetConsoleCursorPosition(hConsole, coord))
    {
        // Uh-oh! The function call failed, so you need to handle the error.
        // You can call GetLastError() to get a more specific error code.
        // ...
    }
}

const unsigned long long PI[] = {3141592653, 5897932384, 6264338327, 9502884197, 1693993751,
                                 582097494, 4592307816, 4062862089, 9862803482, 5342117067, 9000000000};

int piDigitAt(int i) {
    auto divider = (long long)pow(10, (i / 10 + 1) * 10 - i - 1);
    return (int)(PI[i / 10] / divider) % 10;
}