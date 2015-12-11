#include "stdafx.h"
#include "KaartStapel.h"

KaartStapel::KaartStapel()
{
	std::ifstream bouwkaarten("Resources/Bouwkaarten.csv");

	if (bouwkaarten)
	{
		while (!bouwkaarten.eof())
		{
			std::unique_ptr<BouwKaart> kaart(new BouwKaart);
			bouwkaarten >> *kaart;
			if (kaart->IsValid())
				stapel_.push_back(std::move(kaart));
		}
	}

	std::random_shuffle(stapel_.begin(), stapel_.end());

	std::cout << stapel_.size() << '\n';
}
