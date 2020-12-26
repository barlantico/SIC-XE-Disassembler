/**
* Brian Arlantico, cssc3010, 821125494
* Nicholas Dollick, cssc3046, 815970305
* CS530 Fall 2020
* Assignment 2, Disassembler for XE computer
* searchdir.cpp
*/
using namespace std;
#include "functions.hpp"

void searchDirectory(vector<string> &files)
{

DIR *dirPointer; //directory pointer
struct dirent *dirStruct; //directory structure
string direc = "."; //linux directory

if((dirPointer = opendir(direc.c_str())) == NULL) //error checking
    cout << "Could not find directory" << endl;

while ((dirStruct = readdir(dirPointer)) != NULL) //look for all files in the directory
{
    files.push_back(string(dirStruct->d_name)); //saves names of files onto string vector
}

closedir(dirPointer);

}

int getFiles(vector<string> &files, ifstream &symfile, ifstream &objfile, string file) {

    string temp = file;

    //append file extensions to the file name
    string ofile = file.append(".obj"); 
    string sfile = temp.append(".sym");

    //find files and open it if they are present
    for (unsigned int i = 0;i < files.size();i++) {
        if (files[i].compare(ofile) == 0) {
            objfile.open(ofile.c_str());
        }
        if (files[i].compare(sfile) == 0) {
            symfile.open(sfile.c_str());
        }
    }

    //checking if one or more files could not be found
    if (!objfile.is_open())
        cout << "Error: " << ofile << " could not be found." << endl;

    if (!symfile.is_open())
        cout << "Error: " << sfile << " could not be found." << endl;

    if (!objfile.is_open() || !symfile.is_open())
        return 0;

    return 1; //success in finding the files

}