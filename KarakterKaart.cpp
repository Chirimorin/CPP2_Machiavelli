#include "stdafx.h"
#include "KarakterKaart.h"

#include "Functions.hpp"

std::string KarakterKaart::getColorString()
{
	if (naam_ == "Koning")
		return "\33[43m";
	if (naam_ == "Koopman")
		return "\33[42m";
	if (naam_ == "Prediker")
		return "\33[44m";
	if (naam_ == "Condottiere")
		return "\33[41m";

	return "\33[40m";
}

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
