# SIC-XE-Disassembler
========================================================================================
                            README
whoami:
    Brian Arlantico, cssc3010, 821125494
    Nicholas Dollick, cssc3046, 815970305
    CS 530, Fall 2020
    Assignment 2, Disassembler for XE computer


Desc:
    A disassembler for sic/xe architecture
                 

Usage:
    compile commands:
        make
            to compile all files
        make clean
            to remove all compiled files

    run:
        ./dis <filename without extension> 

    compile and run using:
        make && ./dis <filename without extension> 

IMPORTANT: both the object file and symbol file must be in the same directory as dis

TESTING WITH INCLUDED SAMPLE FILES:
    dis samp
    dis q2
    dis P2Sample

Manifest:
    functions.hpp
        INFO
            Include and namespace declarations. Prototypes functions. Defines structs for opcodes,
		obj file data, and symbol table data

    searchdir.cpp
        void searchDirectory(vector<string> &files)
	    INFO
		assembles name of all files to be processed into one vector
            VARS IN
                files: an array of files to be opened and processed

        int getFiles(vector<string> &files, ifstream &symfile, ifstream &objfile, string file) 
            VARS IN
                files: vector of all files to be procesed
		symfile: the .sym to be processed
		objfile: the .obj to be processed
		file: filename of .obj/.sym

            VARS OUT
                returns 1 if files exist and can be read or 0 if problem occurred

    structs.cpp
    	INFO
	    Helper fucntions for converting of data to required formats

	char* hex_char_to_bin(char c)
	    VARS IN
                c: char to be covnerted into binary
            VARS OUT
                binary representation as a string

	char* getRegister(int num)
	    VARS IN
                num: register number to be fetched 
            VARS OUT
                register name as string

	string bin_string_to_hex(string bin)
	    VARS IN
                bin: binary string to be converted into its hex equivalent
            VARS OUT
                hex representation of binary input as string

	void getMnemonic(string stropCode, struct opCodeStruct &opCode)
	    INFO
		converts raw obj data into proper mnemonics and format
	    VARS IN
                stropCode: opcode as hex string
		opCode: object which records the converted obj data
	
    disassembler.cpp
    	INFO
	    Called by main to perform main functions of disassembler

	void disassemble(ifstream &objfile, ifstream &symfile, string file)
	    INFO
	    	driver for the disassembling process
	    VARS IN
                objfile: object file provided by user
		symfile: symbol file including symbol table and literal table
		file: name of file as string

	void endDirective(struct objCodeStruct &objStruct, struct symbolTable symStruct, ofstream & sicFile, ofstream &lisFile)
	    INFO
	    	function called at the end of the disassembling process to insert end directive
	    VARS IN
                objStruct: struct for info such as BASE, LOCCTR, programLength
		symStruct: struct that holds information of the symbol table and literal table
		sicFile: output file stream for sic file
		lisFile: output file stream for lis file

	void insertVars(struct objCodeStruct &objStruct, struct symbolTable symStruct, ofstream &sicFile, ofstream &lisFile, int maxAdr)
	    INFO
	    	takes into account variables declared that does not produce object code
	    VARS IN
                objStruct: struct for info such as BASE, LOCCTR, programLength
		symStruct: struct that holds information of the symbol table and literal table
		sicFile: output file stream for sic file
		lisFile: output file stream for lis file
		maxAdr: address that variables are defined until

	int getVarLength(struct objCodeStruct &objStruct, struct symbolTable symStruct)
	    INFO
		retrieves the length of a symbol/literal
	    VARS IN
                objStruct: struct for info such as BASE, LOCCTR, programLength
		symStruct: struct taht holds information of the symbol table and literal table
	    VARS OUT
	    	length of the variable in bytes as an integer
		
	void readobjFile(ifstream &objfile, vector<string> &objVector)
	    INFO
		reads each line of the object file and stores it on a string vector
	    VARS IN
                objfile: object file provided by the user
		objVector: string vector for the object file
		
	void readsymFile(ifstream &symfile, struct symbolTable &symTabStruct)
	    INFO
		reads each line of the symbol table and stores it in the string vectors of
		the symTabStruct
	    VARS IN
                symFile: symbol file provided by the user
		symTabStruct: struct containing information of the symbol table and literal table
		
	void startDirective(struct objCodeStruct &objStruct, ofstream &sicFile, ofstream &lisFile)
	    INFO
		prints the first assembler directive to both sic and lis files
	    VARS IN
                objStruct: struct that holds information such as BASE, LOCCTR,  and programLength
		sicFile: output file stream for sic file
		lisFile: output file stream for lis file
		
	void readObjCode(struct objCodeStruct &objStruct, struct opCodeStruct &opCode, int index)
	    INFO
		analyzes current object code to dissect important information
	    VARS IN
                objStruct: struct that holds information such as BASE, LOCCTR,  and programLength
		opCode: struct that highlights important information of object code (flags, format, etc)
		index: current line of the object file
		
	void printFormat2(struct symbolTable symStruct, struct objCodeStruct &objStruct, struct opCodeStruct &opCode, ofstream &sicFile, ofstream &lisFile, int index)
	    INFO
		writes the disassembled format 2 code onto the sic and lis files
	    VARS IN
                symStruct: struct that holds info from symbol table
 		objStruct: struct that holds BASE, LOCCTR, programLength 
 		opCode: struct that holds necessary info of a certain objCode (flags, format, etc)
 		sicFile: output file for sic file
 		lisFile: output file for lis file
 		index: current line of the object file
		
	void printFormat3(struct symbolTable symStruct, struct objCodeStruct &objStruct, struct opCodeStruct &opCode, ofstream &sicFile, ofstream &lisFile, int index)
	    INFO
		writes the disassembled format 3 code onto the sic and lis files
	    VARS IN
                symStruct: struct that holds info from symbol table
 		objStruct: struct that holds BASE, LOCCTR, programLength 
 		opCode: struct that holds necessary info of a certain objCode (flags, format, etc)
 		sicFile: output file for sic file
 		lisFile: output file for lis file
 		index: current line of the object file
		
	void printFormat4(struct symbolTable symStruct, struct objCodeStruct &objStruct, struct opCodeStruct &opCode, ofstream &sicFile, ofstream &lisFile, int index)
	    INFO
		writes the disassembled format 4 code onto the sic and lis files
	    VARS IN
                symStruct: struct that holds info from symbol table
 		objStruct: struct that holds BASE, LOCCTR, programLength 
 		opCode: struct that holds necessary info of a certain objCode (flags, format, etc)
 		sicFile: output file for sic file
 		lisFile: output file for lis file
 		index: current line of the object file
		
	string insertSymbol(struct symbolTable symStruct, struct objCodeStruct &objStruct)
	    INFO
		finds and returns the symbol string (if any) at the current LOCCTR 
	    VARS IN
                symStruct: struct that holds info from symbol table
 		objStruct: struct that holds BASE, LOCCTR, programLength
	    VARS OUT
	    	string of the symbol at that current LOCCTR 

	string findOperand(struct symbolTable symStruct, int TA)
	    INFO
		using the target address and addressing mode, finds the operand as a string
	    VARS IN
                symStruct: struct that holds info from symbol table
 		TA: target address in displacement field of object code as an integer
	    VARS OUT
	    	operand as a string 
		
	void insertDirective(struct symbolTable symStruct, struct objCodeStruct &objStruct, struct opCodeStruct &opCode, ofstream &sicFile, ofstream &lisFile, int trIndex)
	    INFO
		depending on the instruction just printed or literal printed, it selectively inserts directives on the correct lines
	    VARS IN
                symStruct: struct that holds info from symbol table
                objStruct: struct that holds BASE, LOCCTR, programLength 
                opCode: struct that holds important info about an objcode (flags, format, etc)
                sicFile: output file for sic file
                lisFile: output file for lis file
                trIndex: index for text record in objStruct
		
    main.cpp
        int main(int argc, char **argv)
            INFO
                Driver function
    
    INCLUDED TEST FILES
	P2Sample.obj
	P2Sample.sym
	q2.obj
	q2.sym
	samp.obj
	samp.sym
    		
Significant design decisions:

    Created three different structs for the main data structures of this system. This 
    included opCodeStruct, objCodeStruct, and symbolTable. Specifically, the opCodeStruct
    contains the nixbpe flags, format, displacement, the mnemonic, and the entire object 
    code of that instruction. The objCodeStruct includes important information about the 
    program itself, including the length, the LOCCTR, BASE (if defined), and the pointer
    to the current instruction of the text record. The last struct defined is for the symbol
    table, which includes string vectors for the symbol table and literal table, int values
    for the length of the symbol table, literal table, and if any new literals are encountered. 
    The decision to separate the three components into different structs is to utilize
    the pass by reference feature to functions to reduce the amount of arguments each
    function needs. There is also an apparent distinction between the three components, 
    which made it natural to separate them into the three structs. 
    
    Another design decision was to disregard the modification records in the obj file.
    Since our system is a disassebmler, we are not concerned with loading the program
    to any particular address, so the modification records held no significance to 
    our procedures.
    
    The work between the two of us was split in this way: Nicholas worked on identifying
    the nixbpe bits and all the functions located in the structs.cpp while Brian focused
    on the algorithm of the disassembler which in turn utilizes the functions created by
    Nicholas. We evenly contributed to the makefile, SDD, README, and the test files.
    
    There is not any extra functionality that our system can provide as all the features/
    algorithms in our system are required to perform the disassembly process.
    
    Currently, we have not discovered any known bugs or deficiencies, but a wrong object
    file or symbol file can cause run-time errors such as segmentation faults. Our system
    was built on the premise that the object file and symbol file were generated by a 
    working assembler, which is why our program does not perform any error checking beyond
    ensuring that both files are present.
    
========================================================================================
                    NOTES AND REFLECTION

One of the difficulties in this process was the uncertainty created during file updates.
It seemed like each iteration of project files broke the output with incorrect format 
and required us to verify the integrity of the object files by hand. Included in this
submission is folder of known working test files. They consist of corrected versions
of the files from canvas, and custom ones based on book examples. The most recent change
to the sample files on canvas (more specifically, the structure of the literal table) 
could not be accounted for, due to the fact that we were very far in our testing 
phase once this change in structure was introduced. We felt that attempting to 
incorporate this change would only lead to further bugs in our program that may remain
undiscovered, so the decision to adhere to the preivous structure of the literal table
ensures our system will function as expected.

========================================================================================
