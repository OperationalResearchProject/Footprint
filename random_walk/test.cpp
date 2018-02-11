/*
compil:
g++ -O3 --std=c++11 test.cpp -I. -o build/test

exe:
build/test ~/recherche/softs/deepFL/qap_generators/instances/qap_uni_30_2.dat 2
 */

#include <iostream>   // cout
#include <stdexcept>  // runtime_error 
#include <fstream>
#include <random>

typedef unsigned short * Perm;

#include <evalQAP.h>

// global variable : dimension of the problem
unsigned n;

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

// print 
void print(const Perm & p) {
  for(unsigned j = 0; j < n; j++)
    std::cout << p[j] << " ";
}
      
// main
int main(int argc, char **argv)
{
  if (argc != 3) {
    cerr << "two arguments are required: file name of the QAP instance, and random seed" << std::endl;
    return 0;
  }

  // file from QAPLib
  EvalQAP eval(argv[1]);

  // random generator
  std::mt19937 gen( atoi(argv[2]) ); 

  // dimension of the problem
  n = eval.n;
  //std::cout << n << std::endl;

  // one solution
  Perm solution = new unsigned short[n];

  // random iniitalization
  randomPerm(gen, solution);

  // print solution
  //print(solution); std::cout << std::endl;
  
  // compute and print fitness value
  int fitness = eval(solution);
  //std::cout << fitness << std::endl;

  // test incremental evaluation
  int fitij, fitij_full ;
  int * deltas = new int[n*(n-1)/2];
  bool ok = true;
  for(unsigned i = 1; i < n; i++) {
    for(unsigned j = 0; j < i; j++) {
      
      deltas[index(i,j)] = eval.compute_delta(solution, i, j);
      fitij = fitness + deltas[index(i,j)];
      swap(solution, i, j);
      fitij_full = eval(solution);
      swap(solution, i, j);
      ok = ok && (fitij_full == fitij);
      std::cout << i << " " << j << " " << index(i, j) << " " << fitij_full << " " << fitij << " " << (fitij_full == fitij) << std::endl;
    }
  }

  // test double incremental evaluation
  int delta_ij, delta_ij_full ;
  bool ok2 = true;

  for(unsigned k = 1; k < n; k++) {
    for(unsigned l = 0; l < k; l++) {
      // mouvement (k,l) then update all delta values
      swap(solution, k, l);

      for(unsigned i = 1; i < n; i++) {
	for(unsigned j = 0; j < i; j++) {
	  delta_ij_full = eval.compute_delta(solution, i, j);
	  delta_ij = eval.compute_delta_fast(deltas[index(i,j)], solution, k, l, i, j);

	  ok2 = ok2 && (delta_ij_full == delta_ij);
	  std::cout << k << " " << l << " " << i << " " << j << " " << delta_ij_full << " " << delta_ij << " " << (delta_ij_full == delta_ij) << std::endl;
	  
	}
      }

      // back (k,l)
      swap(solution, k, l);
    }
  }
  std::cout << "delta eval ok:" << ok << std::endl;
  std::cout << "fast delta eval ok:" << ok2 << std::endl;

  delete [] deltas;
  return 1;
}
