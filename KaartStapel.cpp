#include "stdafx.h"
#include "KaartStapel.h"
#include "Random.h"

KaartStapel::KaartStapel()
{
	std::cout << "KaartStapel ctor\n";
	std::ifstream bouwkaarten("Resources/Bouwkaarten.csv");

	if (bouwkaarten) {
		while (!bouwkaarten.eof()) {
			std::unique_ptr<BouwKaart> kaart(new BouwKaart);
			bouwkaarten >> *kaart;
			if (kaart->IsValid())
				stapel_.push_back(std::move(kaart));
		}
	}

	std::shuffle(stapel_.begin(), stapel_.end(), Random::getEngine());

	std::cout << stapel_.size() << " kaarten ingeladen.\n";
}

std::unique_ptr<BouwKaart> KaartStapel::getBuildCard()
{
	std::unique_ptr<BouwKaart> bouwkaart = std::move(stapel_.front());
	stapel_.pop_front();
	return bouwkaart;
}

void KaartStapel::addBuildCard(std::unique_ptr<BouwKaart> bouwkaart) {
	stapel_.push_back(move(bouwkaart));
}
