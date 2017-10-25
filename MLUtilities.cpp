#include "MLUtilities.h"

#include "GeneticNetworkImplementation.h"

//double calculateAccuracy(NeuralNetwork * nn, // return const to NeuralNetwork
//						 const std::vector<std::vector<double>> & _x,
//						 const std::vector<std::vector<double>> & _y) {
//	double accuracy = 0;
//	for (size_t k = 0; k < _x.size(); k++) {
//		auto y = nn->propogate(_x[k]);
//		accuracy += _y[k][argmax(y)];
//	}
//	return accuracy / _x.size() * 100;
//}

void printNeuralNetwork(const NeuralNetwork * nn) {
	auto & layers = nn->getLayers();
	for (auto & l : layers) {
		for (auto n : l.getNeurons()) {
			printf("Neuron: s: %.2lf     w:", n.getBias());
			for (auto w : n.getWeights()) {
				printf(" %.2lf ", w);
			}
			printf("\n");
		}
		printf("\n");
	}
}

MutationParameters readMutationParameters() {

	std::ifstream file;
	file.open("2048Configuration.txt");

	MutationParameters mp;
	file >> mp.flipRate;
	file >> mp.changeRate;
	file >> mp.additionRate;
	file >> mp.changeRange;
	file >> mp.addRange;

	file.close();

	return mp;
}

void printDataset(const std::vector<std::pair<std::vector<double>, std::vector<double>>> & dataset) {
	for (size_t i = 0; i < dataset.size(); i++) {

		printf("Example %d: ", i);

		size_t inSize = dataset[i].first.size();
		size_t outSize = dataset[i].second.size();

		for (size_t j = 0; j < inSize; j++) {
            printf("%.1lf ", dataset[i].first[j]);
        }

        printf("     ");

        for (size_t j = 0; j < outSize; j++) {
            printf("%.0lf ", dataset[i].second[j]);
        }

        printf("\n");
    }
}