/*
Author: Siddharth Dahiya
Package: Course and Professor Scheduling (Backend)
File: Utility.h
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



#ifndef UTILITY_H
#define UTILITY_H
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;
class Utility {

public:
    static void CleanWhiteSpace(string &toClean) {
        while (toClean.find(" ") != string::npos) {
            toClean.replace(toClean.find(" "), 1, "");
        }

        while (toClean.find("\t") != string::npos) {
            toClean.replace(toClean.find("\t"), 1, "");
        }

    }

    static void LowerCase(string &toLower) {
        int len = toLower.length();
        for (int i = 0; i < len; i++) {
            if (toLower[i] > 64 && toLower[i] < 91)
                toLower[i] += 32;
        }
    }

    /*
    This function returns a vector of strings
    */
    static vector<string> Tokenize(string toTokenize, char token) {
        vector<string> tokenList;
        while (toTokenize.find(token) != string::npos) {
            tokenList.push_back(toTokenize.substr(0, toTokenize.find(token)));
            toTokenize.erase(0, toTokenize.find(token) + 1);
        }
        if (!toTokenize.empty())
            tokenList.push_back(toTokenize);
        return tokenList;
    }

    static void RemoveChar(string &str, char toRemove) {
        str.erase(std::remove(str.begin(), str.end(), toRemove), str.end());
    }

    static double CalculateMean(int values[], int size) {
        double sum = 0.0;
        for (int i = 0; i < size; ++i) {
            sum += values[i];
        }
        return sum / size;
    }

    static double StandardDeviation(int values[], int size, double mean) {
        double dev = 0.0, sdev = 0.0;
        for (int i = 0; i < size; ++i) {
            dev = (values[i] - mean)*(values[i] - mean);
            sdev = sdev + dev;
        }

        return sqrt(sdev / (size - 1));
    }

    static string FormatDouble(double value, int decimals) {
        stringstream output;
        output << fixed << setprecision(decimals) << value;
        return output.str();
    }

};
#endif