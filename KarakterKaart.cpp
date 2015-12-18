#include "stdafx.h"
#include "KarakterKaart.h"

#include "Functions.hpp"

std::istream& operator>>(std::istream& is, KarakterKaart& kaart)
{
	std::string line;

	std::getline(is, line);

	std::vector<std::string> data = Split(line, ';');

	if (data.size() == 2)
	{
		kaart.nummer_ = std::stoi(data[0]);
		kaart.naam_ = data[1];

		kaart.isValid_ = true;
	}

	return is;
}
