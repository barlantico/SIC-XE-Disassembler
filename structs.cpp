/**
* Brian Arlantico, cssc3010, 821125494
* Nicholas Dollick, cssc3046, 815970305
* CS530 Fall 2020
* Assignment 2, Disassembler for XE computer
* structs.cpp
*/
using namespace std;
#include "functions.hpp"

/**
 * @brief Takes hex number as a character and returns binary string representation
 * @param c the character of hex number
 * @return binary in string format
*/
const char* hex_char_to_bin(char c)
{

    switch(toupper(c))
    {
        case '0': return "0000";
        case '1': return "0001";
        case '2': return "0010";
        case '3': return "0011";
        case '4': return "0100";
        case '5': return "0101";
        case '6': return "0110";
        case '7': return "0111";
        case '8': return "1000";
        case '9': return "1001";
        case 'A': return "1010";
        case 'B': return "1011";
        case 'C': return "1100";
        case 'D': return "1101";
        case 'E': return "1110";
        case 'F': return "1111";
        default: return "AAAA"; //not a hex number
    }
}

/**
 * @brief Retrieve register mnemonic from number
 * @param num is the integer representation of the register
 * @return string of the register mnemonic
*/
const char* getRegister(int num) {
    switch(num)
    {
        case 0 : return "A";
        case 1 : return "X";
        case 2 : return "L";
        case 3 : return "B";
        case 4 : return "S";
        case 5 : return "T";
        case 6 : return "F";
        case 8 : return "PC";
        case 9 : return "SW";
        default : return "?"; //invalid register
    }

}

/**
 * @brief Takes a binary string and returns the hex representation as a string
 * @param bin is binary in string format
 * @return hex string
*/
string bin_string_to_hex(string bin) {
    string temp = "";
    string opHex = "";

    for (int i = 0; i < bin.length(); i +=4) {
        temp.assign(bin,i,4);

        
            if (temp == "0000")
                opHex.append("0");
            else if (temp == "0001")
                opHex.append("1");
            else if (temp == "0010")
                opHex.append("2");
            else if (temp == "0011")
                opHex.append("3");
            else if (temp == "0100")
                opHex.append("4");
            else if (temp == "0101")
                opHex.append("5");
            else if (temp == "0110")
                opHex.append("6");
            else if (temp == "0111")
                opHex.append("7");
            else if (temp == "1000")
                opHex.append("8");
            else if (temp == "1001")
                opHex.append("9");
            else if (temp == "1010")
                opHex.append("A");
            else if (temp == "1011")
                opHex.append("B");
            else if (temp == "1100")
                opHex.append("C");
            else if (temp == "1101")
                opHex.append("D");
            else if (temp == "1110")
                opHex.append("D");
            else if (temp == "1111")
                opHex.append("F");
            else
                opHex.append("?"); //unknown binary string
        
    }

    return opHex;


}

/**
 * @brief returns mnemonic of the opcode and differentiates between format 2 or 3/4
 * @param stropCode hex opcode representation as a string
 * @param opCode struct that holds necessary info of a certain objCode (flags, format, etc)
 * @return void
*/
void getMnemonic(string stropCode, struct opCodeStruct &opCode) {

    int index = 0;

    const static string ops[] = {
    "18", "58", "90", "40", "B4", "28",
    "88", "A0", "24", "64", "9C", "C4",
    "C0", "F4", "3C", "30", "34", "38",
    "48", "00", "68", "50", "70", "08",
    "6C", "74", "04", "D0", "20", "60",
    "98", "C8", "44", "D8", "AC", "4C",
    "A4", "A8", "F0", "EC", "0C", "78",
    "54", "80", "D4", "14", "7C", "E8",
    "84", "10", "1C", "5C", "94", "B0",
    "E0", "F8", "2C", "B8", "DC"
};

const static string mnemonics[] = {
    "ADD", "ADDF", "ADDR", "AND", "CLEAR", "COMP",
    "COMPF", "COMPR", "DIV", "DIVF", "DIVR", "FIX",
    "FLOAT", "HIO", "J", "JEQ", "JGT", "JLT",
    "JSUB", "LDA", "LDB", "LDCH", "LDF", "LDL",
    "LDS", "LDT", "LDX", "LPS", "MUL", "MULF",
    "MULR", "NORM", "OR", "RD", "RMO", "RSUB",
    "SHIFTL", "SHIFTR", "SIO", "SSK", "STA", "STB",
    "STCH", "STF", "STI", "STL", "STS", "STSW",
    "STT", "STX", "SUB", "SUBF", "SUBR", "SVC",
    "TD", "TIO", "TIX", "TIXR", "WD"
};
//search for the mnemonic index
while (stropCode != ops[index])
        index++;

//sets format 2 if format 2 instruction found
if (mnemonics[index] == "ADDR")
    opCode.format = 2;
else if (mnemonics[index] == "CLEAR")
    opCode.format = 2;
else if (mnemonics[index] == "COMPR")
    opCode.format = 2;
else if (mnemonics[index] == "DIVR")
    opCode.format = 2;
else if (mnemonics[index] == "MULR")
    opCode.format = 2;
else if (mnemonics[index] == "RMO")
    opCode.format = 2;
else if (mnemonics[index] == "SHIFTL")
    opCode.format = 2;
else if (mnemonics[index] == "SHIFTR")
    opCode.format = 2;
else if (mnemonics[index] == "SUBR")
    opCode.format = 2;
else if (mnemonics[index] == "SVC")
    opCode.format = 2;
else if (mnemonics[index] == "TIXR")
    opCode.format = 2;
else 
    opCode.format = 0;

    opCode.opMnemonic = mnemonics[index];
}
