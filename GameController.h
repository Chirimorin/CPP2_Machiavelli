#pragma once
#include "KarakterKaart.h"
#include "KaartStapel.h"
class ClientCommand;
class Player;
class Socket;

class GameController
{
public:
	static GameController& getInstance();

	void addPlayer(std::shared_ptr<Player> player, std::shared_ptr<Socket> client);
	void removePlayer(std::shared_ptr<Player> player);

	bool handleCommand(ClientCommand& command);
	void messageAllPlayers(std::string message);
private:
	GameController();
	~GameController() {};

	bool gameStarted_ = false;
	std::vector<std::unique_ptr<KarakterKaart>> karakterKaarten_;
	std::unique_ptr<KaartStapel> kaartStapel_;
	std::map<std::shared_ptr<Player>, std::shared_ptr<Socket>> spelers_;
	std::shared_ptr<Player> koning_;
	int goudstukken_ = 30;
	bool cheat_ = false;

	bool startGame();
	void loadCharacterCards();
	void distributeCharacterCards();
	void cheatDistributeCharacterCards();
	void addRandomCharacterCard(std::vector<std::unique_ptr<KarakterKaart>> &currentKarakterKaarten, std::shared_ptr<Player> player);
};
