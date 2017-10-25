#pragma once

#include "Activations.h"

/**
 *  Class Neuron
 *
 *	Neuron class represents one computational unit of neural network. It has some
 * parameters called weights, and he transforms input vector to a row number named
 * activation value. Neuron also has bias unit which is single double value.
 *
 *	Neuron calculates activation value by given formula:
 *     output = ActivationFunction(bias + (input, weights), ActivationFunctionParams),
 * where (input, weights) is a scalar product of vectors input and weights. For
 * example, if we have ReLU activation, our input is (-1, 2, 0), weight vector is
 * (5, 0, -5), and shift is 1, neuron will calculate:
 *     output = ReLU(1 + [(-1)*5 + 2*0 + 0*5]) = ReLU(-4) = 0.

 *  Neuron can also calculate it's weighted input sum by formula:
 *     inputSum = bias + (input, weights).
 */

class Neuron {

public:

	Neuron(const size_t size = 1,
		   const ActivationFunctionType aft = RELU,
		   const std::vector<double> & afp = DEFAULT_ACTIVATION_PARAMS) :
		   functionId(aft), functionParameters(afp),
		   weights(std::vector<double>(size, 0)), bias(0) {}

    Neuron(const std::vector<double> & weights, const double bias = 0,
		   const ActivationFunctionType aft = RELU,
		   const std::vector<double> & afp = std::vector<double>()) :
		   functionId(aft), functionParameters(afp), weights(weights), bias(bias) {}

	inline void setActivationFunctionParams(const std::vector<double> & params)
			{ functionParameters = params; }

	inline const std::vector<double> & getActivationFunctionParams() const
			{ return functionParameters; }

	inline void setWeights(const std::vector<double> & w) { weights = w; }
	inline std::vector<double> & getWeights() { return weights; }
	inline const std::vector<double> & getWeights() const { return weights; }

	inline void setBias(const double b) { bias = b; }
	inline double getBias() const { return bias; }
	inline double & getBias() { return bias; }

	inline ActivationFunctionType getActivationFunction() const { return functionId; }
	inline size_t size() const { return weights.size(); }

	double calculateInputSum(const std::vector<double> & in) const;
	inline double calculateOut(const std::vector<double> & in) const
			{ return calculateOut(calculateInputSum(in)); }

	virtual ~Neuron() {};

protected:

    ActivationFunctionType functionId;
	std::vector<double> functionParameters;

	double bias;
    std::vector<double> weights;

	inline double calculateOut(const double inputSum) const {
		return ACTIVATIONS[functionId](inputSum, functionParameters);
	}

};
