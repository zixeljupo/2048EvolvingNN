#pragma once

// DEPRECATED CODE

#include <vector>

#include "URNG.h"
#include "NeuralNetwork.h"

int playMonteCarlo(std::vector<NeuralNetwork *> & nns, URNG & random);
int playStupidGame(URNG & random);
void init2048();
int playGame(NeuralNetwork * nn, URNG & random); // return const to neuralNetwork
