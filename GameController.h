#pragma once
#include "KarakterKaart.h"
#include "KaartStapel.h"
class ClientCommand;
class Player;
class Socket;

namespace machiavelli {
	extern const std::string prompt;
}

enum class GameState
{
	NotStarted,
	ChooseCharacter,
	RemoveCharacter,
	NewRound,
	ChooseGoldOrCard,
	PickBuildCard,
	PlayTurn, 
	Ended
};



class GameController
{
public:
	static GameController& getInstance();

	void addPlayer(std::shared_ptr<Player> player, std::shared_ptr<Socket> client);
	void removePlayer(std::shared_ptr<Player> player);

	bool handleCommand(ClientCommand& command);
	void messageAllPlayers(std::string message);
	void messagePlayer(std::shared_ptr<Player> player, std::string message);
private:
	GameController();
	~GameController() {};

	std::vector<std::unique_ptr<KarakterKaart>> karakterKaarten_;
	std::vector<std::unique_ptr<KarakterKaart>> discardedKarakterKaarten_;
	std::unique_ptr<KaartStapel> kaartStapel_;
	std::map<std::shared_ptr<Player>, std::shared_ptr<Socket>> spelers_;
	std::shared_ptr<Player> koning_;
	std::shared_ptr<Player> currentPlayer_;
	int currentCharacter_ = 1;
	int goudstukken_ = 30;
	bool cheat_ = false;

	GameState currentState_ = GameState::NotStarted;

	std::shared_ptr<Player> winnaar_ = nullptr;

	std::vector<std::unique_ptr<BouwKaart>> mogelijkeNieuweBouwkaarten_;
	//std::unique_ptr<BouwKaart> nieuweBouwKaart1_;
	//std::unique_ptr<BouwKaart> nieuweBouwKaart2_;

	bool startGame();
	void nextPlayer();
	void loadCharacterCards();

	void distributeCharacterCards();
	void promptForCharacterCard();
	void getCharacterCard(std::string name);
	void removeCharacterCard(std::string name);
	void cheatDistributeCharacterCards();

	void startRound();
	void newTurn();
	void addRandomCharacterCard(std::vector<std::unique_ptr<KarakterKaart>> &currentKarakterKaarten, std::shared_ptr<Player> player);

	void addGold();
	void chooseNewBuildCard();
	void promptForGetNewBuildCard();
	void getNewBuildCard(std::string name);
	
	void promptPlayTurn();
	void buildCard(std::string card);
	void useAbility();

	void endGame();
};
