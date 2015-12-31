#pragma once
#include <random>

class Random
{
public:
	Random();
	~Random();

	static int getRandomNumber(int min, int max);
	static std::default_random_engine getEngine();
};

