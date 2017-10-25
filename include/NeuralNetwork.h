#pragma once

#include <iostream>

#include "Neuron.h"
#include "NeuralLayer.h"

/**
 *  Class NeuralNetwork
 *
 *	NeuralNetwork class represents complex non-linear transormation over input vector to
 * output vector. It consists of one or more neural layers.
 *
 *  NeuralNetwork calculate it's output vector by propagating input values through all
 * layers. First the input is sent to the first layer of neural network, which calculates
 * it's output, which is passed as input for second layer, and so on. Finally the last
 * layer returns vector of double values as all neural network output.
 *
 *  With such calculation scheme inputs count of every layer must match outputs count for
 * previous layer, and first layer inputs count is whole neural network inputs count, and
 * last layer output is whole network output.
 *
 *	NeuralNetwork can be created in three ways:
 *   1) by copy constructor
 *   2) read all network data from istream
 *   3) create network with computational topology (and it will have unitialized weights)
 *
 *  Computational topology contains such information as layers count, neurons count in
 * each layer, and all parameters of every neuron except weights and biases.
 *
 *  Weights and biases are actually main part of neural network. By changing them you could
 * approximate every possible continuous function on bounded interval well (if you have
 * enough layers, search for meme "stack more layers").
 *
 *  To find appropriate parameters of weights and biases researchers usually use
 * backpropagation algorithm, but here we won't write code for it.
 */

using ActivationFunctionConfiguration =
		std::pair<ActivationFunctionType, std::vector<double>>;
using LayerConfiguration = std::vector<ActivationFunctionConfiguration>;
using NetworkConfiguration = std::vector<LayerConfiguration>;

struct Topology {
	Topology(const size_t _inputsCount) : inputsCount(_inputsCount),
			neuronConfigurations(NetworkConfiguration()) {}
	Topology(const NetworkConfiguration & _neuronConfigurations, const size_t _inputsCount) :
			neuronConfigurations(_neuronConfigurations), inputsCount(_inputsCount) {}
	NetworkConfiguration neuronConfigurations;
	size_t inputsCount;
};

extern Topology createDefaultTopology(
		const size_t inputSize, const size_t outputSize = 1, const size_t layersCount = 2,
		const ActivationFunctionConfiguration & afc =
		std::make_pair(SIGMOID, DEFAULT_ACTIVATION_PARAMS));

extern Topology createEmptyTopology(const size_t inputSize);

extern void addLayer(Topology & topology, const size_t layerSize,
		const ActivationFunctionConfiguration & afc);

class NeuralNetwork {

public:

	NeuralNetwork(std::istream & stream, const bool binary);
	NeuralNetwork(const Topology & topology);
	NeuralNetwork(const NeuralNetwork * nn);

	inline std::vector<NeuralLayer> & getLayers() { return layers; }
	inline const std::vector<NeuralLayer> & getLayers() const { return layers; }

	inline size_t size() const { return layers.size(); }

	inline size_t getInputsCount() const {
		return layers[0].getNeurons()[0].size();
	}

	inline size_t getOutputsCount() const {
		return layers[size() - 1].size();
	}

	size_t getNeuronsCount() const;
	size_t getHiddenNeuronsCount() const;

	// TODO return const keyword back
	double * propogate(std::vector<double> & input);

	virtual void saveNetwork(std::ostream & stream, const bool binary) const;

	virtual ~NeuralNetwork() {}

protected:

	std::vector<NeuralLayer> layers;
	double ** layerOutput; // TODO remove this

};
