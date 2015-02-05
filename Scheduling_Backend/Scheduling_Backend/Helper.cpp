/*
Author: Siddharth Dahiya
Package: Course and Professor Scheduling (Backend)
File: Helper.h
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
#ifndef HELPER_H_
#define HELPER_H_

#include <random>
#include <cmath>

class Helper {
public:
    std::mt19937 rand_gen;
    std::random_device plant;
    unsigned long seed;

    Helper( ) {
        seed = plant();
        seed = 2381215245;
        rand_gen.seed(seed);
    }

    inline double randNum( ) {
        std::uniform_real_distribution<> random(0, 1);
        return random(rand_gen);
    }

    inline int randNum(int min, int max) {
        std::uniform_int_distribution<> random(min, max);
        return random(rand_gen);
    }
    
    inline double randDouble(double min, double max) {
        std::uniform_real_distribution<> random(min, max);
        return random(rand_gen);
    }

    inline bool randBoolean( ) {
        std::uniform_int_distribution<> random(0, 1);
        return (random(rand_gen) % 2) == 0;
    }
};

#endif