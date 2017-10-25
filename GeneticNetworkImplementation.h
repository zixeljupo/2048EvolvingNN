#pragma once

#include "GeneticNetwork.h"
#include "URNG.h"

/**
 *  Class GeneticNetworkImplementation
 *
 *	GeneticNetworkImplementation is a child of GeneticNetwork abstract class which
 * implements it's four pure virtual methods.
 *
 *  MutationParameters struct describes genetic network mutation character. See full
 * description in commentaries below.
 *
 *  GeneticNetworkImplementation class has two new members: random and mutationParams.
 * random is a reference to URNG that is used in mutate method for random changes over
 * parameters. mutationParams describes how mutate will modify parameters.
 */

/**
 * There are three types of mutations: flip mutation, randomly changed weight mutation and
 * epsilon addition. All mutations are performed on every single gene separately.
 * Flip mutation inverses gene value. Change mutation replaces gene with random value of
 * selected range. Add mutation adds random value to existing gene of selected range.
 */
struct MutationParameters {

	// rates sum should not exceed 1.0 because only one mutation per weight is allowed
	double flipRate;
	double changeRate;
	double additionRate;

	double changeRange;
	double addRange;
};

extern std::istream & operator>>(std::istream & stream, MutationParameters & mp);

const MutationParameters DEFAULT_MUTATION_PARAMS = {0, 0.001, 0.2, 0.2, 0.1};
const double DEFAULT_SIMILARITY = 0.002;

class GeneticNetworkImplementation : public GeneticNetwork {

public:

	GeneticNetworkImplementation(const GeneticNetworkImplementation * gnn) :
			GeneticNetwork(gnn),
			mutationParams(gnn->mutationParams),
			similarityValue(gnn->similarityValue) {}

	GeneticNetworkImplementation(const NeuralNetwork * nn,
			const MutationParameters & mutationParameters = DEFAULT_MUTATION_PARAMS,
			const double similarity = DEFAULT_SIMILARITY) :
			GeneticNetwork(nn), mutationParams(mutationParameters),
			similarityValue(similarity) {}

	GeneticNetworkImplementation(const Topology & topology,
			const MutationParameters & mutationParameters = DEFAULT_MUTATION_PARAMS,
			const double similarity = DEFAULT_SIMILARITY) :
			GeneticNetwork(topology), mutationParams(mutationParameters),
			similarityValue(similarity) {}

	virtual std::vector<double *> getNetworkGeneticCode() override;
	virtual std::vector<double> getNetworkGeneticCode() const override;
	virtual void mutate(URNG & random) override;
	virtual std::pair<GeneticNetwork *, GeneticNetwork *> crossingOver(
			const GeneticNetwork * nn, URNG & random) const override;

	inline MutationParameters & getMutationParams() { return mutationParams; }
	inline const MutationParameters & getMutationParams() const { return mutationParams; }
	inline void setMutationParams(const MutationParameters & mutationParameters)
			{ mutationParams = mutationParameters; }

	inline double getSimilarityValue() const { return similarityValue; }
	inline void setSimilarityValue(const double similarity) { similarityValue = similarity; }

	virtual ~GeneticNetworkImplementation() {}

protected:
	
	MutationParameters mutationParams;
	double similarityValue;

};
