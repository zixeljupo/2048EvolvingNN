#include "GeneticNetworkImplementation.h"

std::istream & operator>>(std::istream & stream, MutationParameters & mp) {
	return stream >> mp.flipRate >> mp.changeRate >> mp.additionRate
			>> mp.changeRange >> mp.addRange;
}

void GeneticNetworkImplementation::mutate(URNG & random) {

	std::vector<double *> code = getNetworkGeneticCode();

	const double fr = mutationParams.flipRate;
	const double cr = mutationParams.changeRate;
	const double ar = mutationParams.additionRate;
	const double frAndCr = fr + cr;
	const double frAndCrAndAr = fr + cr + ar;
	register double p;

	// if (fr + cr + ar > 1) might be an error...

	for (double * w : code) {

		p = random.next<double>();

		if (p >= frAndCrAndAr) {
			continue;
		}

		if (p < fr) { // flip
			*w = -*w;
		} else if (p < frAndCr) { // change value in range changeRange
			*w = random.next<double>(-mutationParams.changeRange, mutationParams.changeRange);
		} else { // add value in range addRange
			*w += random.next<double>(-mutationParams.addRange, mutationParams.addRange);
		}
	}
}

std::vector<double> GeneticNetworkImplementation::getNetworkGeneticCode() const {
	std::vector<double> code;
	for (const NeuralLayer & l : layers) {
		for (const Neuron & n : l.getNeurons()) {
			code.push_back(n.getBias());
			code.insert(std::end(code), std::begin(n.getWeights()), std::end(n.getWeights()));
		}
	}
	return code;
}

std::vector<double *> GeneticNetworkImplementation::getNetworkGeneticCode() {
	std::vector<double *> code;
	for (NeuralLayer & l : layers) {
		for (Neuron & n : l.getNeurons()) {
			code.push_back(&(n.getBias()));
			for (double & w : n.getWeights()) {
				code.push_back(&w);
			}
		}
	}
	return code;
}

std::pair<GeneticNetwork *, GeneticNetwork *>
		GeneticNetworkImplementation::crossingOver(const GeneticNetwork * nn, URNG & random) const {
	
	GeneticNetworkImplementation * c1 = new GeneticNetworkImplementation(this);
	GeneticNetworkImplementation * c2 = new
			GeneticNetworkImplementation((GeneticNetworkImplementation *)nn);

	std::vector<double *> codeVector1 = c1->getNetworkGeneticCode();
	std::vector<double *> codeVector2 = c2->getNetworkGeneticCode();

	register double ** code1 = codeVector1.data();
	register double ** code2 = codeVector2.data();

	register const int size = codeVector1.size();
	register const int splitPoint = random.next<int>(1, size - 1);

	for (register int i = 0; i < size; ++i) {
		if (std::abs(*(code1[i]) - *(code2[i])) > similarityValue) {
			if (i < splitPoint) {
				*(code1[i]) = *(code2[i]);
			} else {
				*(code2[i]) = *(code1[i]);
			}
		}
	}

	return std::make_pair(c1, c2);
}
