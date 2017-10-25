#pragma once

#include "WeightsInitializingStrategy.h"

class WeightsInitializer : public WeightsInitializingStrategy {
public:
	void initializeWeights(NeuralNetwork & nn, URNG & gen) const override;
};
