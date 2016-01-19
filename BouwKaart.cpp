#include "stdafx.h"
#include "BouwKaart.h"

#include "Functions.hpp"

std::string BouwKaart::getColorString()
{
	if (kleur_ == "geel")
		return "\33[43m";
	if (kleur_ == "groen")
		return "\33[42m";
	if (kleur_ == "blauw")
		return "\33[44m";
	if (kleur_ == "rood")
		return "\33[41m";
	if (kleur_ == "lila")
		return "\33[45m";

	return "\33[40m";
}

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
