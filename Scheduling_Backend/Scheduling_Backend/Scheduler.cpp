/*
Author: Siddharth Dahiya
Package: Course and Professor Scheduling (Backend)
File: Scheduler.cpp
Contact: syd5144@gmail.com

Copyright (c) 2015 Siddharth Dahiya

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/


#include "Population.h"
#include "getCPUTime.c"
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

struct stat info;

int main(int argc, char* argv[] ) {
    if(argc < 4){
    	cerr << "Invalid number of arguments provided. Delta, Input Filename and output destination folder are required." << endl;
    	exit(-1);
    }
    
    int suffix_cntr = 0;
    int delta = stoi(argv[1]);
    string prepend(argv[3]);
    string inputFile(argv[2]);
		if (stat (argv[2], &info) != 0){
			cerr << "Invalid input file. File does not exist." << endl;
			exit(-1);
		}
    
    string destinationFolder(argv[3]);
    bool console = false;
	bool printAll = false;
    if(argc >= 5 && strcmp(argv[4], "yes") == 0){
    	cout << "Outputting to Console." << endl;
    	console = true;
    }
	
	if(argc == 6 && strcmp(argv[5], "all") == 0){
    	cout << "Printing all results." << endl;
    	printAll = true;
    }
	
    char hostnameCharArray[128];
    gethostname(hostnameCharArray, sizeof hostnameCharArray);
    
    string hostname(hostnameCharArray);
		if(console){
			cout << "Hostname: " << hostname << endl;
		}
		
    if(prepend.back() != '/'){
    	prepend += "/";
    	destinationFolder += "/";
    }
   	prepend += hostname + "-";
   	destinationFolder += hostname + "-";
   	prepend += "stat";
    
    if(!(stat(argv[3], &info) == 0 && info.st_mode && S_IFDIR)){
    	cout << "(" << argv[2] << ") is not a valid directory. Exiting..." << endl;
    	exit(-1);
    }
    
    while (std::ifstream( prepend + to_string(suffix_cntr) + ".txt")) {
        suffix_cntr++;
    }
     
    double startTime, endTime;
    startTime = getCPUTime( );
    
    Population p(delta, inputFile, destinationFolder, suffix_cntr, console);
    p.Evolve( );
    if(console){
		  cout << endl << "End Evolution" << endl;
		  cout << "*********************************************************************" << endl;
		}
    p.PrintEnd(printAll, true);
    endTime = getCPUTime( );
    ofstream statFile;
    string file = prepend + to_string(suffix_cntr) + ".txt";
    statFile.open(file, ofstream::out | ofstream::app);
    statFile << "CPU time used = " << ( endTime - startTime ) << endl;
    statFile.close( );
 		if(console){
    	fprintf(stdout, "CPU time used = %lf\n", ( endTime - startTime ));
    }
}
