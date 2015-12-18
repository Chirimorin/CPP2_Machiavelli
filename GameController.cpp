#include "stdafx.h"
#include "GameController.h"
#include "KaartStapel.h"

GameController& GameController::getInstance()
{
	static GameController instance;
	return instance;
}

void GameController::addPlayer(std::shared_ptr<Player> player, std::shared_ptr<Socket> client)
{
	spelers_.insert(std::make_pair(player, client));
}

void GameController::removePlayer(std::shared_ptr<Player> player)
{
	spelers_.erase(player);
}

GameController::GameController()
{
	std::cout << "GameController ctor\n";
	kaartStapel_ = std::make_unique<KaartStapel>();
}
