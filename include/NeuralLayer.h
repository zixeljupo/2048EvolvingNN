#pragma once

#include "Neuron.h"

/**
 *  Class NeuralLayer
 *
 *	NeuralLayer class represents one layer of neural network that contains vector of
 * different neurons. All neurons in layer should have same inputs count. NeuralLayer
 * calculates vector of activation values of neurons for given input vector by passing
 * it to all neurons. So the output vector is given by formula:
 *     output[i] = neuron[i].calculateOutput(x).
 */

class NeuralLayer {

public:

    NeuralLayer() {}
	NeuralLayer(const std::vector<Neuron> & neuronsVector) { setNeurons(neuronsVector); }

	void setNeurons(const std::vector<Neuron> & neuronsVector);
	inline std::vector<Neuron> & getNeurons() { return neurons; }
	inline const std::vector<Neuron> & getNeurons() const { return neurons; }

	inline size_t size() const { return neurons.size(); }

    std::vector<double> calculateOutput(const std::vector<double> & input) const;

	virtual ~NeuralLayer() {}

protected:

    std::vector<Neuron> neurons;

};
