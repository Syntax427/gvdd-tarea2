#include "mrl_sketch.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <cmath>

using namespace std;

vector<int> leerDatos(string nombreArchivo) {
    vector<int> datos;
    ifstream archivo(nombreArchivo);
    int valor;
    
    if (!archivo.is_open()) {
        cerr << "no se pudo abrir el archivo " << nombreArchivo << endl;
        return datos;
    }

    while (archivo >> valor) {
        datos.push_back(valor);
    }
    
    archivo.close();
    return datos;
}

// cuantos elementos son menores que x
int obtenerRankReal(const vector<int>& datosOrdenados, int x) {
    // lower_bound retorna el primer elemento >= x. 
    // La distancia desde el inicio es la cantidad de elementos < x.
    auto it = lower_bound(datosOrdenados.begin(), datosOrdenados.end(), x);
    return distance(datosOrdenados.begin(), it);
}

void ejecutarExperimento(string nombreArchivo, vector<double> epsilons) {
    cout << "Leyendo " << nombreArchivo << endl;
    
    vector<int> datos = leerDatos(nombreArchivo);
    if (datos.empty()) return;

    int n = datos.size();
    cout << "cargados " << n << " elementos." << endl;

    vector<int> datosOrdenados = datos;
    sort(datosOrdenados.begin(), datosOrdenados.end());

    string nombreSalida = "resultados_" + nombreArchivo + ".csv";
    ofstream salida(nombreSalida);
    salida << "Epsilon,TipoConsulta,ValorConsulta,ValorEstimado,ValorReal,ErrorAbsoluto,ErrorRelativo,ErrorMaxPermitido\n";

    for (double eps : epsilons) {
        cout << "  > epsilon = " << eps << endl;

        // llenar MRL Sketch
        MRL_Sketch sketch(eps, n);
        for (int x : datos) {
            sketch.insertar(x);
        }

        // error de rank
        // prueba 50 puntos distribuidos uniformemente en el rango de datos
        for (int i = 0; i < 50; i++) {
            // toma valores que realmente existen en el dataset para probar
            int indicePrueba = (n / 50) * i;
            if (indicePrueba >= n) indicePrueba = n - 1;
            int valorQuery = datosOrdenados[indicePrueba];

            int rankEst = sketch.rank(valorQuery);
            int rankReal = obtenerRankReal(datosOrdenados, valorQuery);
            
            int error = abs(rankEst - rankReal);
            double limiteTeorico = eps * n; 

            salida << eps << ",RANK," << valorQuery << "," << rankEst << "," 
                   << rankReal << "," << error << "," 
                   << (double)error/n << "," << limiteTeorico << "\n";
        }

        //error de quantile
        // prueba quantiles desde 0.05 hasta 0.95
        for (double phi = 0.05; phi < 1.0; phi += 0.05) {
            int valEst = sketch.quantile(phi);
            
            int idxReal = floor(phi * n);
            if(idxReal >= n) idxReal = n-1;
            int valReal = datosOrdenados[idxReal];
            int errorValor = abs(valEst - valReal);

            int rankDelEstimado = obtenerRankReal(datosOrdenados, valEst);
            int rankObjetivo = phi * n;
            int errorRankEnQuantil = abs(rankDelEstimado - rankObjetivo);
            
            double limiteTeorico = eps * n;

            salida << eps << ",QUANTILE," << phi << "," << valEst << "," 
                   << valReal << "," << errorValor << "," 
                   << errorRankEnQuantil << "," << limiteTeorico << "\n";
        }
    }
    cout << "  Resultados guardados en " << nombreSalida << endl << endl;
}

int main() {
    vector<string> archivos = {
        "chicago2015.txt", 
        "chicago2016.txt", 
        "uniform.txt", 
        "log-normal.txt"
    };

    vector<double> epsilons = {0.1, 0.05};

    for (const auto& archivo : archivos) {
        ejecutarExperimento(archivo, epsilons);
    }

    return 0;
}