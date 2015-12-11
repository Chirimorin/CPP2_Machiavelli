#include "stdafx.h"
#include "BouwKaart.h"

#include "Functions.hpp"

std::istream& operator>>(std::istream& is, BouwKaart& kaart)
{
	std::string line;

	std::getline(is, line);

	std::vector<std::string> data = Split(line, ';');

	if (data.size() == 4)
	{
		kaart.naam_ = data[0];
		kaart.prijs_ = std::stoi(data[1]);
		kaart.kleur_ = data[2];

		if (kaart.kleur_ != "lila")
			kaart.isValid_ = true;
	}

	return is;
}
