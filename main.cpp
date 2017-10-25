#include <iostream>
#include <algorithm>
#include <fstream>

#include "GeneticNetworkImplementation.h"
#include "URNG.h"
#include "Evolve2048Networks.h"
#include "2048.h"
#include "IrisTest.h"

#include "2048Clever.h"

using namespace std;

//void testURNG() {
//  
//	vector<int> seeds;
//	seeds.push_back(1);
//
//	URNG urng;
//	URNG urng0(1);
//	URNG urng1(seeds);
//
//	auto a = urng.next(1, 10);
//    auto b = urng.next(1.0, 2.0);
//    auto c = urng.next(1000000000000000, 2000000000000000);
//    auto d = urng.next<char>();
//    auto e = urng.next<short>();
//    auto f = urng.next<int>();
//    auto g = urng.next<long long>();
//    auto h = urng.next<unsigned char>();
//    auto i = urng.next<unsigned short>();
//    auto j = urng.next<unsigned>();
//    auto k = urng.next<unsigned long long>();
//    auto l = urng.next<bool>();
// 
//	cout << "random " << typeid(a).name() << ": " << a << endl;
//    cout << "random " << typeid(b).name() << ": " << b << endl;
//    cout << "random " << typeid(c).name() << ": " << c << endl;
//	cout << "random " << typeid(d).name() << ": " << d << endl;
//	cout << "random " << typeid(e).name() << ": " << e << endl;
//	cout << "random " << typeid(f).name() << ": " << f << endl;
//    cout << "random " << typeid(g).name() << ": " << g << endl;
//    cout << "random " << typeid(h).name() << ": " << h << endl;
//    cout << "random " << typeid(i).name() << ": " << i << endl;
//    cout << "random " << typeid(j).name() << ": " << j << endl;
//    cout << "random " << typeid(k).name() << ": " << k << endl;
//    cout << "random " << typeid(l).name() << ": " << l << endl;
//}

#include "WeightsInitializer.h"
#include <thread>

double mulx(double initial, const double * v1, const double * v2) {
	for (int k = 0; k < 258; ++k) {
		initial += v1[k]*v2[k];
	}
	return initial;
}

void fuckProcessor() {
	double k = 0, u = 1;
	double v = 0;
	for (int i = 0; i < 1000000000; i++) {
		for (int j = 0; j < 1000000000; j++) {
			u += 1;
			k += 1;
			u /= k;
			k /= 3;
			u += k;
			k += u;
			v += k*u;
		}
	}
}

void checkPerformance() {

	init_tables();

	std::ifstream networksFile("ValueNetworks/LightNetwork1_142", std::ios::in | std::ios::binary);
	if (!networksFile.is_open()) {
		throw std::runtime_error("Can't open networks file.\n");
	}
	size_t population;
	networksFile.read((char *)&population, sizeof(population));
	auto * gnn = new GeneticNetworkImplementation(new NeuralNetwork(networksFile, true));
	networksFile.close();

	std::ofstream analysisFile("Analyzed data.txt", std::ios::out);
	for (size_t i = 0; i < 10000; i++) {
		analysisFile << fastPlayGame(gnn, URNG(i)) << std::endl;
		printf("\r%d", i);
	}
	analysisFile.close();
}

int main() {

	//mt19937 g;
	//vector<int> seedVec;
	//seed_seq seeds;

	//seedVec = vector<int>(2, 3);
	//seeds.generate(seedVec.begin(), seedVec.end());
	//g.seed(seedVec);
	//cout << uniform_int_distribution<int>(0, 1000)(g) << " ";

	//seedVec = vector<int>(2, 2);
	//seeds.generate(seedVec.begin(), seedVec.end());
	////g.seed(seedVec);
	//cout << uniform_int_distribution<int>(0, 1000)(g) << " ";

	//return 0;

	//std::vector<std::thread> threads(128);
	//int threadsCount = threads.size();
	//for (int t = 0; t < threadsCount; ++t) {
	//	threads[t] = std::thread(fuckProcessor);
	//}
	//for (int t = 0; t < threadsCount; ++t) {
	//	threads[t].join();
	//}

	//vector<double> v1(257);
	//vector<double> v2(257);
	//URNG gen;
	//for (size_t i = 0; i < v1.size(); i++) {
	//	v1[i] = gen.next<double>(0, 1);
	//}
	//for (size_t i = 0; i < v2.size(); i++) {
	//	v2[i] = gen.next<double>(0, 1);
	//}
	//int iterCount = 10000;
	//int loopCount = 258*3;

	//double v = 0;

	//unsigned long long start = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	//for (int i = 0; i < iterCount; ++i) {
	//	for (int j = 0; j < loopCount; j++) {
	//		v += mulx(1, v1.data(), v2.data());
	//	}
	//}
	//unsigned long long end = std::chrono::high_resolution_clock::now().time_since_epoch().count();

	//printf("%d microseconds per network\n", (int)((end - start)/iterCount));
	//printf("%lf\n", v);

	//testIrisDataset();

	//init2048();
	init_tables();
	evolve();
	//play_game(find_best_move);


	//size_t population;
	//std::vector<NeuralNetwork *> nns(8);

	//std::ifstream networksFile("ValueNetworks/Network_257_in_194", std::ios::binary);
	//if (!networksFile.is_open()) {
	//	printf("Networks not loaded!");
	//	return -1;
	//}
	//networksFile.read((char *)&population, sizeof(population));
	//nns[0] = new GeneticNetworkImplementation(new NeuralNetwork(networksFile, true), URNG());
	//networksFile.close();

	//const Topology topology = createDefaultTopology(
	//	257, 1, 4,
	//	std::make_pair(LRELU, std::vector<double>())
	//);

	//URNG gen(1);
	//WeightsInitializingStrategy * wis = new WeightsInitializer();
	//auto * gnn = new GeneticNetworkImplementation(topology, gen);
	//wis->initializeWeights(*gnn, gen);

	//vector<double> x(257);
	//for (size_t i = 0; i < x.size(); i++) {
	//	x[i] = gen.next<double>(0, 1);
	//}

	//printf("%lf\n", gnn->propogate(x)[0]);

	//int iterCount = 10000;

	//unsigned long long start = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	//for (int i = 0; i < iterCount; ++i) {
	//	gnn->propogate(x);
	//}
	//unsigned long long end = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	//printf("%d microseconds per network\n", (int)((end - start)/iterCount));

	//for (size_t i = 1; i < 8; i++) {
	//	nns[i] = new GeneticNetworkImplementation(
	//			new NeuralNetwork((GeneticNetworkImplementation *)nns[0]), URNG());
	//}

	//printf("Loaded.\n");
	//fastMonteCarlo(nns, URNG());






	//init_tables();

	//printf("Loading...\n");

	//size_t population;
	//std::vector<NeuralNetwork *> nns(8);

	//std::ifstream networksFile("ValueNetworks/LightNetwork_1930", std::ios::binary);
	//if (!networksFile.is_open()) {
	//	printf("bad\n");
	//}
	//networksFile.read((char *)&population, sizeof(population));
	//nns[0] = new GeneticNetworkImplementation(new NeuralNetwork(networksFile, true));
	//networksFile.close();

	//for (size_t i = 1; i < 8; i++) {
	//	nns[i] = new GeneticNetworkImplementation(
	//			new NeuralNetwork((GeneticNetworkImplementation *)nns[0]));
	//}

	//printf("Loaded.\n");

	//fastMonteCarlo(nns, URNG());

	//checkPerformance();

	return 0;
}
