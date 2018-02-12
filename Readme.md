# Footprint for QAP problem

## Requirements :

 - compile QAP Generators in folder QAPGenerators: 

``` gcc makeQAPuni.cc -o makeQAPuni -lm ```

``` gcc makeQAPrl.cc -o makeQAPrl -lm ```


 - compile Random walk in folder random_walk: 

``` g++ --std=c++11 randomWalk.cpp -I. -o build/randomWalk -lm ```

## How use the generator ?

``` python main.py instance_size number_of_instance_wanted ```