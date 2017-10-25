#pragma once

#include "NeuralNetwork.h"
#include "URNG.h"

/**
 *  Class GeneticNetwork
 *
 *	GeneticNetwork is an abstract class that extends NeuralNetwork. Genetic networks are
 * used when you try to find appropriate weights and biases by genetic algorithm.
 *
 *  It has four pure virtual methods that should be implemented to use genetic algorithm
 * over neural networks correctly.
 *
 * 1)   mutate method causes genetic network to change it's weights and biases randomly.
 *
 * 2-3) getNetworkGeneticCode returns representation of neural network with vector of
 *      double values. Typically, this method just returns all weights and biases of
 *      neural network. It should have two implementations: vector of values and vector
 *      of pointers to weights and biases in neural network. With the second method
 *      we could easily modify network genetic code.
 *
 * 4)   crossingOver method returns one child of two neural networks with same structure,
 *      but different weights and biases based on it's parent's weights and biases.
 */

class GeneticNetwork : public NeuralNetwork {

public:

	GeneticNetwork(const Topology & topology) : NeuralNetwork(topology) {}
	GeneticNetwork(const GeneticNetwork * gnn) : NeuralNetwork(gnn) {}
	GeneticNetwork(const NeuralNetwork * nn) : NeuralNetwork(nn) {}

	virtual std::vector<double> getNetworkGeneticCode() const = 0;
	virtual std::vector<double *> getNetworkGeneticCode() = 0;
	virtual void mutate(URNG & random) = 0;
	virtual std::pair<GeneticNetwork *, GeneticNetwork *>
			crossingOver(const GeneticNetwork * nn, URNG & random) const = 0;

	virtual ~GeneticNetwork() {}
};
