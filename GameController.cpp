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
	std::cout << "GameController ctor\n";
	kaartStapel_ = std::make_unique<KaartStapel>();
}
