#include "mrl_sketch.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

using namespace std;

MRL_Sketch::MRL_Sketch(double epsilon, int size) : e(epsilon), n(size) {
        if (epsilon <= 0.0 || epsilon >= 1.0) {
            throw invalid_argument("epsilon debe estar en (0,1)");
        }
        if (n <= 0) {
            throw invalid_argument("n debe ser positivo");
        }

        k = static_cast<int>(1/e * ceil(log2(e*n)) + 1);
        if (k % 2 != 0) k += 1;

        l = static_cast<int>(ceil(log2(n/k)));

        A.assign(l + 1, std::vector<int>());
    }

void MRL_Sketch::printInfo() {
        std::cout << "epsilon = " << e << "\n";
        std::cout << "n = " << n << "\n";
        std::cout << "k = " << k << "\n";
        std::cout << "L = " << l << "\n";
        std::cout << "Total arreglos = " << A.size() << "\n";
}

void MRL_Sketch::printCompacter() {
    cout << A.size() << " filas y " << A[0].size() << " columnas\n";
    for (size_t i = 0; i < A.size(); ++i) {
        std::cout << "Fila " << i << ": ";
        for (size_t j = 0; j < A[i].size(); ++j) {
            std::cout << A[i][j] << " ";
        }
        std::cout << "\n";
    }
}   

void MRL_Sketch::insertar(int i){
    A[0].push_back(i);

    int j = 0;
    while (j <= l && A[j].size() == k) {
        sort(A[j].begin(), A[j].end());

        vector<int> compacted;

        for (int i = 0; i < k; i += 2){
            compacted.push_back(A[j][i]);
        }

        A[j].clear();

        if (j == l){
            A[j] = compacted;
            return;
        }

        for (int v : compacted){
            A[j + 1].push_back(v);
        }
        j++;
    }
}

int MRL_Sketch::rank(int x) {
    int ans = 0;
    for (int j = 0; j <= l; j++) {
        int weight = (1 << j);
        for (int z : A[j]) {
            if (z < x) ans += weight;
        }
    }
    return ans;
}

int MRL_Sketch::select(int r) {
    vector<pair<int,int>> B;

    for (int j = 0; j <= l; j++) {
        int weight = (1 << j);
        for (int z : A[j])
            B.push_back({z, weight});
    }

    sort(B.begin(), B.end(),
            [](auto &a, auto &b){ return a.first < b.first; });

    int sum = 0;
    for (auto &t : B) {
        sum += t.second;
        if (sum >= r) return t.first;
    }

    return B.back().first;
}

int MRL_Sketch::quantile(double phi) {
    int r = floor(phi * n);
    return select(r);
}