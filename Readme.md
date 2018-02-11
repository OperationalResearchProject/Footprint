# Footprint for QAP problem

## Required :

compile QAP Generators in folder QAPGenerators: 

``` gcc makeQAPuni.cc -o makeQAPuni -lm ```

``` gcc makeQAPrl.cc -o makeQAPrl -lm ```


compile Random walk in folder random_walk: 

``` g++ --std=c++11 randomWalk.cpp -I. -o build/randomWalk -lm ```
