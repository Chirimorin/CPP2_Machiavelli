#include "stdafx.h"
#include "GameController.h"
#include "KaartStapel.h"

GameController& GameController::getInstance()
{
	static GameController instance;
	return instance;
}

GameController::GameController()
{
	loadCharacterCards();

	std::cout << "GameController ctor\n";
	kaartStapel_ = std::make_unique<KaartStapel>();
	
}

void GameController::loadCharacterCards()
{
	std::ifstream karakterkaarten("Resources/karakterkaarten.csv");

	if (karakterkaarten)
	{
		while (!karakterkaarten.eof())
		{
			std::unique_ptr<KarakterKaart> kaart(new KarakterKaart);
			karakterkaarten >> *kaart;
			if (kaart->isValid())
				karkaterKaarten_.push_back(std::move(kaart));
		}
	}
}