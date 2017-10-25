// DEPRECATED CODE

#include <algorithm>
#include <atomic>
#include <numeric>
#include <thread>

#include "2048.h"
#include "MLUtilities.h"

namespace {

	typedef std::vector<std::vector<int>> Field;
	typedef std::function<void(const Field & input, Field & result)> Rotator;
	typedef std::pair<double, int> DirWeight;

	struct LineState {
		std::vector<int> line;
		int action;
	};

	LineState makeLineState(const std::vector<int> & line, const int action) {
		LineState ls;
		ls.line = line;
		ls.action = action;
		return ls;
	}

	const int CELL_BINARY_OFFSET = 5; // log2(32)
	const int CELL_BINARY_MOD_MASK = 32 - 1; // 0b11111
	const int LINE_MOVE_DESCRIPTOR_SIZE = 32*32*32*32;
	const int FEATURES_COUNT = 16*16 + 1;

	std::atomic<int> maxScore(0); // atomic variable that contains highest score for all getScore calls
	std::vector<int> powers2(18); // powers of 2
	std::vector<Rotator> forwardRotator, backwardRotator;

	// vector that describes all possible line movements for fast move and merge processing
	// for line (2^a, 2^b, 2^c, 2^d) we create an index i = a + (b<<5) + (c<<10) + (d<<15), which
	// indexes structure in this vector that describes line transorm to get new line
	std::vector<LineState> moveStates(LINE_MOVE_DESCRIPTOR_SIZE, makeLineState(std::vector<int>(4), 0));
}

inline bool pairCompare(const DirWeight & firstElem, const DirWeight & secondElem) {
	return firstElem.first > secondElem.first;
}

inline int freeCellsCount(const Field & field) {
    register int count = 0;
    for (register int y = 0; y < 4; ++y) {
        for (register int x = 0; x < 4; ++x) {
			count += !field[y][x];
        }
    }
    return count;
}

inline int calculateScore(const Field & field) {
	register int score = 0;
	for (register int y = 0; y < 4; ++y) {
		for (register int x = 0; x < 4; ++x) {
			score += powers2[field[y][x]] * (field[y][x] - 1);
		}
	}
	return score;
}

inline void printField(std::ostream & out, const Field & field) {
	for (int y = 0; y < 4; ++y) {
		for (int x = 0; x < 4; ++x) {
			out << powers2[field[y][x]] << "\t";
		}
		out << "\n";
	}
	out << "\n";
}

// this transforms field as like as press in up direction
inline int press(Field & field) {

	register int action = 0;
	register const LineState * state;
	register int * line;

	for (register int y = 0; y < 4; ++y) { // for each line
		line = field[y].data(); // getting raw line data
		state = &moveStates[ line[0] + // getting move and merge line descriptor
							(line[1] << CELL_BINARY_OFFSET) +
							(line[2] << CELL_BINARY_OFFSET*2) +
							(line[3] << CELL_BINARY_OFFSET*3) ];
		field[y] = state->line; // update line
		if (state->action) { // check for action
			action = 1;
		}
	}

	return action;
}

inline int moveField(const Field & input,
					 Field & temp,
					 Field & output,
					 const int dir) {
	forwardRotator[dir](input, temp);
	int action = press(temp);
	backwardRotator[dir](temp, output);
	return action;
}

inline void rotateLeft(const Field & input, Field & result) {
	result = input;
}

inline void rotateRight(const Field & input, Field & result) {
	for (register int i = 0; i < 4; ++i) {
		for (register int j = 0; j < 4; ++j) {
			result[i][3 - j] = input[i][j];
		}
	}
}

inline void rotateUp(const Field & input, Field & result) {
	for (register int i = 0; i < 4; ++i) {
		for (register int j = 0; j < 4; ++j) {
			result[j][i] = input[i][j];
		}
	}
}

inline void rotateDown(const Field & input, Field & result) {
	for (register int i = 0; i < 4; ++i) {
		for (register int j = 0; j < 4; ++j) {
			result[j][3 - i] = input[i][j];
		}
	}
}

inline void rotateDownReversed(const Field & input, Field & result) {
	for (register int i = 0; i < 4; ++i) {
		for (register int j = 0; j < 4; ++j) {
			result[3 - j][i] = input[i][j];
		}
	}
}

// adds new number (if possible) to the field with passed RNG
inline void createNewNumber(Field & field, URNG & random) {

	const double fourProbability = 0.1;

    int free = freeCellsCount(field);
    if (free != 0) {

		int number = random.next<double>(0, 1) <= fourProbability ? 2 : 1;
        int blockPlace = random.next<int>(1, free);
        int pos = 1;

        for (int y = 0; y < 4; ++y) {
            for (int x = 0; x < 4; ++x) {
				if (!field[y][x]) {
					if (pos == blockPlace) {
						field[y][x] = number;
						return;
					}
					++pos;
				}
            }
        }
    }
}

// function to fill moveStates vector with correct descriptors
inline void mergeLine(LineState & lineState) {

	register int action = false;
	register int i, j;
	register int * line = lineState.line.data();

	const int zeroCount = !line[0] + !line[1] + !line[2] + !line[3];
	switch (zeroCount) {
	case 0: // We have no flat field. Here we can have double merge and single merge.
		if (line[0] == line[1] && line[2] == line[3]) { // double merge
			action = 1;
			line[0]++;
			line[1] = line[2] + 1;
			line[2] = line[3] = 0;
		} else if (line[0] == line[1]) { // single merge
			action = 1;
			line[0]++;
			line[1] = line[2];
			line[2] = line[3];
			line[3] = 0;
		} else if (line[1] == line[2]) { // single merge
			action = 1;
			line[1]++;
			line[2] = line[3];
			line[3] = 0;
		} else if (line[2] == line[3]) { // single merge
			action = 1;
			line[2]++;
			line[3] = 0;
		}
		break;
	case 1:
	case 2: // Moving, merging... Most complicated case.
		i = j = 0;
		for (; i < 4; ++i) { // moving tiles to line start without merge
			if (line[i]) {
				// line[i] is non-zero here, we should later make it zero
				// (if it was not replaced with otner tile)
				line[j++] = line[i];
				if (j - 1 != i) {
					action = 1;
				}
			}
		}
		for (; j < 4; ++j) { // making all tiles at the end of the line equal to zero
			line[j] = 0;
		}
		if (line[0] == line[1]) { // 0-1 merge
			action = 1;
			line[0]++;
			line[1] = line[2];
			line[2] = 0;
		} else if (line[1] == line[2]) { // 1-2 merge
			action = 1;
			line[1]++;
			line[2] = 0;
		}
		break;
	case 3: // here we have single tile so no merge will happen, but we should check if it will move
		if (!line[0]) {
			action = 1;
		}
		line[0] += line[1] + line[2] + line[3];
		line[1] = line[2] = line[3] = 0;
		break;
	default: // here we do not have any tile, so we do not need to do anything
		break;
	}

	lineState.action = action;
}

void init2048() {

	powers2[0] = 0;
	powers2[1] = 2;
	for (size_t i = 2; i < powers2.size(); ++i) {
		powers2[i] = powers2[i - 1]*2;
	}

	register int i, j;
	for (i = 0; i < LINE_MOVE_DESCRIPTOR_SIZE; ++i) {
		for (j = 0; j < 4; ++j) {
			moveStates[i].line[j] = (i >> (CELL_BINARY_OFFSET*j)) & CELL_BINARY_MOD_MASK;
		}
		mergeLine(moveStates[i]);
	}

	forwardRotator.push_back(rotateLeft);
	forwardRotator.push_back(rotateRight);
	forwardRotator.push_back(rotateUp);
	forwardRotator.push_back(rotateDown);

	backwardRotator = forwardRotator;
	backwardRotator[3] = rotateDownReversed;
}

char * dirsNames[4] = {"LEFT", "RIGHT", "UP", "DOWN"};

int playGame(NeuralNetwork * nn, URNG & random) { // return const to NeuralNetwork

	Field field(4, std::vector<int>(4, 0));
	Field rotateTemp(4, std::vector<int>(4, 0));
	Field temp(4, std::vector<int>(4, 0));
	std::vector<DirWeight> dirWeights(4);
	std::vector<double> x(FEATURES_COUNT);

	createNewNumber(field, random);
	createNewNumber(field, random);

	//printField(std::cout, field);

	int isLoose;

	do {

		for (int dir = 0; dir < 4; ++dir) {
			if (!moveField(field, rotateTemp, temp, dir)) {
				dirWeights[dir] = std::make_pair(-1e20, dir);
			} else {
				std::fill(x.begin(), x.end(), 0);
				for (int i = 0; i < 4; ++i) {
					for (int j = 0; j < 4; ++j) {
						x[   temp[i][j]*16 + i*4 + j   ] = 1;
					}
				}
				x[16*16] = freeCellsCount(temp)/16.0;
				auto y = nn->propogate(x);
				dirWeights[dir] = std::make_pair(y[0], dir);
			}
		}

		std::sort(dirWeights.begin(), dirWeights.end(), pairCompare);

		isLoose = true;
		for (int i = 0; i < 4; ++i) {
			if (moveField(field, rotateTemp, field, dirWeights[i].second)) {
				isLoose = false;
				break;
			}
		}

		createNewNumber(field, random);
		//printField(std::cout, field);

	} while (!isLoose);

	for (int y = 0; y < 4; ++y) {
		for (int x = 0; x < 4; ++x) {
			if (powers2[field[y][x]] == 2048) {
				printf("#");
			}
		}
	}

	int score = calculateScore(field);
	if (score > maxScore) {
		maxScore = score;
		printf("\nScore: %d\n", score);
		printField(std::cout, field);
	}

	return score;
}

const int threadsCount = 8;
std::vector<double> scores(threadsCount);


//inline void playRangomReal(NeuralNetwork * nn, Field field, const int index,
//						   URNG & random, std::mt19937 & rnd,
//						   std::vector<int> order, Field & rotateTemp) {
//
//	createNewNumber(field, random);
//
//	int isLoose;
//
//	do {
//
//		isLoose = true;
//
//		std::shuffle(order.begin(), order.end(), rnd);
//
//		for (int dir : order) {
//			rotate1(field, rotateTemp, dir);
//			int pressed = press(rotateTemp);
//			rotate2(rotateTemp, field, dir);
//			if (pressed) {
//				isLoose = false;
//				break;
//			}
//		}
//
//		createNewNumber(field, random);
//	} while (!isLoose);
//
//	scores[index] += calculateScore(field);
//}

inline void playRecursive(NeuralNetwork * nn, Field field, const int index,
						   URNG & random, std::vector<DirWeight> & dirWeights, std::vector<double> & x,
						   Field & temp, Field & rotateTemp) {

	createNewNumber(field, random);

	//printField(std::cout, field);

	int isLoose;

	do {

		for (int dir = 0; dir < 4; ++dir) {
			if (!moveField(field, rotateTemp, temp, dir)) {
				dirWeights[dir] = std::make_pair(-1e20, dir);
			} else {
				std::fill(x.begin(), x.end(), 0);
				for (int i = 0; i < 4; ++i) {
					for (int j = 0; j < 4; ++j) {
						x[   temp[i][j]*16 + i*4 + j   ] = 1;
					}
				}
				x[16*16] = freeCellsCount(temp)/16.0;
				auto y = nn->propogate(x);
				dirWeights[dir] = std::make_pair(y[0], dir);
			}
		}

		std::sort(dirWeights.begin(), dirWeights.end(), pairCompare);

		isLoose = true;
		for (int i = 0; i < 4; ++i) {
			if (moveField(field, rotateTemp, field, dirWeights[i].second)) {
				isLoose = false;
				break;
			}
		}

		createNewNumber(field, random);
		//printField(std::cout, field);

	} while (!isLoose);

	scores[index] += calculateScore(field);
}

//void playRandom(NeuralNetwork * nn, Field field, const int gamesCount, const int index) {
//
//	URNG random(0);
//	std::mt19937 rnd;
//	rnd.seed((int)time(0));
//
//	std::vector<int> order(4);
//	std::iota(order.begin(), order.end(), 0);
//
//	Field rotateTemp(4, std::vector<int>(4, 0));
//
//	scores[index] = 0;
//	for (int i = 0; i < gamesCount; ++i) {
//		playRangomReal(nn, field, index, random, rnd, order, rotateTemp);
//	}
//	scores[index] /= gamesCount;
//}

void playRandom(NeuralNetwork * nn, Field field, const int gamesCount, const int index) {

	URNG random;
	std::vector<DirWeight> dirWeights(4);
	std::vector<double> x(FEATURES_COUNT);
	Field temp(4, std::vector<int>(4, 0));
	Field rotateTemp(4, std::vector<int>(4, 0));

	scores[index] = 0;
	for (int i = 0; i < gamesCount; ++i) {
		playRecursive(nn, field, index, random, dirWeights, x, temp, rotateTemp);
	}
	scores[index] /= gamesCount;
}

int playMonteCarlo(std::vector<NeuralNetwork *> & nns, URNG & random) {

	Field field(4, std::vector<int>(4, 0));
	Field temp(4, std::vector<int>(4, 0));
	Field rotateTemp(4, std::vector<int>(4, 0));
	std::vector<DirWeight> dirWeights(4);
	std::vector<std::thread> threads(threadsCount);

	createNewNumber(field, random);
	createNewNumber(field, random);

	printField(std::cout, field);

	int isLoose;

	do {

		for (int dir = 0; dir < 4; ++dir) {
			double score = -1;
			if (moveField(field, rotateTemp, temp, dir)) {
				score = 0;
				for (size_t t = 0; t < threadsCount; ++t) {
					threads[t] = std::thread(playRandom, nns[t], temp, 100, t);
				}
				for (size_t t = 0; t < threadsCount; ++t) {
					threads[t].join();
				}
				for (size_t t = 0; t < threadsCount; ++t) {
					score += scores[t];
				}
				score /= 4;
			}
			dirWeights[dir] = std::make_pair(score, dir);
		}

		std::sort(dirWeights.begin(), dirWeights.end(), pairCompare);

		isLoose = true;
		for (int i = 0; i < 4; ++i) {
			if (moveField(field, rotateTemp, field, dirWeights[i].second)) {
				isLoose = false;
				break;
			}
		}

		printField(std::cout, field);

		createNewNumber(field, random);
	} while (!isLoose);

	return calculateScore(field);
}
