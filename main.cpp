/**
* Brian Arlantico, cssc3010, 821125494
* Nicholas Dollick, cssc3046, 815970305
* CS530 Fall 2020
* Assignment 2, Disassembler for XE computer
* main.cpp
*/
using namespace std;
#include "functions.hpp"


int main(int argc, char **argv)
{
    if (argc != 2) {
        cout << "Usage: dis <filename>" << endl; //invalid nubmer of arguments
        return 0;
    }

    vector<string> files = vector<string>(); //string vector for files in directory

    searchDirectory(files); //call function to search directory
    //declaration of ifstream objects to open sym and obj file
    ifstream objfile;
    ifstream symfile;

    //turn argument into string
    string inFile(argv[1]);

    //function to find obj and sym files
    if (!getFiles(files, symfile, objfile, inFile))
        return 0; //exit if one or more files not found

    //calls main disassembler function
    disassemble(objfile,symfile, inFile);

    return 1;
    
}