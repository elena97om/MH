/*
PROBLEMA DE LA MÁXIMA DIVERSIDAD(MDP)
PRÁCTICA 1: TÉCNICAS DE BÚSQUEDA LOCAL Y ALGORITMOS GREEDY
METAHEURÍSTICAS UGR
ELENA ORTIZ MORENO
CURSO 20/21 
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <utility>
#include <chrono>
#include "random.h"

using namespace std;
using namespace std::chrono;

int leerDatos(vector<vector<double> > &m, string nombre){
  ifstream fichero(nombre.c_str());
  int n, i, j;
  double distancia;
  fichero >> n;
  m.resize(n);
  for(i = 0; i < n; i++){
    m[i].resize(n);
  }
  fichero >> n;

  while(fichero >> i >> j >> distancia){
    m[i][j] = distancia;
    m[j][i] = distancia;
    m[i][i] = 0;
  }
  fichero.close();
  return n;
}

double funcionObjetivo(const vector<int> &solucion, const vector<vector<double> > &m){
  double suma = 0;
  for(int i = 0; i < solucion.size() - 1; i++){
    for(int j = i + 1; j < solucion.size(); j++)
      suma += m[solucion[i]][solucion[j]];
  }
  return suma;
}

double contribucion(int elemento, const vector<int> &solucion, const vector<vector<double> > &m){
  double suma = 0;
  for(int i = 0; i < solucion.size(); i++){
    suma += m[solucion[i]][elemento];
  }
  return suma;
}

double distanciaAlResto(int elemento, const vector<vector<double> > &m){
  int suma = 0;
  for(int i = 0; i < m.size(); i++){
      suma += m[elemento][i];
  }
  return suma;
}

int indiceMayorContribucion(const vector<pair<int,double> > &contribuciones){
  double mayorContribucion = 0;
  int indice = 0;
  for(int i = 0; i < contribuciones.size(); i++){
    if(contribuciones[i].second > mayorContribucion){
      mayorContribucion = contribuciones[i].second;
      indice = i;
    }
  }
  return indice;
}

double contribucionUnoASeleccionados(int elemento, const set<int> &solucion, const vector<vector<double> > &m){
  double coste=99999;
  for(auto i = solucion.begin(); i != solucion.end(); i++){
    if(m[elemento][*i] < coste){
      coste = m[elemento][*i];
    }
  }
  return coste;
}


set<int> Greedy(const vector<vector<double> > &m, int n){
  set<int> seleccionados;
  set<int> vecinos;
  vector<pair<int,double> > contribuciones;

  for(int i = 0; i < m.size(); i++){ //distancias de todos a todos
    contribuciones.push_back(pair<int,double>(i,distanciaAlResto(i, m)));
    vecinos.insert(i);
  }

  int ultimo_seleccionado = indiceMayorContribucion(contribuciones);
  seleccionados.insert(ultimo_seleccionado);
  vecinos.erase(ultimo_seleccionado);

  while(seleccionados.size() < n){
    contribuciones.clear();

    for(auto i = vecinos.begin(); i != vecinos.end(); i++){
      // Calculo las contribuciones minimas
      contribuciones.push_back(pair<int,double>(*i,contribucionUnoASeleccionados(*i,seleccionados,m)));
    }
    double max = 0;
    for(int i = 0; i < contribuciones.size(); i++){
      if(max < contribuciones[i].second){
        max = contribuciones[i].second;
        ultimo_seleccionado = contribuciones[i].first;
      }
    }
    seleccionados.insert(ultimo_seleccionado);
    vecinos.erase(ultimo_seleccionado);
  }
  return seleccionados;
}

int BL(vector<int> &solucion, const vector<vector<double> > &m, int evaluaciones, int n){
  int contador = 0;
  bool mejora = true;
  vector<double> costes;
  vector<int> vecinos;

  while(solucion.size() != n){
    bool metido = false;
    int random = Randint(0,m.size()-1);
    for(int i = 0; i < solucion.size(); i++){
      if(solucion[i] == random) metido = true;
    }
    if(!metido)  solucion.push_back(random);
  }

  for(int i = 0; i < m.size(); i++){
    bool metido = false;
    for(int j = 0; j < solucion.size(); j++){
      if(solucion[j] == i){
        metido = true;
      }
    }
    if(!metido) vecinos.push_back(i);
  }
  for(int i = 0; i < solucion.size(); i++){
    costes.push_back(contribucion(solucion[i], solucion, m));
  }
  while(contador < evaluaciones){
    for(int i = 0; i < solucion.size(); i++){ //empieza el cambio
      mejora = false;
      int elemento = solucion[i];
      for(int j = 0; j < vecinos.size(); j++){
        contador++;
        solucion[i] = vecinos[j];
        double contribucion_j = contribucion(vecinos[j], solucion, m);
        if(contribucion_j > costes[i]){ //Actualizo la contribución y sigo el cambio
          for(int k  = 0; k < solucion.size(); k++){
            costes[k] -= m[elemento][k];
            costes[k] += m[vecinos[j]][k];
          }
          vecinos[j] = elemento;
          mejora = true;
          break;
        }
        solucion[i] = elemento;
        if(contador == evaluaciones) break;
      }
      if(mejora || (contador == evaluaciones)) break;
    }
    if (!mejora) break;
  }
  return contador;
}

int main(int argc, char const *argv[]) {
  if (argc!=3) {
    cout << argv[0] << " archivo_istancias semilla" << endl;
    return -1;
  }

  int semilla = strtol(argv[2],NULL,10);
  Set_random(semilla);
  vector<vector<double> > distancias;
  int n = leerDatos(distancias, argv[1]);

  vector<int> solucionGreedy;
  vector<int> solucionBL;
  vector<int> vecinos;

  duration<double> intervalo;
  high_resolution_clock::time_point ini, fin;
  double tiempo;

  ini = high_resolution_clock::now();
  set<int> seleccionados = Greedy(distancias, n);
  fin = high_resolution_clock::now();
  intervalo = duration_cast<duration<double> > (fin-ini);
  tiempo = intervalo.count();

  for(auto i = seleccionados.begin(); i != seleccionados.end(); i++){
    solucionGreedy.push_back(*i);
  }

  //Cálculo funcionObjetivo de la solucionGreedy:
  double costeGreedy = funcionObjetivo(solucionGreedy, distancias);

  ini = high_resolution_clock::now();
  BL(solucionBL,distancias,100000,n);
  fin = high_resolution_clock::now();

  intervalo = duration_cast<duration<double> > (fin-ini);
  double tiempobl = intervalo.count();

  //Cálculo funcionObjetivo de la solucionGreedy:
  double costeBL = funcionObjetivo(solucionBL, distancias);

  cout << "Tiempo Greedy: " << tiempo << endl;
  cout << "Coste Greedy: " << costeGreedy << endl;
  cout << "Tiempo BL: " << tiempobl << endl;
  cout << "Coste BL: " << costeBL << endl;
  cout << "Archivo: " << argv[1] << endl;
  cout << "Semilla: " << semilla << endl;
  return 0;
}

