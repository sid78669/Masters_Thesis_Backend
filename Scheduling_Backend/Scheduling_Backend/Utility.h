#ifndef UTILITY_H
#define UTILITY_H

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
        int len = toLower.length( );
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
        if (!toTokenize.empty( ))
            tokenList.push_back(toTokenize);
        return tokenList;
    }

    static void RemoveChar(string &str, char toRemove) {
        str.erase(std::remove(str.begin( ), str.end( ), toRemove), str.end( ));
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

    /*
    // Function Definition
    double m1 (double n[], int arraySize)	{
    double sum=0.0;

    for (int i = 0; i <= arraySize; i++)	{
    sum += n[i];
    }
    m2=sum/50;
    return m2;
    }	// Function for mean

    double sD (double n[], int arraySize)	{
    double sD;
    double sum2 = 0.0;

    for ( int i = 0; i <=arraySize; i++ )	{
    sum2 += pow((n[i]-m2),2);
    }
    sD=sqrt(sum2/(arraySize-1));
    return sD;
    }	// Function for standard deviation
    // End of Program
    */

};
#endif