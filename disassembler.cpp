/**
* Brian Arlantico, cssc3010, 821125494
* Nicholas Dollick, cssc3046, 815970305
* CS530 Fall 2020
* Assignment 2, Disassembler for XE computer
* disassembler.cpp
*/
using namespace std;
#include "functions.hpp"

/**
 * @brief Main driver for the disassembling process
 * @param objfile is the object file provided by user
 * @param symfile is the symbol file provided by user
 * @param file is the name of the file provided by user
 * @return void
*/
void disassemble(ifstream &objfile, ifstream &symfile, string file) {
    int textRecordAdr = 0; //address of current text record
    int index = 0; //index of the objfile

    string sicFileName = file.append(".sic");
    file = file.substr(0, file.find(".", 0)); //remove appended sic
    string lisFileName = file.append(".lis"); 

    //declaring structs 
    struct objCodeStruct objStruct;
    struct symbolTable symStruct;

    //counter for new literals
    symStruct.newLiterals = 0;

    //reads both sym and obj files and closes after gathering infoi
    readobjFile(objfile, objStruct.objVector);
    readsymFile(symfile, symStruct);
    objfile.close();
    symfile.close();

    //declare both output files
    ofstream sicFile(sicFileName.c_str());
    ofstream lisFile(lisFileName.c_str());
    struct opCodeStruct opCode; //opCodeStruct for flags, format, etc


    index = 1; //index starts at 1 for first text record
    while (objStruct.objVector[index][0] == 'T') {
        string trTemp = ""; //text record temp string
        trTemp.assign(objStruct.objVector[index],1,6);
        textRecordAdr = (int)strtol(trTemp.c_str(),NULL,16); //starting address of text record as int
        objStruct.LOCCTR = textRecordAdr; //LOCCTR starts where that address of text record starts 
        trTemp.assign(objStruct.objVector[index],7,2); 
        int trLength = (int)strtol(trTemp.c_str(),NULL,16); //save text record length to know when to stop
        objStruct.textRecordPointer = 9; //start of first instruction

    
        if (index == 1)
            startDirective(objStruct, sicFile, lisFile); //insert start directive in both output files
        
    
        while (objStruct.LOCCTR < textRecordAdr + trLength) {
            readObjCode(objStruct, opCode, index); //analyze the current instruction

            //increment accordingly based on format
            if (opCode.format == 3) {
                printFormat3(symStruct, objStruct, opCode, sicFile, lisFile, index);
                objStruct.LOCCTR += 3;
                objStruct.textRecordPointer += 3;
            }

            if (opCode.format == 4) {
                printFormat4(symStruct, objStruct, opCode, sicFile, lisFile, index);
                objStruct.LOCCTR += 4;
                objStruct.textRecordPointer += 5;
            }

            if (opCode.format == 2) {
                printFormat2(symStruct, objStruct, opCode, sicFile, lisFile, index);
                objStruct.LOCCTR += 2;
                objStruct.textRecordPointer += 1;

            }

            //inserts directives as needed (BASE, LTORG)
            insertDirective(symStruct, objStruct, opCode, sicFile, lisFile, index); 
        
            
        } 
        //next row of objfile
        index++;

        //checks if there are variables in between text records
        if (objStruct.objVector[index][0] == 'T') {
        trTemp.assign(objStruct.objVector[index],1,6);
        textRecordAdr = (int)strtol(trTemp.c_str(),NULL,16);
        if(textRecordAdr != objStruct.LOCCTR) { 
            insertVars(objStruct, symStruct, sicFile, lisFile, textRecordAdr);
        }
        }
    }
    
    //inserts final variables and completes with the end directive 
    insertVars(objStruct, symStruct, sicFile, lisFile, objStruct.programLength);
    endDirective(objStruct, symStruct, sicFile, lisFile);

    sicFile.close();
    lisFile.close();
}

/**
 * @brief Inserts the END directive to finalize the sic/lis files
 * @param objStruct struct that holds BASE, LOCCTR, programLength 
 * @param symStruct struct that holds symbol/lit table info
 * @param sicFile output file for the sic file
 * @param lisFile output file for the lis file
 * @return void
*/
void endDirective(struct objCodeStruct &objStruct, struct symbolTable symStruct, ofstream & sicFile, ofstream &lisFile) {
    string temp = "";
    string end = "END";
    stringstream ss;
    int index = 0;
    int adr = 0;
    string endSymbol = "";
    string lineBuf(63, ' ');
    string lisLineBuf(63, ' ');

    ss << setw(4) << setfill('0') << hex << objStruct.LOCCTR; //LOCCTR as a string
    temp = ss.str();

    //address insertion for lis file
    for (int i = 0; i < temp.length(); i++)
        lisLineBuf[i] = toupper(temp[i]);

    temp = "";

    while (objStruct.objVector[index][0] != 'E') //disregarding mod records
        index++;

    //finds the symbol that end directive has as address
    temp.assign(objStruct.objVector[index],1,6);
    adr = (int)strtol(temp.c_str(), NULL, 16);
    endSymbol = findOperand(symStruct, adr);

    //insert end and symbol at apporpriate places in files
    for (int i = 0; i < end.length(); i++) {
        lineBuf[i+9] = end[i];
        lisLineBuf[i+15] = end[i];
    }

    for (int i = 0; i < endSymbol.length(); i++) {
        lineBuf[i+17] = endSymbol[i];
        lisLineBuf[i+23] = endSymbol[i];
    }
    
    //null terminate and write to files
    lineBuf[lineBuf.length()] = '\0';
    lisLineBuf[lisLineBuf.length()] = '\0';
    sicFile << '\n' << lineBuf;
    lisFile << '\n' << lisLineBuf;


}

/**
 * @brief Inserts vars (RESW/RESB) in between text records or last text record and end of prog length
 * @param objStruct struct that holds BASE, LOCCTR, programLength 
 * @param symStruct struct that holds symbol/lit table info
 * @param sicFile output file for the sic file
 * @param lisFile output file for the lis file
 * @param maxAdr max address that the algorithm should go up to as an integer
 * @return void
*/
void insertVars(struct objCodeStruct &objStruct, struct symbolTable symStruct, ofstream &sicFile, ofstream &lisFile, int maxAdr) {
    string lineBuf(63, ' ');
    string lisLineBuf(63, ' ');
    string tp = "";
    string symbol = "";
    string temp;
    int lengthTemp = 0;
    stringstream ss;
    int varLength = 0;

    //constant resb/resw keywords
    string resb = "RESB";
    string resw = "RESW";

    //loop for when the address of end of program/next text record is reached
    while (objStruct.LOCCTR < maxAdr) { 
        //get symbol and length of variables
        symbol = insertSymbol(symStruct, objStruct);      
        varLength = getVarLength(objStruct, symStruct);

        ss.str(string());
        ss.clear();

        ss << setw(4) << setfill('0') << hex << objStruct.LOCCTR; //string locctr for lis file
        tp = ss.str();

        //for loops to insert at appropriate places in files
        for (int i = 0; i < 4; i++) 
            lisLineBuf[i] = toupper(tp[i]);

        for (int i = 0; i < symbol.length(); i++) {
            lineBuf[i] = symbol[i];
            lisLineBuf[i+6] = symbol[i];
        }

        //distinguish between RESB and RESW (since word = 3 bytes)
        if (varLength % 3 == 0) {
        for (int i = 0; i < resw.length(); i++) {
            lineBuf[i+9] = resw[i];
            lisLineBuf[i+15] = resw[i];
        }

            lengthTemp = varLength/3;
        }
        else {
            //prints resb
        for (int i = 0; i < resb.length(); i++) {
            lineBuf[i+9] = resb[i];
            lisLineBuf[i+15] = resb[i];
        }

            lengthTemp = varLength;
        }

        ss.str(string());
        ss.clear();
        ss << dec << lengthTemp;
        temp = ss.str();
        
        //length printed after resb/resw
        for (int i = 0; i < temp.length(); i++) {
            lineBuf[i+17] = temp[i];
            lisLineBuf[i+23] = temp[i];
        }
        
        //adds length of variable to LOCCTR
        objStruct.LOCCTR += varLength;

        //null terminates and prints to files
        lineBuf[lineBuf.length()] = '\0';
        lisLineBuf[lisLineBuf.length()] = '\0';

        sicFile << '\n' << lineBuf;
        lisFile << '\n' << lisLineBuf;
        
        //clears the string buffers
        for (int i = 0; i < lineBuf.length(); i++) {
            lineBuf[i] = ' ';
            lisLineBuf[i] = ' ';
        }
    }

}

/**
 * @brief Retrieves length of variable in bytes
 * @param objStruct struct that holds BASE, LOCCTR, programLength 
 * @param symStruct struct that holds symbol/lit table info
 * @return integer of length of variable
*/
int getVarLength(struct objCodeStruct &objStruct, struct symbolTable symStruct) {
    int varLength = 0;
    int intTemp = 0;
    stringstream ss;
    string strAdr = "";
    string temp = "";
    int index = 0;

    //gets the LOCCTR as a string for searching the sym/lit tab
    ss << hex << setw(6) << setfill('0') << objStruct.LOCCTR;
    strAdr = ss.str();

    transform(strAdr.begin(),strAdr.end(),strAdr.begin(), ::toupper);

    //search the sym/lit table
    for (index = 0; index < symStruct.symTabLength; index++) {
        temp.assign(symStruct.symVector[index],8,6);
        if (strAdr == temp)
            break;
    }

    //gets length as a symbol
    if (index != symStruct.symTabLength - 1) {
        temp.assign(symStruct.symVector[index],8,6);
        intTemp = (int)strtol(temp.c_str(),NULL,16);
        temp.assign(symStruct.symVector[index+1],8,6);
        varLength = (int)strtol(temp.c_str(),NULL,16);
        varLength -= intTemp;
    }

    //gets length as a literal
    else {
        temp.assign(symStruct.symVector[index],8,6);
        intTemp = (int)strtol(temp.c_str(),NULL,16);
        varLength = objStruct.programLength - intTemp;
    }
    
    return varLength;
}

/**
 * @brief Inserts info from objFile into vector
 * @param objfile input file provided by user
 * @param objVector vector containing lines from objfile
 * @return void
*/
void readobjFile(ifstream &objfile, vector<string> &objVector) {
    string temp = "";

    while(getline(objfile,temp))
        objVector.push_back(temp);

}

/**
 * @brief Inserts info from objFile into vector
 * @param symfile input file provided by user
 * @param symTabStruct struct containing info from symtab
 * @return void
*/
void readsymFile(ifstream &symfile, struct symbolTable &symTabStruct) {
    string temp = "";

    for (int i = 0; i < 3; i++)
        getline(symfile,temp);
    
    while (temp != "Name    Literal  Length Address:") //implies all sym files will be the same format
    {
        symTabStruct.symVector.push_back(temp);
        getline(symfile,temp);
    }

    getline(symfile,temp); 

    while(getline(symfile,temp)) {
        if (temp.length() >= 30) //ignores newlines/spaces
        symTabStruct.symVector.push_back(temp);
    }

    int i = 0;
    while (symTabStruct.symVector[i] != "\0")  
        i++;

    symTabStruct.symTabLength = i;
    symTabStruct.litTabLength = symTabStruct.symVector.size() - i - 1;
}

/**
 * @brief Begins the disassembling process with the START directive
 * @param objStruct struct that holds BASE, LOCCTR, programLength 
 * @param sicFile output file for sic file
 * @param lisFile output file for lis file
 * @return void
*/
void startDirective(struct objCodeStruct &objStruct, ofstream &sicFile, ofstream &lisFile) { 
    string startAdr;
    stringstream ss;
    int startAddress = 0;
    string start = "START";
    string tp = "";
    string lineBuf(63, ' ');
    string lisLineBuf(63, ' ');

    //prints LOCCTR for listing file 
    ss << setw(4) << setfill('0') << hex << objStruct.LOCCTR; 
    tp = ss.str();

    for (int i = 0; i < 4; i++) {
        lisLineBuf[i] = tp[i];
    }

    //prints start directive line in correct places of both files
    for (int i = 0; i < 6; i++) {
        lineBuf[i] = objStruct.objVector[0][i+1];
        lisLineBuf[i+6] = objStruct.objVector[0][i+1];
    }

    for (int i = 0; i < start.length(); i ++) {
        lineBuf[i+9] = start[i];
        lisLineBuf[i+15] = start[i];
    }

    //gets address of header file for use in start directive
    startAdr.assign(objStruct.objVector[0],7,6);
    startAddress = (int)strtol(startAdr.c_str(),NULL,16);
    stringstream adr;

    if (startAddress == 0)
        adr << hex << startAddress;
    else
        adr << setw(4) << setfill('0') << hex << startAddress;


    startAdr = adr.str();

    for (int i = 0; i < startAdr.length(); i++) {
        lineBuf[i+17] = toupper(startAdr[i]);
        lisLineBuf[i+23] = toupper(startAdr[i]);
    }

    //save program length
    tp.assign(objStruct.objVector[0], 15,4);
    objStruct.programLength = (int)strtol(tp.c_str(),NULL,16);

    //null terminate strings and write to both files
    lineBuf[lineBuf.length()] = '\0';
    lisLineBuf[lisLineBuf.length()] = '\0';

    sicFile << lineBuf;
    lisFile << lisLineBuf;
}

/**
 * @brief dissects important information of objCode
 * @param objStruct struct that holds BASE, LOCCTR, programLength 
 * @param opCode struct that holds necessary info of a certain objCode (flags, format, etc)
 * @param index line of the current objCode
 * @return void
*/
void readObjCode(struct objCodeStruct &objStruct, struct opCodeStruct &opCode, int index) {
    string temp = "";
    string opBinary = "";
    int tempTextRecordPointer = 0;

    //extracts first byte of obj code
    temp.assign(objStruct.objVector[index], objStruct.textRecordPointer, 2);
    tempTextRecordPointer = objStruct.textRecordPointer;

    //converts hex to binary string
    for (int j = 0; j < 2; j++)
        opBinary.append(hex_char_to_bin(temp[j]));

    //calculates n and i bits
    opCode.n = opBinary[6] - '0';
    opCode.i = opBinary[7] - '0';

    opBinary[6] = '0';
    opBinary[7] = '0';

    //retrieve mnemonic of obj code
    temp = bin_string_to_hex(opBinary);
    getMnemonic(temp, opCode);

    if (opCode.format != 2) {
        string xbpe = "";
        string xbpeBin = "";
        objStruct.textRecordPointer += 2;
        xbpe.assign(objStruct.objVector[index], objStruct.textRecordPointer, 1);

        //calculates xbpe flags
        xbpeBin = hex_char_to_bin(xbpe[0]);

        opCode.x = xbpeBin[0] - '0';
        opCode.b = xbpeBin[1] - '0';
        opCode.p = xbpeBin[2] - '0';
        opCode.e = xbpeBin[3] - '0';

        //sets format flag based on e bit
        if (opCode.e == 0) { 
            opCode.format = 3;
            opCode.objCode.assign(objStruct.objVector[index], tempTextRecordPointer, 6);
        }

        if (opCode.e == 1) {
            opCode.format = 4;
            opCode.objCode.assign(objStruct.objVector[index], tempTextRecordPointer, 8);
        }
    }
    
    //format 2 instruction
    else
        opCode.objCode.assign(objStruct.objVector[index], tempTextRecordPointer, 4);

    
}

/**
 * @brief prints the disassembled format 2 code onto sic/lis files
 * @param symStruct struct that holds info from symbol table
 * @param objStruct struct that holds BASE, LOCCTR, programLength 
 * @param opCode struct that holds necessary info of a certain objCode (flags, format, etc)
 * @param sicFile output file for sic file
 * @param lisFile output file for lis file
 * @param index line of the current objCode
 * @return void
*/
void printFormat2(struct symbolTable symStruct, struct objCodeStruct &objStruct, struct opCodeStruct &opCode, ofstream &sicFile, ofstream &lisFile, int index) {
    string lineBuf(63,' ');
    string lisLineBuf(63,' ');
    int PC = objStruct.LOCCTR + 2;
    string reg1 = "";
    string reg2 = "";
    string temp = "";
    string tp = "";
    int regTemp = 0;
    stringstream ss;

    //insert address in listing file
    ss << setw(4) << setfill('0') << hex << objStruct.LOCCTR;
    tp = ss.str();
    ss.str(string());
    ss.clear();

    for (int i = 0; i < 4; i++) 
        lisLineBuf[i] = toupper(tp[i]);
    
    for (int i = 0; i < opCode.objCode.length(); i++) {
        lisLineBuf[i+32] = opCode.objCode[i];
    }

    objStruct.textRecordPointer += 2;

    //Find r1 mnemonic
    temp.assign(objStruct.objVector[index], objStruct.textRecordPointer,1);
    regTemp = (int)strtol(temp.c_str(),NULL,10);
    reg1 = getRegister(regTemp);

    //find r2 mnemonic
    temp.assign(objStruct.objVector[index], ++objStruct.textRecordPointer,1);
    regTemp = (int)strtol(temp.c_str(),NULL,10);
    reg2 = getRegister(regTemp);

    //find symbol if present
    string symbol = insertSymbol(symStruct,objStruct);

    //inserts symbol and opcode mnemonic 
    for (int i = 0; i < symbol.length(); i++) {
        lineBuf[i] = symbol[i];
        lisLineBuf[i+6] = symbol[i];
    }

    for (int i = 0; i < opCode.opMnemonic.length(); i++)  {
        lineBuf[i+9] = opCode.opMnemonic[i];
        lisLineBuf[i+15] = opCode.opMnemonic[i];
    }

    //instructions that only use 1 register
    if (opCode.opMnemonic == "CLEAR" || opCode.opMnemonic == "TIXR") 
    {
        for (int i = 0; i < reg1.length(); i++)
        {
        lineBuf[17+i] = reg1[i];
        lisLineBuf[23+i] = reg1[i];
        }
    }

    else //prints the mnemonics of both registers
     {   
         for (int i = 0; i < reg1.length(); i++)
         {
            lineBuf[i+17] = reg1[i];
            lisLineBuf[i+23] = reg1[i];
         }
        lineBuf[reg1.length()+17] = ',';
        lisLineBuf[reg1.length()+23] = ',';

        for (int i = 0; i < reg2.length(); i++) {
            lineBuf[i+reg1.length()+18] = reg2[i];
            lisLineBuf[i+reg1.length()+24] = reg2[i];
        }

     }

    //null terminate the strings and print to files
    lineBuf[lineBuf.length()] = '\0';
    lisLineBuf[lisLineBuf.length()] = '\0';
    sicFile << '\n' << lineBuf;
    lisFile << '\n' << lisLineBuf;

}

/**
 * @brief prints the disassembled format 3 code onto sic/lis files
 * @param symStruct struct that holds info from symbol table
 * @param objStruct struct that holds BASE, LOCCTR, programLength 
 * @param opCode struct that holds necessary info of a certain objCode (flags, format, etc)
 * @param sicFile output file for sic file
 * @param lisFile output file for lis file
 * @param index line of the current objCode
 * @return void
*/
void printFormat3(struct symbolTable &symStruct, struct objCodeStruct &objStruct, struct opCodeStruct &opCode, ofstream &sicFile, ofstream &lisFile, int index) {
    string lineBuf(63,' ');
    string lisLineBuf(63, ' ');
    int PC = objStruct.LOCCTR + 3;
    stringstream ss;
    string strDisp;
    string tp = "";

    //print LOCCTR for listing file
    ss << setw(4) << setfill('0') << hex << objStruct.LOCCTR;
    tp = ss.str();

    ss.str(string());
    ss.clear();

    for (int i = 0; i < 4; i++) {
        lisLineBuf[i] = toupper(tp[i]);
    }

    string symbol = insertSymbol(symStruct,objStruct); //find symbol if any at that address location 

    //print symbol at appropraite place in files
    for (int i = 0; i < symbol.length(); i++) {
        lineBuf[i] = symbol[i];
        lisLineBuf[i+6] = symbol[i];
    }

    //print opcode mnemonic at appropriate place in files
    for (int i = 0; i < opCode.opMnemonic.length(); i++)  {
        lineBuf[i+9] = opCode.opMnemonic[i];
        lisLineBuf[i+15] = opCode.opMnemonic[i];
    }

    //print obj code in listing file
    for (int i = 0; i < opCode.objCode.length(); i++) {
        lisLineBuf[i+32] = opCode.objCode[i];
    }


    if (opCode.opMnemonic == "RSUB") { //rsub has no address
        lineBuf[lineBuf.length()] = '\0';
        lisLineBuf[lisLineBuf.length()] = '\0';
        sicFile << '\n' << lineBuf;
        lisFile << '\n' << lisLineBuf;
        return;
    }
        
    //extract displacement of format 3 instruction
    strDisp.assign(objStruct.objVector[index],++objStruct.textRecordPointer,3);
    opCode.intDisp = (int)strtol(strDisp.c_str(),NULL,16);

    //immediate addressing
    if (opCode.i == 1 && opCode.n == 0) {
        lineBuf[16] = '#';
        lisLineBuf[22] = '#';
    }

    //indirect addressing
    if (opCode.i == 0 && opCode.n == 1) {
        lineBuf[16] = '@';
        lisLineBuf[22] = '@';
    }
    
    //direct addressing
    if (opCode.p == 0 && opCode.b == 0) {
        ss << opCode.intDisp;
        strDisp = ss.str();
        for (int i = 0; i < strDisp.length(); i++) {
            lineBuf[i+17] = strDisp[i];
            lisLineBuf[i+23] = strDisp[i];
        }
        
    }

    //PC relative addressing
    if (opCode.p == 1) {
        string operand = "";
        int TA = 0;
        if (strDisp[0] == 'F' || strDisp[0] == 'E' ||strDisp[0] == 'D' || strDisp[0] == 'C' ||strDisp[0] == 'B' || strDisp[0] == 'A' ||strDisp[0] == '9' ||strDisp[0] == '8')
            TA = PC + (opCode.intDisp - 4096); //negative displacement
        else
            TA = PC + opCode.intDisp;

        operand = findOperand(symStruct, TA);

        //print literal with = in the first column
        if (operand[0] == '=') {
            for (int i = 0; i < operand.length(); i++)  {
                lineBuf[i+16] = operand[i];
                lisLineBuf[i+22] = operand[i];
            }

            int found = 0;

            //Algorithm to keep track of any new literals encountered until next pool
            for (int i =0; i < symStruct.newLitTable.size(); i++) {
                if (symStruct.newLitTable[i] == operand) {
                    found = 1;
                    break;
                }
            }

            if (found == 0) {
                symStruct.newLitTable.push_back(operand);
                symStruct.newLiterals += 1;
            }
        }

        else 
        {
            if (opCode.x == 1) //indexed addressing
            operand.append(",X");
        
            for (int i = 0; i < operand.length(); i++) {
                lineBuf[i+17] = operand[i];
                lisLineBuf[i+23] = operand[i];
            }
        }
    }

    //base relative addressing
    if (opCode.b == 1) {
        string operand = "";
        int TA = objStruct.BASE + opCode.intDisp;
        operand = findOperand(symStruct,TA);

        //printing literal 
        if (operand[0] == '=')
        for (int i = 0; i < operand.length(); i++) {
            lineBuf[i+16] = operand[i];
            lisLineBuf[i+22] = operand[i];
        }

    else {
        //indexed addressing
        if (opCode.x == 1)
            operand.append(",X");

        for (int i = 0; i < operand.length(); i++) {
            lineBuf[i+17] = operand[i];
            lisLineBuf[i+23] = operand[i];
        }
    }
    }

    //null terminate and write to files
    lineBuf[lineBuf.length()] = '\0';
    lisLineBuf[lisLineBuf.length()] = '\0';
    sicFile << "\n" << lineBuf;
    lisFile << '\n' << lisLineBuf;


}

/**
 * @brief prints the disassembled format 4 code onto sic/lis files
 * @param symStruct struct that holds info from symbol table
 * @param objStruct struct that holds BASE, LOCCTR, programLength 
 * @param opCode struct that holds necessary info of a certain objCode (flags, format, etc)
 * @param sicFile output file for sic file
 * @param lisFile output file for lis file
 * @param index line of the current objCode
 * @return void
*/
void printFormat4(struct symbolTable symStruct, struct objCodeStruct &objStruct, struct opCodeStruct &opCode, ofstream &sicFile, ofstream &lisFile, int index) {
    string lineBuf(63, ' ');
    string lisLineBuf(63, ' ');
    int PC = objStruct.LOCCTR + 4;
    stringstream ss;
    string strDisp;
    string tp = "";

    ss << setw(4) << setfill('0') << hex << objStruct.LOCCTR;
    tp = ss.str();

    ss.str(string());
    ss.clear();

    //printing address and object code for listing file
    for (int i = 0; i < 4; i++) {
        lisLineBuf[i] = toupper(tp[i]);
    }

    for (int i = 0; i < opCode.objCode.length(); i++) {
        lisLineBuf[i+32] = opCode.objCode[i];
    }

    //symbol insertion
    string symbol = insertSymbol(symStruct, objStruct);

    for (int i = 0; i < symbol.length(); i++) {
        lineBuf[i] = symbol[i];
        lisLineBuf[i+6] = symbol[i];
    }

    //opcode mnemonic insertion
    for (int i = 0; i < opCode.opMnemonic.length(); i++)  {
        lineBuf[i+9] = opCode.opMnemonic[i];
        lisLineBuf[i+15] = opCode.opMnemonic[i];
    }

    //direct addressing (usually the case in format 4)
    if (opCode.b == 0 && opCode.p == 0) {
        strDisp.assign(objStruct.objVector[index], ++objStruct.textRecordPointer, 5);
        opCode.intDisp = (int)strtol(strDisp.c_str(),NULL,16); //the displacement is the actual TA
        string operand = findOperand(symStruct, opCode.intDisp); 

        //print operands to files
        for (int i = 0; i < operand.length(); i++) {
            lineBuf[i+17] = operand[i];
            lisLineBuf[i+23] = operand[i];
        }
    }
    //check if immediate or indirect
    if (opCode.i == 1 && opCode.n == 0) {
        lineBuf[16] = '#';
        lisLineBuf[22] = '#';
    }

    if (opCode.i == 0 && opCode.n == 1) {
        lineBuf[16] = '@';
        lisLineBuf[22] = '@';
    }

    //extended format 
    lineBuf[8] = '+';
    lisLineBuf[14] = '+';

    //null terminate and print on files
    lineBuf[lineBuf.length()] = '\0';
    lisLineBuf[lisLineBuf.length()] = '\0';
    sicFile  << '\n' << lineBuf;
    lisFile << '\n' << lisLineBuf;


}

/**
 * @brief finds the symbol (if any) that belongs at that address
 * @param symStruct struct that holds info from symbol table
 * @param objStruct struct that holds BASE, LOCCTR, programLength 
 * @return string of symbol that belongs on that line
*/
string insertSymbol(struct symbolTable symStruct, struct objCodeStruct &objStruct) {
    stringstream ss;
    string stringFinder;
    int index;
    string symbol(6, ' ');
    string temp = "";

    //turn LOCCTR into string hex format to search table
    ss << hex << setw(6) << setfill('0') << objStruct.LOCCTR;

    stringFinder = ss.str();

    transform(stringFinder.begin(),stringFinder.end(), stringFinder.begin(), ::toupper);

    for (index = 0; index < symStruct.symTabLength; index++) {
        temp.assign(symStruct.symVector[index],8,6);

        if (stringFinder == temp) { //if the addresses are equal then symbol is there
            break;
        }
    }

    temp.assign(symStruct.symVector[index],0,6);
    
    for (int i = 0; i < temp.length(); i++) 
        symbol[i] = temp[i];

    return symbol;

}

/**
 * @brief finds the operand for that particular instruction
 * @param symStruct struct that holds info from symbol table
 * @param TA target address for the operand
 * @return string of operand
*/
string findOperand(struct symbolTable symStruct, int TA) {
    int found = 0;
    int index = 0;
    stringstream ss;
    ss << hex << setw(6) << setfill('0') << TA;
    string strAdr = ss.str();
    string temp = "";

    //Turn TA into string and search sym/lit tables
    transform(strAdr.begin(),strAdr.end(),strAdr.begin(), ::toupper);

    for (index = 0; index < symStruct.symTabLength; index++) {
        temp.assign(symStruct.symVector[index],8,6);

        if (temp == strAdr) {
            found = 1;
            break;
        }
    }

    if (found == 0) {
        for (index = symStruct.symTabLength + 1; index < symStruct.symVector.size(); index++) {
            temp.assign(symStruct.symVector[index], 24, 6);

            if (temp == strAdr) {
            found = 1;
            break;
            }   
        }
    }
    
    //found in symbol table
    if (found == 1) {
    if (index < symStruct.symTabLength) {
        temp.assign(symStruct.symVector[index],0,6);

    }

    //found in literal table
    if (index >= symStruct.symTabLength + 1)
        temp.assign(symStruct.symVector[index],8,8);
    }

    temp = temp.substr(0,temp.find(" ",0)); //remove extra spaces
    return temp;
}

/**
 * @brief finds the symbol (if any) that belongs at that address
 * @param symStruct struct that holds info from symbol table
 * @param objStruct struct that holds BASE, LOCCTR, programLength 
 * @param opCode struct that holds important info about an objcode (flags, format, etc)
 * @param sicFile output file for sic file
 * @param lisFile output file for lis file
 * @param trIndex index for text record in objStruct
 * @return void
*/
void insertDirective(struct symbolTable symStruct, struct objCodeStruct &objStruct, struct opCodeStruct &opCode, ofstream &sicFile, ofstream &lisFile, int trIndex) {
    string lineBuf(63,' ');
    string lisLineBuf(63,' ');
    string tp = "";
    stringstream ss;
    int index = 0;
    int increment = 0;
    int found = 0;
    
    //ASSUMPTION: base is used after LDB 
    if (opCode.opMnemonic == "LDB") {
        string base = "BASE";
        objStruct.BASE = opCode.intDisp;

        ss << setw(4) << setfill('0') << hex << objStruct.LOCCTR;
        tp = ss.str();

        ss.str(string());
        ss.clear();

        //print LOCCTR to listing file
        for (int i = 0; i < 4; i++) 
        lisLineBuf[i] = toupper(tp[i]);

        //print BASE directive in appropriate areas of both files
        for (int i = 0; i < base.length(); i++) {
            lineBuf[i+9] = base[i]; 
            lisLineBuf[i+15] = base[i];
        }
        
        //finds the operand from the previous instruction for BASE 
        string operand = findOperand(symStruct, opCode.intDisp);
        
        for (int i = 0; i < operand.length(); i++) {
            lineBuf[i+17] = operand[i];
            lisLineBuf[i+23] = operand[i];
        }

        //null terminate and write to files
        lisLineBuf[lisLineBuf.length()] ='\0';
        lineBuf[lineBuf.length()] = '\0';
        sicFile << '\n' << lineBuf;
        lisFile << '\n' << lisLineBuf;
    }

    ss << hex << setw(6) << setfill('0') << objStruct.LOCCTR;
    string strAdr = ss.str();
    string temp = "";

    for (index = 0; index < symStruct.symTabLength; index++) {
        temp.assign(symStruct.symVector[index],8,6);
        if (temp == strAdr) {
            found = 1;
            break;
        }
    }

    if (found == 0) {
        for (index = symStruct.symTabLength + 1; index < symStruct.symVector.size(); index++) {
            temp.assign(symStruct.symVector[index], 24, 6);

            if (temp == strAdr) {
            found = 1;
            break;
            }   
        }
    }
    
    if (found == 1) {
        //symbols dont really matter in terms of assembler directives
        //literals are more of our concern
    if (index < symStruct.symTabLength) { 
        temp.assign(symStruct.symVector[index],0,6);
    }

    if (index >= symStruct.symTabLength + 1) {
        string ltorg = "LTORG";
        temp.assign(symStruct.symVector[index],8,8);
        //insert at listing file

        ss.str(string());
        ss.clear();

        ss << setw(4) << setfill('0') << hex << objStruct.LOCCTR;
        tp = ss.str();

        ss.str(string());
        ss.clear();

        //address for listing file
        for (int i = 0; i < 4; i++) 
        lisLineBuf[i] = toupper(tp[i]);

        for (int i = 0; i < ltorg.length(); i++) //insert LTORG
        {
            lineBuf[i+9] = ltorg[i];
            lisLineBuf[i+15] = ltorg[i];
        }

        //null terminate and insert LTORG 
        lisLineBuf[lisLineBuf.length()] = '\0';
        lineBuf[lineBuf.length()] = '\0';
        sicFile << '\n' << lineBuf;
        lisFile << '\n' << lisLineBuf;

        //insert literals since beginning/last LTORG
        while (symStruct.newLiterals > 0) { 
        
        //clear string buffer for listing file
        for (int i = 0; i < lisLineBuf.length(); i++)
            lisLineBuf[i] = ' ';

        ss << setw(4) << setfill('0') << hex << objStruct.LOCCTR;
        tp = ss.str();

        ss.str(string());
        ss.clear();

        //address of current literal
        for (int i = 0; i < 4; i++) 
            lisLineBuf[i] = toupper(tp[i]);

        //find the literal at that address
        temp = findOperand(symStruct,objStruct.LOCCTR);
        for (int i = 0; i < temp.length(); i++)
            lisLineBuf[i+14] = temp[i];

        temp.assign(symStruct.symVector[index],17,5);
        increment = (int)strtol(temp.c_str(),NULL,10);
        tp.assign(objStruct.objVector[trIndex], objStruct.textRecordPointer, increment);
        
        //insert obj code of literal
        for (int i = 0; i < tp.length(); i++)
            lisLineBuf[i+32] = tp[i];

        //star directive
        lisLineBuf[6] = '*';

        //increment text record pointer and LOCCTR according to literal length
        objStruct.textRecordPointer += increment;
        objStruct.LOCCTR += increment/2;
        lisFile << '\n' << lisLineBuf;

        //decrement amount of new literals after printing 
        symStruct.newLiterals -= 1;
        }
    }
}
    
}