/**************************************************************************
 *
 * Source code to extract the Local Optima Network (LON) of the QAP
 *
 * Source   : QAPnetwork.cpp
 * Language : C++
 * Author   : Sebastien Verel
 * Version  : 0.1
 * Date     : 07/30/2010
 *
 * Related publications:
 *
 * Daolio F., Tomassini M., Verel S., Ochoa G.
 * Communities of Minima in Local Optima Networks of Combinatorial Spaces
 * Physica A: Statistical Mechanics and its Applications, Volume 390, Issue 9, Pages 1684-1694, May 2011.
 *
 * Daolio F., Verel S., Ochoa G. & Tomassini M. (2010). 
 * Local Optima Networks of the Quadratic Assignement Problem. 
 * WCCI 2010 IEEE World Congress on Computational Intelligence (pp. 3145-3152). IEEE Press.
 *
 *
 * To compile:
 *   The GNU Scientific Library (GSL) is required. See the web site : http://www.gnu.org/s/gsl/ 
 * 
 *   Then type the classical commands:
 *      g++ -c qapNetwork.cpp -I.
 *      g++ -o qapNetwork qapNetwork.o -L. -lm -lgsl -lgslcblas
 *
 * To execute:
 *   ./qapNetwork instances/KCso7uni-1.dat out.nodes out.edges   
 *   or
 *   ./qapNetwork instances/KCso7uni-1.dat out.nodes out.edges out.fitness
 *
 * "instances/KCso7uni-1.dat" is an instance file name of the QAP problem to consider in QAPlib format
 * some instances are given in the directory instances
 *
 * "out.nodes" is the file with the id, - fitness, and the basin size of each node (local optimum) of the network :
 *   idNode1 fitness1 basinSize1
 *   idNode2 fitness2 basinSize2
 *   ....
 * warning: the computed fitness is the opposite fitness (- fitness) to tranform the QAP into a maximization problem
 *
 * "out.edges" is the file with weighted edges of the network:
 *   idNode_i1 idNode_j1 weight_ij1 idNode_j2 wij2 idNode_j3 weight_ij3 ...
 *   ....
 *
 *  which means that there is the edge (idNode_i1, idNode_j1) with the weight weight_ij1, etc.
 *
 *
 * Warnings: 
 *   In this version, the full enumeration of the search space is computed.
 *   The search space have to be 'small'.
 *
 * This code could be used for other permutation search space (TSP, Flow shop, etc.)
 *
 * Contact:
 *   Sebastien Verel, sebastien.verel@unice.fr
 *
 **************************************************************************/
#ifndef __evalQAP
#define __evalQAP

#include <fstream>

using namespace std;

/*
  class to compute the fitness function for the QAP 

  maximization problem, it's compute the opposite of the fitness value (negative value)
*/
class EvalQAP {
public:
  /*
    read instance from a file of QAPLib
  */
  EvalQAP(const char * fileData) {
    fstream file(fileData, ios::in);
    unsigned i, j, dim;

    if (file.is_open()) {
      file >> dim;
      file >> n;
      A = new int *[n];
      B = new int *[n];

      for(i = 0; i < n; i++) {
	A[i] = new int[n];
	for(j = 0; j < n; j++) {
	  file >> A[i][j];
	}
      }
	    
      for(i = 0; i < n; i++) {
	B[i] = new int[n];
	for(j = 0; j < n; j++) 
	  file >> B[i][j];
      }
	    
      file.close();
    } else
      cerr <<"FuncQap: impossible to open " << fileData << endl;
  };
    
  /*
    destructor
  */
  ~EvalQAP() {
    unsigned i;
	
    if (A != NULL) {
      for(i = 0; i < n; i++) 
	delete[] A[i];
      delete[] A;
    }

    if (B != NULL) {
      for(i = 0; i < n; i++) 
	delete[] B[i];
      delete[] B;
    }
  }
    
  /*
    full evaluation

    note that: Perm is a typedef of unsigned *
  */
  int operator()(const Perm & solution) { // evaluate the solution
    int cost = 0;

    for (int i = 0; i < n; i++)
      for (int j = 0; j < n; j++)
	cost += A[i][j] * B[solution[i]][solution[j]]; 
	
    return cost;
  }


  /*
    incremental evaluation 
    for a swap of i and j
    complexity: O(n)
  */
  int compute_delta(Perm p, int i, int j) {
    int d; 
    int k;

    d = (A[i][i]-A[j][j])*(B[p[j]][p[j]]-B[p[i]][p[i]]) +
      (A[i][j]-A[j][i])*(B[p[j]][p[i]]-B[p[i]][p[j]]);

    for (k = 0; k < n; k++) 
      if (k != i && k != j)
	d = d + (A[k][i]-A[k][j])*(B[p[k]][p[j]]-B[p[k]][p[i]]) +
	  (A[i][k]-A[j][k])*(B[p[j]][p[k]]-B[p[i]][p[k]]);
	
    return d;
  }

  /*
    double incremental evaluation for a swap of i2 and j2 after i and j
    compute the new delta values
    complexity : O(1) when {i,j} \cup {i2, j2} \not= \emptyset
    for ex. see E. Taillard, "COMPARISON OF ITERATIVE SEARCHES FOR THE QUADRATIC ASSIGNMENT PROBLEM", ECOLE PLOYTECHNIQUE FÉDÉRALE DE LAUSANNE (EPFL), 1994.
  */
  int compute_delta_fast(int delta, Perm p, int i, int j, int i2, int j2) {
    if (i2 == i && j2 == j)
      return -delta;
    else
      if (i2 == i || i2 == j || j2 == i || j2 == j)
	return compute_delta(p, i2, j2);
      else {
	return (delta 
		+ (A[i][i2] - A[i][j2] + A[j][j2] - A[j][i2]) * (B[p[j]][p[i2]] - B[p[j]][p[j2]] + B[p[i]][p[j2]] - B[p[i]][p[i2]])
		+ (A[i2][i] - A[j2][i] + A[j2][j] - A[i2][j]) * (B[p[i2]][p[j]] - B[p[j2]][p[j]] + B[p[j2]][p[i]] - B[p[i2]][p[i]]));
      }
  }

  // number of variables
  int n;

private:
  // matrix A
  int ** A;

  // matrix B
  int ** B;

};

#endif

