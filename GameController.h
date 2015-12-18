#pragma once
#include "KarakterKaart.h"

class KaartStapel;

class GameController
{
public:
	static GameController& getInstance();
private:
	GameController();
	~GameController() {};
	
	std::vector<std::unique_ptr<KarakterKaart>> karkaterKaarten_;
	std::unique_ptr<KaartStapel> kaartStapel_;

	void loadCharacterCards();
};

