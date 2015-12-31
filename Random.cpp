#include "stdafx.h"
#include "Random.h"

Random::Random()
{
}

Random::~Random()
{
}

int Random::getRandomNumber(int min, int max)
{
	std::uniform_int_distribution<int> randomNumber{ min, max };
	return randomNumber(getEngine());
}

std::default_random_engine Random::getEngine() {
	std::random_device dev;
	std::default_random_engine dre{ dev() };

	return dre;
}
