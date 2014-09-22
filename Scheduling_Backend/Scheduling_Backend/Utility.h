#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <vector>
using namespace std;
class Utility {

public:
    static void CleanWhiteSpace(string & toClean) {
        while (toClean.find(" ") != string::npos) {
            toClean.replace(toClean.find(" "), 1, "");
        }

        while (toClean.find("\t") != string::npos) {
            toClean.replace(toClean.find("\t"), 1, "");
        }

    }

    static void LowerCase(string & toLower) {
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
            toTokenize.erase(0, toTokenize.find(token)+1);
        }
        if (!toTokenize.empty( ))
            tokenList.push_back(toTokenize);
        return tokenList;
    }
};

#endif