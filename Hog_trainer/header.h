//
// Created by LaobaBurger on 16-12-01 17:59.
//

#include<iostream>
#include<windows.h>
#include<string>
using namespace std;

/*
 * Prints a string, formatted by a SCTA identifier
 * Arguments:
 * id: SCTA identifier
 * s: string to be printed
 */
void scta(int id, const string& s) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), id);
    cout<<s;
}

/*
 * header() prints a banner at top of the window.
 * Arguments:
 * title: program name
 * ver: program version
 */

void header(const string& title, const string& ver)
{
    int shift = 21;
    cout<<"\n";
    scta(15," ");
    scta(19696," ");
    for(int i = 0; i < title.length() + shift; i++) {
        scta(1039," ");
    }
    scta(22000," \n");
    scta(15," ");
    scta(2063," ");
    scta(15,"   ");
    scta(15, title);  //program name
    scta(15,"   By: ");
    scta(12,"L");scta(10,"a");scta(11,"o");scta(3,"b");scta(10,"a");
    scta(6,"Burger");
    scta(4111," ");

    scta(15,"   ");
    scta(35855," ");
    scta(34063,"Ver.");
    scta(37903," ");
    scta(34063," ");
    scta(34063, ver);
    scta(37903," \n");//ver.

    scta(15," ");
    scta(34831," ");
    for(int i = 0; i < title.length() + shift; i++) {
        scta(32783," ");
    }
    scta(36879," ");
    scta(15,"\n");
    scta(32783,"                                                                                ");
    scta(15,"\n");
}