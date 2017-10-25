#pragma once

#include <random>
#include <chrono>
#include <iterator>

#if defined(max)
#undef max
#endif
#if defined(min)
#undef min
#endif

template <class Generator>
class NumberGenerator final {

	Generator g;

public:

	template <typename S>
	NumberGenerator(const std::vector<S> & seedVec) {
		g.seed(std::seed_seq(seedVec.begin(), seedVec.end()));
	}

	template <typename S>
	NumberGenerator(const S & s) {
		auto seed = static_cast<std::seed_seq::result_type>(s);
		std::vector<decltype(seed)> seedVec(1, seed);
		g.seed(std::seed_seq(seedVec.begin(), seedVec.end()));
	}

	NumberGenerator() {
		auto seed = static_cast<std::seed_seq::result_type>(
			std::chrono::high_resolution_clock::now().time_since_epoch().count()
		);
		std::vector<decltype(seed)> seedVec(1, seed);
		g.seed(std::seed_seq(seedVec.begin(), seedVec.end()));
	}

	template <typename T>
	inline typename std::enable_if<std::is_floating_point<T>::value, T>::type
	next(const T min = 0, const T max = 1) {
		return std::uniform_real_distribution<T>(min, max)(g);
	}

	template <typename T>
	inline typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value, T>::type
	next(const T min = std::numeric_limits<T>::lowest(),
		 const T max = std::numeric_limits<T>::max()) {
		return std::uniform_int_distribution<T>(min, max)(g);
	}

	template <typename T>
	inline typename std::enable_if<std::is_same<T, bool>::value, T>::type next() {
		return std::bernoulli_distribution()(g);
	}
};
