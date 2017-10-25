#include <sstream>

#include "NeuralLayer.h"

namespace {
	bool neuronsAreSameSize(const std::vector<Neuron> & neuronsVector) {
		if (neuronsVector.size() < 2) {
			return true;
		}
		const size_t neuronSize = neuronsVector[0].size();
		for (Neuron n : neuronsVector) {
			if (n.size() != neuronSize) {
				return false;
			}
		}
		return true;
	}
}

void NeuralLayer::setNeurons(const std::vector<Neuron> & neuronsVector) {
	if (!neuronsAreSameSize(neuronsVector)) {
		throw std::length_error("vector contains neurons with different sizes");
	}
	neurons = neuronsVector;
}

std::vector<double> NeuralLayer::calculateOutput(const std::vector<double> & input) const {
	std::vector<double> output;
	for (const Neuron & n : neurons) {
		output.push_back(n.calculateOut(input));
	}
	return output;
}
