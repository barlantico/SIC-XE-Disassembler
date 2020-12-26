/**
* Brian Arlantico, cssc3010, 821125494
* Nicholas Dollick, cssc3046, 815970305
* CS530 Fall 2020
* Assignment 2, Disassembler for XE computer
* functions.h
*/

#include <sys/types.h>
#include <dirent.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <sstream> 
#include <iomanip>
#include <cctype>
#include <algorithm>

void searchDirectory(vector<string> &files);
int getFiles(vector<string> &files, ifstream &symfile, ifstream &objfile, string file);
void disassemble(ifstream &objfile, ifstream &symfile, string file);
const char* hex_char_to_bin(char c);
void getMnemonic(string stropCode, struct opCodeStruct &opCode);
string bin_string_to_hex(string bin);
const char* hex_char_to_bin(char c);
void readobjFile(ifstream &objfile, vector<string> &objVector);
void readsymFile(ifstream &symfile, struct symbolTable &symTabStruct);
void startDirective(struct objCodeStruct &objStruct, ofstream &sicFile, ofstream &lisFile);
void readObjCode(struct objCodeStruct &objStruct, struct opCodeStruct &opCode, int index);
void printFormat3(struct symbolTable &symStruct, struct objCodeStruct &objStruct, struct opCodeStruct &opCode, ofstream &sicFile, ofstream &lisFile, int index);
void printFormat4(struct symbolTable symStruct, struct objCodeStruct &objStruct, struct opCodeStruct &opCode, ofstream &sicFile, ofstream &lisFile, int index);
string insertSymbol(struct symbolTable symStruct, struct objCodeStruct &objStruct);
string findOperand(struct symbolTable symStruct, int TA);
void insertDirective(struct symbolTable symStruct, struct objCodeStruct &objStruct, struct opCodeStruct &opCode, ofstream &sicFile, ofstream &lisFile, int trIndex);
void insertVars(struct objCodeStruct &objStruct, struct symbolTable symStruct, ofstream &sicFile, ofstream &lisFile, int maxAdr);
int getVarLength(struct objCodeStruct &objStruct, struct symbolTable symStruct);
void endDirective(struct objCodeStruct &objStruct, struct symbolTable symStruct, ofstream &sicFile, ofstream &lisFile);
void printFormat2(struct symbolTable symStruct, struct objCodeStruct &objStruct, struct opCodeStruct &opCode, ofstream &sicFile, ofstream &lisFile, int index);
const char* getRegister(int num);

//struct for opcode that is being processed at that address
struct opCodeStruct {
    string objCode;
    string opMnemonic;
    int n;
    int i;
    int x;
    int b;
    int p;
    int e;
    int intDisp;
    int format;
};

//struct for the obj file which keeps important information about the program
struct objCodeStruct {
    vector<string> objVector;
    int LOCCTR;
    int BASE;
    int programLength;
    int textRecordPointer;
};

//struct for symbol table that has vectors and lengths for sym/lit tables
struct symbolTable { 
    vector<string> symVector;
    vector<string> newLitTable;
    int symTabLength;
    int litTabLength;
    int newLiterals;
};
