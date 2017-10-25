#include "WeightsInitializer.h"

void WeightsInitializer::initializeWeights(NeuralNetwork & nn, URNG & gen) const {
	for (NeuralLayer & l : nn.getLayers()) {
		for (Neuron & n : l.getNeurons()) {
			n.setBias(gen.next<double>(-0.5, 0.5));
			for (double & w : n.getWeights()) {
				w = gen.next<double>(-0.5, 0.5);
            }
        }
    }
}

//namespace {
//	double calculateFactor(size_t inputsCount, size_t hiddenNeuronsCount) {
//		return 0.7*pow((double)hiddenNeuronsCount, (double)1/inputsCount);
//	}
//}
//
//void WeightsInitializer::initializeWeights(NeuralNetwork & nn, URNG & gen) const {
//
//	double factor = calculateFactor(nn.getInputsCount(), nn.getHiddenNeuronsCount());
//
//	for (NeuralLayer & l : nn.getLayers()) {
//		for (Neuron & n : l.getNeurons()) {
//			for (double & w : n.getWeights()) {
//				w = gen.next<double>(-0.5, 0.5);
//            }
//        }
//    }
//
//	for (NeuralLayer & l : nn.getLayers()) {
//		for (Neuron & n : l.getNeurons()) {
//			for (size_t j = 0; j < l.size(); j++) {
//				double norm = 0;
//                for (size_t q = 0; q < l.size(); q++) {
//					double weight = l.getNeurons()[q].getWeights()[j];
//                    norm += weight*weight;
//                }
//                norm = sqrt(norm);
//				n.getWeights()[j] *= factor/norm;
//			}
//        }
//    }
//
//	for (NeuralLayer l : nn.getLayers()) {
//        for (Neuron n : l.getNeurons()) {
//            n.setShift(gen.next<double>(-factor, factor));
//        }
//    }
//}
