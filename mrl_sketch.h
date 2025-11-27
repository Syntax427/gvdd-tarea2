#ifndef MRL_SKETCH_H
#define MRL_SKETCH_H

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

using namespace std;

class MRL_Sketch {
private:
    double e;
    int n;
    int k;
    int l;
    vector<vector<int>> A;

public:
    MRL_Sketch(double epsilon, int size);

    void printInfo();
    void printCompacter();
    void insertar(int i);
    int rank(int x);
    int select(int r);
    int quantile(double phi);
};

#endif