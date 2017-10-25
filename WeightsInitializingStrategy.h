#pragma once

#include "NeuralNetwork.h"
#include "URNG.h"

class WeightsInitializingStrategy {
public:
	virtual void initializeWeights(NeuralNetwork & nn, URNG & gen) const = 0;
	virtual ~WeightsInitializingStrategy() {}
};
