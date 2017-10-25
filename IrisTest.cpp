//#include "IrisTest.h"
//
//#include <istream>
//#include <vector>
//#include <string>
//#include <fstream>
//#include <algorithm>
//#include <sstream>
//#include <thread>
//
//#include "GeneticNetworkImplementation.h"
//#include "WeightsInitializer.h"
//#include "MLUtilities.h"
//
//typedef std::vector<std::pair<GeneticNetworkImplementation *, double>>  nscore;
//
//namespace {
//
//	std::vector<std::pair<std::vector<double>, std::vector<double>>> loadIrisDataset(std::istream & inputFile) {
//
//		const size_t dataSize = 150;
//		const size_t featuresCount = 4;
//		const size_t labelsCount = 3;
//
//		const char charMask[] = {'o', 's', 'g'};
//		const size_t charMaskLocation = 8;
//
//		std::vector<std::pair<std::vector<double>, std::vector<double>>> data;
//
//		for (size_t i = 0; i < dataSize; i++) {
//
//			data.push_back(make_pair(std::vector<double>(), std::vector<double>()));
//
//			double value;
//			char irisName[100];
//			char comma;
//
//			for (size_t j = 0; j < featuresCount; j++) {
//				inputFile >> value >> comma;
//				data[i].first.push_back(value);
//			}
//
//			inputFile >> irisName;
//			for (size_t j = 0; j < labelsCount; j++) {
//				data[i].second.push_back(irisName[charMaskLocation] == charMask[j]);
//			}
//		}
//
//		return data;
//	}
//}
//
//void calculateLosses(const std::vector<std::vector<double>> & _xTrain,
//					 const std::vector<std::vector<double>> & _yTrain,
//					 nscore * scores, const size_t start, const size_t size) {
//	for (size_t i = start; i < start + size; i++) {
//		double loss = 0;
//		for (size_t k = 0; k < _xTrain.size(); k++) {
//			auto y = scores->at(i).first->propogate(_xTrain[k]);
//			loss += lLoss(_yTrain[k], y, 4);
//		}
//		scores->at(i).second = loss;
//	}
//}
//
//void testIrisDataset() {
//
//	// agrorithm parameters
//	const int seed = 0;
//	const size_t population = 200;
//	const size_t layersCount = 5;
//	const size_t iterationsCount = 4000;
//	const auto activationFunction = LRELU;
//	const auto activationFunctionParams = std::vector<double>();
//	MutationParameters mp = {0, 0, 0.05, 0, 0.3};
//
//
//	std::ifstream irisFile;
//	irisFile.open("irises.txt");
//	if (!irisFile.is_open()) {
//		throw std::exception("can't open irises file");
//	}
//
//
//	auto data = loadIrisDataset(irisFile); // loading dataset
//
//	printf("Initial dataset:\n");
//	printDataset(data); // printing initial dataset
//
//	std::shuffle(data.begin(), data.end(), std::mt19937()); // shuffling dataset
//
//	printf("Shuffled dataset:\n");
//	printDataset(data); // printing shuffled dataset
//
//
//	size_t dataSize = data.size();
//	size_t inputSize = data[0].first.size();
//	size_t outputSize = data[0].second.size();
//
//
//	// initialize algorithm variables
//	URNG random(seed);
//	WeightsInitializingStrategy * wis = new WeightsInitializer();
//	std::vector<std::pair<GeneticNetworkImplementation *, double>> networksWithLoss;
//	const Topology topology = createDefaultTopology(inputSize, outputSize, layersCount,
//			make_pair(activationFunction, activationFunctionParams));
//
//	/*std::ifstream networksFile;
//	networksFile.open("NeuralNetworks40.txt");
//	for (size_t i = 0; i < population; i++) {
//		networksWithLoss.push_back(std::make_pair(
//			new GeneticNetworkImplementation(new NeuralNetwork(networksFile), random, mp), 0.0));
//	}
//
//	networksFile.close();*/
//
//	// creating initial population
//	for (size_t i = 0; i < population; i++) {
//		GeneticNetworkImplementation * gnn = new GeneticNetworkImplementation(topology, random, mp);
//		wis->initializeWeights(*gnn, random);
//		networksWithLoss.push_back(std::make_pair(gnn, 0.0));
//	}
//
//
//	// splitting dataset to train set and test set
//	std::vector<std::vector<double>> _xTrain, _yTrain, _xTest, _yTest;
//	for (size_t i = 0; i < dataSize; i++) {
//		if (i < dataSize/3*2) {
//			_xTrain.push_back(data[i].first);
//			_yTrain.push_back(data[i].second);
//		} else {
//			_xTest.push_back(data[i].first);
//			_yTest.push_back(data[i].second);
//		}
//	}
//
//	// genetic algorithm run
//	for (size_t step = 0; step < iterationsCount; step++) {
//
//		std::thread * threads[8];
//
//		// calculating fitness
//		for (size_t i = 0; i < 8; i++) {
//			threads[i] = new std::thread(calculateLosses, _xTrain, _yTrain,
//					&networksWithLoss, i*population/8, population/8);
//		}
//
//		for (size_t i = 0; i < 8; i++) {
//			threads[i]->join();
//		}
//
//		// sorting by fitness
//		sort(networksWithLoss.begin(), networksWithLoss.end(), lossComparator<NeuralNetwork*>());
//
//		// making selection and mutation
//		for (size_t i = 0; i < population/2; i++) {
//			size_t j = i + population/2;
//			delete networksWithLoss[j].first;
//			networksWithLoss[j].first = new GeneticNetworkImplementation(networksWithLoss[i].first);
//
//
//			mp = readMutationParameters();
//			networksWithLoss[i].first->setMutationParams(mp);
//			networksWithLoss[j].first->setMutationParams(mp);
//
//
//			networksWithLoss[i].first->mutate();
//			networksWithLoss[j].first->mutate();
//		}
//
//		// printing infos
//		printf("Generation %d, loss %lf\n", step, networksWithLoss[0].second);
//
//		if (step % 10 == 0) {
//			printf("\nTrain accuracy: %.1lf\n", calculateAccuracy(networksWithLoss[0].first, _xTrain, _yTrain));
//			printf("\nTest accuracy: %.1lf\n", calculateAccuracy(networksWithLoss[0].first, _xTest, _yTest));
//		}
//		if (step % 10 == 0) {
//
//			std::stringstream ss;
//			ss << "NeuralNetworks" << step << ".txt";
//			std::ofstream networksFile;
//			networksFile.open(ss.str());
//
//			for (size_t i = 0; i < population; i++) {
//				networksWithLoss[i].first->saveNetwork(networksFile, false);
//			}
//
//			networksFile.close();
//		}
//	}
//
//	printf("\nFinal train accuracy: %.1lf\n", calculateAccuracy(networksWithLoss[0].first, _xTrain, _yTrain));
//	printf("\nFinal test accuracy: %.1lf\n", calculateAccuracy(networksWithLoss[0].first, _xTest, _yTest));
//	getchar(); getchar();
//}
