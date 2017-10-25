#pragma once

#include <vector>
#include <exception>
#include <fstream>

struct MutationParameters;
class NeuralNetwork;

template <typename T>
struct lossComparator {
	inline bool operator()(const std::pair<T, double> & p1,
						   const std::pair<T, double> & p2) {
		return p1.second < p2.second;
	}
};

template <typename T>
struct scoreComparator {
	inline bool operator()(const std::pair<T, double> & p1,
						   const std::pair<T, double> & p2) {
		return p1.second > p2.second;
	}
};

template <typename T>
inline T norm(const std::vector<T> & v, const int n = 2) {
	//if (n < 1) {
	//    throw std::invalid_argument("L[n] norm should have positive n");
	//}
	return pow(sum(pow(abs(v), n)), 1.0/n);
}

template <typename T>
inline T operator&(const std::vector<T> & v1, const std::vector<T> & v2) {

	//if (v1.size() != v2.size()) {
	//	throw std::length_error("");
	//}

	T result = 0;
	for (size_t i = 0; i < v1.size(); i++) {
		result += v1[i]*v2[i];
	}
	return result;
}

template <typename T>
inline std::vector<T> operator*(const std::vector<T> & v1, const std::vector<T> & v2) {

	//if (v1.size() != v2.size()) {
	//	throw std::length_error("");
	//}

	std::vector<T> result;
	for (size_t i = 0; i < v1.size(); i++) {
		result.push_back(v1[i]*v2[i]);
	}
	return result;
}

template <typename T>
inline std::vector<T> operator/(const std::vector<T> & v1, const std::vector<T> & v2) {

	//if (v1.size() != v2.size()) {
	//	throw std::length_error("");
	//}

	std::vector<T> result;
	for (size_t i = 0; i < v1.size(); i++) {
		result.push_back(v1[i]/v2[i]);
	}
	return result;
}

template <typename T>
inline std::vector<T> operator+(const std::vector<T> & v1, const std::vector<T> & v2) {

	//if (v1.size() != v2.size()) {
	//	throw std::length_error("");
	//}

	std::vector<T> result;
	for (size_t i = 0; i < v1.size(); i++) {
		result.push_back(v1[i] + v2[i]);
	}
	return result;
}

template <typename T>
inline std::vector<T> operator+(const T v1, const std::vector<T> & v2) {
	return std::vector<T>(v2.size(), v1) + v2;
}

template <typename T>
inline std::vector<T> operator+(const std::vector<T> & v1, const T v2) {
	return v2 + v1;
}

template <typename T>
inline std::vector<T> operator-(const std::vector<T> & v1, const std::vector<T> & v2) {

	//if (v1.size() != v2.size()) {
	//	throw std::length_error("");
	//}

	std::vector<T> result;
	for (size_t i = 0; i < v1.size(); i++) {
		result.push_back(v1[i] - v2[i]);
	}
	return result;
}

template <typename T>
inline std::vector<T> operator-(const T v1, const std::vector<T> & v2) {
	return std::vector<T>(v2.size(), v1) - v2;
}

template <typename T>
inline std::vector<T> operator-(const std::vector<T> & v1, const T v2) {
	return v2 - v1;
}

template <typename T>
inline T max(const std::vector<T> & v) {

	if (v.size() == 0) {
		throw std::length_error("");
	}

	T max_val = v[0];
	for (size_t i = 1; i < v.size(); i++) {
		max_val = max(max_val, v[i]);
	}
	return max_val;
}

template <typename T>
inline size_t argmax(const std::vector<T> & v) {

	if (v.size() == 0) {
		throw std::length_error("");
	}

	T max_val = v[0];
	size_t max_pos = 0;
	for (size_t i = 1; i < v.size(); i++) {
		if (v[i] > max_val) {
			max_val = v[i];
			max_pos = i;
		}
	}
	return max_pos;
}

template <typename T>
inline T min(const std::vector<T> & v) {

	if (v.size() == 0) {
		throw std::length_error("");
	}

	T min_val = v[0];
	for (size_t i = 1; i < v.size(); i++) {
		min_val = min(min_val, v[i]);
	}
	return min_val;
}

template <typename T>
inline size_t argmin(const std::vector<T> & v) {

	if (v.size() == 0) {
		throw std::length_error("");
	}

	T min_val = v[0];
	size_t min_pos = 0;
	for (size_t i = 1; i < v.size(); i++) {
		if (v[i] < min_val) {
			min_val = v[i];
			min_pos = i;
		}
	}
	return min_pos;
}

template <typename T>
inline std::vector<bool> operator>(const std::vector<T> & v1, const std::vector<T> & v2) {

	//if (v1.size() != v2.size()) {
	//	throw std::length_error("");
	//}

	std::vector<bool> result;
	for (size_t i = 0; i < v1.size(); i++) {
		result.push_back(v1[i] > v2[i]);
	}
	return result;
}

template <typename T>
inline std::vector<bool> operator<(const std::vector<T> & v1, const std::vector<T> & v2) {

	//if (v1.size() != v2.size()) {
	//	throw std::length_error("");
	//}

	std::vector<bool> result;
	for (size_t i = 0; i < v1.size(); i++) {
		result.push_back(v1[i] < v2[i]);
	}
	return result;
}

template <typename T>
inline std::vector<T> abs(const std::vector<T> & v) {
	std::vector<T> result;
	for (size_t i = 0; i < v.size(); i++) {
		result.push_back(abs(v[i]));
	}
	return result;
}

template <typename T>
inline std::vector<T> pow(const std::vector<T> & v, const int n) {
	std::vector<T> result;
	for (size_t i = 0; i < v.size(); i++) {
		result.push_back(pow(v[i], n));
	}
	return result;
}

template <typename T>
inline T sum(const std::vector<T> & v) {
	T result = 0;
	for (size_t i = 0; i < v.size(); i++) {
		result += v[i];
	}
	return result;
}

double calculateAccuracy(NeuralNetwork * nn, // return const
						 const std::vector<std::vector<double>> & _x,
						 const std::vector<std::vector<double>> & _y);

template <typename T>
inline double lLoss(const std::vector<T> & expected,
					const std::vector<T> & got, const int n = 2) {
	return norm(expected - got, n);
}

void printNeuralNetwork(const NeuralNetwork * nn);

inline void printVector(const char* prefix, const std::vector<double> & vec) {
	printf("%s ", prefix);
	for (size_t i = 0; i < vec.size(); i++) {
		printf("%.2lf ", vec[i]);
	}
	printf("\n");
}

MutationParameters readMutationParameters();

void printDataset(const std::vector<std::pair<std::vector<double>, std::vector<double>>> & dataset);
