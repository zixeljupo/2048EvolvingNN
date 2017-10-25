#pragma once

#include <vector>
#include <functional>

/**
 *  Module Activations
 *
 *	Activations is a module to deal with different neuron activation functions.
 * It provides ACTIVATIONS array to access activation functions. It can be indexed
 * via ActivationFunctionType enum. Each activation function returns single value -
 * activation value corresponding to input value x. Some additional parameters can
 * be passed to activation function with it's second argument of type
 * ActivationFunctionParameters, which is just an std::vector<double> of double
 * values.
 * 
 */

using ActivationFunction = std::function<double(const double, const std::vector<double> &)>;

enum ActivationFunctionType {
	IDENTITY = 0,
	SIGMOID = 1,
	RELU = 2,
	LRELU = 3,
};

/**
 * af_identity returns x as activation value and isn't affected by params vector.
 */
extern double af_identity(const double x, const std::vector<double> & params);

/**
 * af_sigmoid returns 1/(1 + exp(x)) as activation value and isn't affected by params vector.
 */
extern double af_sigmoid(const double x, const std::vector<double> & params);

/**
 * af_relu returns x if x>0 and zero otherwise.
 */
extern double af_relu(const double x, const std::vector<double> & params);

/**
 * af_lrelu returns x if x>0 and alpha*x otherwise. Alpha is usually a small number between
 * 0 and 0.5 that corresponds to function non-linearity. If params vector is not
 * empty, then alpha = params[0], otherwise alpha = 0.01 (default value).
 */
extern double af_lrelu(const double x, const std::vector<double> & params);

// by default there are no additional parameters for activation functions
const std::vector<double> DEFAULT_ACTIVATION_PARAMS = std::vector<double>();

const ActivationFunction ACTIVATIONS[] = { af_identity, af_sigmoid, af_relu, af_lrelu };
