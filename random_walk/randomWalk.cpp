/*
compil:
g++ --std=c++11 randomWalk.cpp -I. -o build/randomWalk

exe:
build/randomWalk ~/recherche/softs/deepFL/qap_generators/instances/qap_uni_7_0.dat 2 100 footprint.dat
build/randomWalk ~/recherche/softs/deepFL/qap_generators/instances/qap_uni_30_0.dat 2 500 footprint.dat
 */

#include <iostream>   // cout
#include <stdexcept>  // runtime_error 
#include <fstream>
#include <random>

typedef unsigned short * Perm;

#include <evalQAP.h>

// global variable : dimension of the problem
unsigned n;
// indexes of permutation
int ** indexes;

// random initialization
void randomPerm(std::mt19937 & gen, Perm p) {
  std::uniform_int_distribution<> dis(0, n-1);

  for(unsigned i = 0; i < n; i++)
    p[i] = i;

  unsigned short tmp;
  unsigned j;

  for(unsigned i = 0; i < n - 1; i++) {
    dis.param( std::uniform_int_distribution<>::param_type(i, n - 1) );
    j = dis(gen);
    tmp  = p[i];
    p[i] = p[j];
    p[j] = tmp;
  }

}

void swap(Perm p, unsigned i, unsigned j) {
  unsigned short tmp = p[i];
  p[i] = p[j];
  p[j] = tmp;
}

unsigned index(unsigned i, unsigned j) {
  unsigned r = n - 1 - i + j;
  return (r * (r + 1) / 2 + j);
}

void indexes_init(int ** indexes, std::pair<unsigned int, unsigned int> * indices) {
  //
  int k;
  for(unsigned i = 0; i < n; i++) {
    indexes[i] = new int[n];

    for(unsigned j = 0; j < i; j++) {
      k = index(i, j);
      indexes[i][j] = k;
      indices[k] = std::pair<unsigned int, unsigned int>(i, j);
    }
  }

  // 
}

// print 
void print(const Perm & p) {
  for(unsigned j = 0; j < n; j++)
    std::cout << p[j] << " ";
}

// random walk with evaluation of neighbors
void randomWalk(std::mt19937 & gen, EvalQAP & eval, unsigned length, ostream & out) {
  // init some neighbor stuff
  unsigned neighborhoodSize = n*(n-1)/2;
  int * deltas = new int[neighborhoodSize];

  std::uniform_int_distribution<> rnd_index(0, neighborhoodSize - 1);

  int ** indexes;
  std::pair<unsigned int, unsigned int> * indices;
  indexes = new int*[n];
  indices = new std::pair<unsigned int, unsigned int>[n*(n-1)/2];

  indexes_init(indexes, indices);

  // one solution
  Perm solution = new unsigned short[n];

  // random iniitalization
  randomPerm(gen, solution);

  // fitness of the current solution
  int fitness = eval(solution);

  // compute all delta values
  for(unsigned i = 1; i < n; i++) {
    for(unsigned j = 0; j < i; j++) {
      deltas[ indexes[i][j] ] = eval.compute_delta(solution, i, j);
    }
  }

  //** Steps
  int ind, k, l;

  for(unsigned iter = 0; iter < length; iter++) {
    // print fitness of the solution and of all neighbors
    out << fitness ;
    for(ind = 0; ind < neighborhoodSize; ind++)
      out << " " << (fitness + deltas[ind]);
    out << std::endl;

    // select a random neighbor, and move
    ind = rnd_index(gen);
    k = indices[ind].first; 
    l = indices[ind].second;
    swap(solution, k, l);
    fitness += deltas[ indexes[k][l] ];

    // update delta values
    for(unsigned i = 1; i < n; i++) 
      for(unsigned j = 0; j < i; j++) 
	deltas[index(i,j)] = eval.compute_delta_fast(deltas[ indexes[i][j] ], solution, k, l, i, j);
  }

  // last solution of the walk
  // print fitness of the solution and of all neighbors
  out << fitness ;
  for(ind = 0; ind < neighborhoodSize; ind++)
    out << " " << (fitness + deltas[ind]);
  out << std::endl;

  // delete of course
  delete solution;
  delete [] deltas;
  delete [] indices;
  for(unsigned i = 0; i < n; i++)
    delete [] indexes[i];
  delete [] indexes;

}
      
// main
int main(int argc, char **argv)
{
  if (argc != 5) {
    cerr << "4 arguments are required. Usage: ./randomWalk qapFilaName seed walkLength outputFileName" << std::endl;
    return 0;
  }

  // file from QAPLib
  EvalQAP eval(argv[1]);

  // random generator
  std::mt19937 gen( atoi(argv[2]) ); 

  // dimension of the problem
  n = eval.n;
  //std::cout << n << std::endl;

  // length of the walk
  unsigned length = atoi(argv[3]);

  // output file
  fstream fileout(argv[4], ios::out);

  randomWalk(gen, eval, length, fileout);

  return 1;
}
