#pragma once
#include "KarakterKaart.h"
#include "KaartStapel.h"
class Player;
class Socket;

class GameController
{
public:
	static GameController& getInstance();

	void addPlayer(std::shared_ptr<Player> player, std::shared_ptr<Socket> client);
	void removePlayer(std::shared_ptr<Player> player);


private:
	GameController();
	~GameController() {};

	std::vector<std::unique_ptr<KarakterKaart>> karkaterKaarten_;
	std::unique_ptr<KaartStapel> kaartStapel_;
	std::map<std::shared_ptr<Player>, std::shared_ptr<Socket>> spelers_;
	void loadCharacterCards();
};
