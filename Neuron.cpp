#include <sstream>

#include "Neuron.h"
#include "MLUtilities.h" // for scalar product

double Neuron::calculateInputSum(const std::vector<double> & in) const {

    if (in.size() != weights.size()) {
		std::stringstream errorMessageStream;
		errorMessageStream << "inputs count ( " << in.size() <<
				" ) does not match weights count ( " << weights.size() << " )";
		throw std::length_error(errorMessageStream.str());
    }

    return bias + (in & weights);
}
