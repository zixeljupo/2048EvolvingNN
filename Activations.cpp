#include "Activations.h"

const double DEFAULT_LEAK_PARAMETER = 0.01;

double af_identity(const double x, const std::vector<double> & params) {
	return x;
}

double af_sigmoid(const double x, const std::vector<double> & params) {
	return 1/(1 + std::exp(-x));
}

double af_relu(const double x, const std::vector<double> & params) {
	return x > 0 ? x : 0;
}

double af_lrelu(const double x, const std::vector<double> & params) {
	if (params.size() == 0) {
		return x > 0 ? x : x*DEFAULT_LEAK_PARAMETER;
	}
	return x > 0 ? x : x*params[0];
}
