#include <sstream>
#include <exception>

#include "NeuralNetwork.h"

Topology createDefaultTopology(const size_t inputSize, const size_t outputSize, const size_t layersCount,
		const ActivationFunctionConfiguration & afc) {
	
	if (layersCount == 0 || outputSize == 0) { // neural network with no inputs is avaliable
		throw std::invalid_argument("topology has no layers or no outputs");
	}

	NetworkConfiguration nc = NetworkConfiguration(
			layersCount - 1, LayerConfiguration(inputSize + 1, afc));
	nc.push_back(LayerConfiguration(outputSize, afc));
	return Topology(nc, inputSize);
}

Topology createEmptyTopology(const size_t inputSize) {
	return Topology(inputSize);
}

void addLayer(Topology & topology, const size_t layerSize,
		const ActivationFunctionConfiguration & afc) {
	topology.neuronConfigurations.push_back(LayerConfiguration(layerSize, afc));
}

NeuralNetwork::NeuralNetwork(const NeuralNetwork * nn) {
	layers = nn->layers;

	// TODO remove this
	layerOutput = (double **)malloc((layers.size() + 1)*sizeof(double *));
	layerOutput[0] = (double *)malloc(getInputsCount()*sizeof(double));
	for (size_t i = 1; i <= layers.size(); ++i) {
		layerOutput[i] = (double *)malloc(layers[i - 1].size()*sizeof(double));
	}
}

NeuralNetwork::NeuralNetwork(std::istream & stream, const bool binary) {

	register double bias;
	register int functionId;
	register size_t layersCount, neuronsCount, weightsCount, paramsCount;
	register size_t l, n;

	std::vector<double> weights;
	ActivationFunctionType function;
	std::vector<double> params;
	std::vector<Neuron> neuronsInLayer;

	if (binary) {

		stream.read((char*)&layersCount, sizeof(layersCount));
		layers = std::vector<NeuralLayer>(layersCount);

		for (l = 0; l < layersCount; ++l) {

			stream.read((char *)&neuronsCount, sizeof(neuronsCount));
			neuronsInLayer = std::vector<Neuron>(neuronsCount);

			for (n = 0; n < neuronsCount; ++n) {

				stream.read((char *)&functionId, sizeof(functionId));
				stream.read((char *)&bias, sizeof(bias));
				stream.read((char *)&paramsCount, sizeof(paramsCount));
				stream.read((char *)&weightsCount, sizeof(weightsCount));

				function = ActivationFunctionType(functionId);
				params = std::vector<double>(paramsCount);
				weights = std::vector<double>(weightsCount);

				if (paramsCount != 0) {
					stream.read((char *)params.data(), sizeof(double)*paramsCount);
				}
				if (weightsCount != 0) {
					stream.read((char *)weights.data(), sizeof(double)*weightsCount);
				}

				neuronsInLayer[n] = Neuron(weights, bias, function, params);
			}
			layers[l] = NeuralLayer(neuronsInLayer);
			neuronsInLayer.clear();
		}

	} else {

		stream >> layersCount;
		layers = std::vector<NeuralLayer>(layersCount);

		for (l = 0; l < layersCount; ++l) {

			stream >> neuronsCount;
			neuronsInLayer = std::vector<Neuron>(neuronsCount);

			for (n = 0; n < neuronsCount; ++n) {

				stream >> functionId >> bias >> paramsCount >> weightsCount;

				function = ActivationFunctionType(functionId);
				params = std::vector<double>(paramsCount);
				weights = std::vector<double>(weightsCount);

				register size_t i;
				for (i = 0; i < params.size(); ++i) {
					stream >> params[i];
				}
				for (i = 0; i < weights.size(); ++i) {
					stream >> weights[i];
				}

				neuronsInLayer[n] = Neuron(weights, bias, function, params);
			}
			layers[l] = NeuralLayer(neuronsInLayer);
			neuronsInLayer.clear();
		}
	}

	// TODO remove this
	layerOutput = (double **)malloc((layers.size() + 1)*sizeof(double *));
	layerOutput[0] = (double *)malloc(getInputsCount()*sizeof(double));
	for (size_t i = 1; i <= layers.size(); ++i) {
		layerOutput[i] = (double *)malloc(layers[i - 1].size()*sizeof(double));
	}
}

NeuralNetwork::NeuralNetwork(const Topology & topology) {

	if (topology.neuronConfigurations.size() == 0) { // networks with no layers are not avaliable
		throw std::invalid_argument("topology has no layers");
	}

	for (size_t i = 0; i < topology.neuronConfigurations.size(); i++) {

		const auto layerConfig = topology.neuronConfigurations[i];

		if (layerConfig.size() == 0) { // layers with no neurons are not avaliable
			throw std::invalid_argument("topology contains flat layer");
		}

		const size_t neuronSize = (i == 0) ? topology.inputsCount :
				topology.neuronConfigurations[i - 1].size();

		std::vector<Neuron> neuronsInLayer;
		for (const auto & neuronConfig : layerConfig) {
			neuronsInLayer.push_back(Neuron(neuronSize, neuronConfig.first, neuronConfig.second));
		}
		layers.push_back(NeuralLayer(neuronsInLayer));
	}

	// TODO remove this
	layerOutput = (double **)malloc((layers.size() + 1)*sizeof(double *));
	layerOutput[0] = (double *)malloc(getInputsCount()*sizeof(double));
	for (size_t i = 1; i <= layers.size(); ++i) {
		layerOutput[i] = (double *)malloc(layers[i - 1].size()*sizeof(double));
	}
}

// TODO return nice look but increase it's speed to good values
double * NeuralNetwork::propogate(std::vector<double> & input) {

	/* Я понимаю, что это красиво, но блин, как же это долго... */
	//if (input.size() != getInputsCount()) {
	//	std::stringstream errorMessageStream;
	//	errorMessageStream << "passed input size ( " << input.size() <<
	//			" ) does not match network input size ( " << getInputsCount() << " )";
	//	throw std::length_error(errorMessageStream.str());
	//}

	//std::vector<double> layerOutput = layers[0].calculateOutput(input);
	//for (size_t i = 1; i < layers.size(); i++) {
	//	layerOutput = layers[i].calculateOutput(layerOutput);
	//}

	//return layerOutput;
	//std::vector<std::vector<double>> layerOutput;

	/* Зато это быстро, как фотоны. (Хотя эта мерзость выполняется 1.9 миллисекунды
	   для сети с конфигурацией 258х258х258х1 (257 входов, 1 выход)) */
	layerOutput[0] = input.data();

	const int lCount = layers.size();
	for (int i = 0; i < lCount; ++i) {

		const auto & neurons = layers[i].getNeurons();

		const double * lInputs = layerOutput[i];
		double * lOutputs = layerOutput[i + 1];

		const int nCount = neurons.size();

		for (int j = 0; j < nCount; ++j) {

			const double * weights = neurons[j].getWeights().data();

			double x = neurons[j].getBias();
			const int wCount = neurons[j].getWeights().size();

			// assembly code looks OK
			for (int k = 0; k < wCount; ++k) x += weights[k]*lInputs[k];
			lOutputs[j] = x > 0 ? x : x*0.01;
		}
	}

	return layerOutput[lCount];
}

size_t NeuralNetwork::getHiddenNeuronsCount() const {
	size_t neuronsCount = 0;
	size_t layersCount = layers.size();
	for (size_t i = 0; i < layersCount - 1; ++i) {
		neuronsCount += layers[i].size();
	}
	return neuronsCount;
}

size_t NeuralNetwork::getNeuronsCount() const {
	size_t neuronsCount = 0;
	for (NeuralLayer l : layers) {
		neuronsCount += l.size();
	}
	return neuronsCount;
}

void NeuralNetwork::saveNetwork(std::ostream & stream, const bool binary) const {

	register double bias;
	register int functionId;
	register size_t layersCount, neuronsCount, weightsCount, paramsCount;

	layersCount = size();

	if (binary) {

		stream.write((char *)&layersCount, sizeof(layersCount));

		for (const auto & layer : layers) {

			neuronsCount = layer.size();
			stream.write((char *)&neuronsCount, sizeof(neuronsCount));

			for (const auto & neuron : layer.getNeurons()) {

				functionId = (int)neuron.getActivationFunction();
				const std::vector<double> & params = neuron.getActivationFunctionParams();
				const std::vector<double> & weights = neuron.getWeights();
				bias = neuron.getBias();
				weightsCount = weights.size();
				paramsCount = params.size();

				stream.write((char *)&functionId, sizeof(functionId));
				stream.write((char *)&bias, sizeof(bias));
				stream.write((char *)&paramsCount, sizeof(paramsCount));
				stream.write((char *)&weightsCount, sizeof(weightsCount));
				if (paramsCount != 0) {
					stream.write((char *)params.data(), sizeof(double)*paramsCount);
				}
				if (weightsCount != 0) {
					stream.write((char *)weights.data(), sizeof(double)*weightsCount);
				}
			}
		}
	} else {

		stream << layersCount << std::endl << std::endl;

		for (const auto & layer : layers) {

			const size_t neuronsCount = layer.size();
			stream << neuronsCount << std::endl;

			for (const auto & neuron : layer.getNeurons()) {

				functionId = neuron.getActivationFunction();
				const std::vector<double> & params = neuron.getActivationFunctionParams();
				const std::vector<double> & weights = neuron.getWeights();
				bias = neuron.getBias();
				weightsCount = weights.size();
				paramsCount = params.size();

				stream << functionId << " " << bias << " " << paramsCount << " " << weightsCount << " ";

				for (const double p : params) {
					stream << p << " ";
				}
				for (const double w : weights) {
					stream << w << " ";
				}

				stream << std::endl;
			}
		}
	}
}
