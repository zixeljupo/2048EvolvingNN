#include <algorithm>
#include <sstream>
#include <thread>
#include <experimental/filesystem>

#include "Evolve2048Networks.h"

#include "URNG.h"
#include "GeneticNetworkImplementation.h"
#include "WeightsInitializer.h"
#include "MLUtilities.h"
#include "2048Clever.h"

namespace {

	using ScoredNetworks = std::vector<std::pair<GeneticNetworkImplementation *, int>>;

	struct CreateConfiguration {

		long long seed;
		size_t population;

		// no role now for those variables
		size_t inputsCount;
		size_t outputsCount;
		size_t layersCount;
		ActivationFunctionType activationFunction;
		std::vector<double> activationFunctionParams;
	};

	struct AlgorithmConfiguration {

		long long seed;
		size_t threadsCount;

		size_t averagingParameter;
		MutationParameters mp;
		bool exit;
	};

	const bool CREATE_NEW = false;
	const CreateConfiguration createConf = {322, 2048, 256, 1, 2, LRELU, DEFAULT_ACTIVATION_PARAMS};
	const AlgorithmConfiguration algorithmConf = {228, 4, 8, DEFAULT_MUTATION_PARAMS, false};

	void readConfiguration(AlgorithmConfiguration & config) {

		std::fstream file;
		file.open("2048Configuration.txt", std::ios::in | std::ios::out);

		if (!file.is_open()) {
			throw std::runtime_error("Can't read configuration file.\n");
		}

		std::string isFinish;
		file >> config.mp;
		file >> config.averagingParameter;
		file >> isFinish;

		if (isFinish == "exit") {
			file.seekg(-4, std::ios::cur);
			file << "    ";
			config.exit = true;
		}

		file.close();
	}

	void createNetworks(ScoredNetworks & nets, const Topology & topology,
						URNG & random, URNG & createRandom) {
		printf("Creating initial population...\n");
		WeightsInitializingStrategy * wis = new WeightsInitializer();
		for (size_t i = 0; i < nets.size(); ++i) {
			auto * gnn = new GeneticNetworkImplementation(topology);
			wis->initializeWeights(*gnn, createRandom);
			nets[i] = std::make_pair(gnn, 0);
			printf("."); // status bar
		}
		printf("\nPopulation created.\n");
	}

	void loadNetworks(ScoredNetworks & nets, const char * prefix, URNG & random) {

		printf("Loading networks from file...\n");

		//std::string filename(prefix);
		//for (auto & p : std::experimental::filesystem::directory_iterator(""))
		//	std::cout << p << std::endl;

		std::ifstream networksFile(prefix, std::ios::in | std::ios::binary);

		if (!networksFile.is_open()) {
			throw std::runtime_error("Can't open networks file.\n");
		}

		size_t population;
		networksFile.read((char *)&population, sizeof(population));
		
		nets = ScoredNetworks(population);
		for (size_t i = 0; i < population; ++i) {
			nets[i] = std::make_pair(
				new GeneticNetworkImplementation(new NeuralNetwork(networksFile, true)), 0
			);
			printf("."); // status bar
		}
		networksFile.close();

		printf("\nNets loaded.\n");
	}

	void saveNetworks(const ScoredNetworks & nets, const size_t step) {

		size_t population = nets.size();

		printf("Exit parameter passed. Saving nets.\n");

		std::stringstream ss;
		ss << "ValueNetworks/LightNetwork1_" << step;
		std::ofstream networksFile(ss.str(), std::ios::out | std::ios::binary);

		networksFile.write((char *)&population, sizeof(population));
		for (size_t i = 0; i < population; ++i) {
			nets[i].first->saveNetwork(networksFile, true);
			printf("."); // status bar
		}
		networksFile.close();

		printf("\nNets saved. Exit.\n");
	}

	void calculateScore(ScoredNetworks * networksWithScore,
						const size_t threadNumber, const size_t threadsCount,
						const size_t averagingParameter, URNG random) {
		size_t population = networksWithScore->size();
		size_t start = population/threadsCount*threadNumber;
		size_t end = start + population/threadsCount;
		for (size_t i = start; i < end; ++i) {

			//// best score
			//int bestScore = fastPlayGame(networksWithScore->at(i).first, random);
			//for (size_t j = 1; j < averagingParameter; ++j) {
			//	bestScore = std::max(bestScore, fastPlayGame(networksWithScore->at(i).first, random));
			//}
			//networksWithScore->at(i).second = bestScore;
			//printf(".");

			// average score
			int avScore = 0;
			for (size_t j = 0; j < averagingParameter; ++j) {
				avScore += fastPlayGame(networksWithScore->at(i).first, random);
			}
			networksWithScore->at(i).second = avScore/averagingParameter;
			if (i % 10 == 0) printf(".");

			//// median score or some combination of scores for some quantiles
			//std::vector<int> scores;
			//for (size_t j = 0; j < averagingParameter; ++j) {
			//	//scores.push_back(playGame(networksWithScore->at(i).first, random));
			//	scores.push_back(fastPlayGame(networksWithScore->at(i).first, random));
			//}
			//std::sort(scores.begin(), scores.end());
			////networksWithScore->at(i).second = (scores[averagingParameter / 2]*5 +
			////		scores[averagingParameter - 1] + scores[0])/7; // some heuristic search for achieving 2048
			//networksWithScore->at(i).second = scores[averagingParameter / 2];
			//printf(".");
		}
	}

	inline void calculateFitness(std::vector<std::thread> & threads, ScoredNetworks & nets,
						  const size_t averagingParameter, URNG & random) {
		size_t threadsCount = threads.size();
		for (size_t t = 0; t < threadsCount; ++t) {
			threads[t] = std::thread(calculateScore, &nets,
					t, threadsCount, averagingParameter, random);
		}
		for (size_t t = 0; t < threadsCount; ++t) {
			threads[t].join();
		}
	}
}

void evolve() {

	AlgorithmConfiguration algConf = algorithmConf;
	std::vector<std::thread> threads(algConf.threadsCount);
	ScoredNetworks networksWithScore;
	URNG random(algConf.seed);

	if (CREATE_NEW) {
		URNG createRandom(createConf.seed);
		networksWithScore = ScoredNetworks(createConf.population);
		//const Topology topology = createDefaultTopology(
		//	createConf.inputsCount, createConf.outputsCount, createConf.layersCount,
		//	std::make_pair(createConf.activationFunction, createConf.activationFunctionParams)
		//);

		// this is neural network initial structure
		Topology topology(createConf.inputsCount);
		auto neuronsActivation = std::make_pair(
				createConf.activationFunction, createConf.activationFunctionParams);
		addLayer(topology, 64, neuronsActivation);
		addLayer(topology, 16, neuronsActivation);
		addLayer(topology, 1,  neuronsActivation); // output layer (one value to say how good is position)
		createNetworks(networksWithScore, topology, random, createRandom);
	} else {
		loadNetworks(networksWithScore, "ValueNetworks/LightNetwork1_142", random);
	}

	// genetic algorithm run
	for (size_t step = 142; true; ++step) {

		unsigned long long start = std::chrono::high_resolution_clock::now().time_since_epoch().count();

		readConfiguration(algConf);
		if (algConf.exit) {
			saveNetworks(networksWithScore, step);
			break;
		}

		calculateFitness(threads, networksWithScore, algConf.averagingParameter, random);
		std::sort(networksWithScore.begin(), networksWithScore.end(), scoreComparator<NeuralNetwork*>());

		size_t population = networksWithScore.size();
		for (size_t i = population/8; i < population; ++i) {
			delete networksWithScore[i].first;
		}

		// Crossingover
		for (size_t i = 0; i < population/8; i += 2) {
			for (size_t j = 1; j < 8; ++j) {
				auto children = networksWithScore[i].first->crossingOver(networksWithScore[i + 1].first, random);
				networksWithScore[i + population/8*j].first = (GeneticNetworkImplementation *)children.first;
				networksWithScore[i + 1 + population/8*j].first = (GeneticNetworkImplementation *)children.second;
			}
		}

		//// Replace with copies
		//for (size_t i = 0; i < population/8; ++i) {
		//	for (size_t j = 1; j < 8; ++j) {
		//		networksWithScore[i + population/8*j].first = new GeneticNetworkImplementation(
		//			(GeneticNetworkImplementation*)networksWithScore[i].first
		//		);
		//	}
		//}

		// Mutate
		for (size_t i = 0; i < population; ++i) {
			networksWithScore[i].first->setMutationParams(algConf.mp);
			networksWithScore[i].first->mutate(random);
		}

		printf("\nGeneration %d: best %d, median %d, worst %d.\n",
				step,
				networksWithScore[0].second,
				networksWithScore[population/2].second,
				networksWithScore[population - 1].second);

		unsigned long long end = std::chrono::high_resolution_clock::now().time_since_epoch().count();
		printf("Time elapsed: %d ms.\n", int((end - start)/1e6));
	}
}
