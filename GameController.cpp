#include "stdafx.h"
#include "GameController.h"
#include "ClientCommand.h"
#include "Player.hpp"
#include "Socket.h"
#include "Random.h"

namespace machiavelli {
	const std::string clearPrompt{ "\r             \r" };
	const std::string prompt       { "machiavelli> " };
}

GameController& GameController::getInstance()
{
	static GameController instance;
	return instance;
}

void GameController::addPlayer(std::shared_ptr<Player> player, std::shared_ptr<Socket> client)
{
	messageAllPlayers("Speler " + player->get_name() + " doet nu mee.");
	spelers_.insert(std::make_pair(player, client));
}

void GameController::removePlayer(std::shared_ptr<Player> player)
{
	spelers_.erase(player);
	messageAllPlayers("Speler " + player->get_name() + " heeft het spel verlaten.");
}

bool GameController::handleCommand(ClientCommand& command)
{
	std::shared_ptr<Socket> client{ command.get_client() };
	std::shared_ptr<Player> player{ command.get_player() };

	if (currentState_ == GameState::NotStarted)
	{
		if (command.get_cmd() == "start") {
			if (spelers_.size() >= 2)
				return startGame();
		}
		if (command.get_cmd() == "start cheat") {
			if (spelers_.size() >= 2) {
				cheat_ = true;
				return startGame();
			}
		}
	}
	else if (currentState_ == GameState::ChooseCharacter &&
			 currentPlayer_ == player)
	{
		getCharacterCard(command.get_cmd());
		return true;
	}
	else if (currentState_ == GameState::RemoveCharacter &&
		     currentPlayer_ == player)
	{
		removeCharacterCard(command.get_cmd());
		return true;
	}
	else if (currentState_ == GameState::PlayTurn &&
		     currentPlayer_ == player)
	{
		// TODO: handle command voor de beurt van een speler
	}

	return false;
}

void GameController::messageAllPlayers(std::string message)
{
	for (std::pair<std::shared_ptr<Player>, std::shared_ptr<Socket>> pair : spelers_)
	{
		messagePlayer(pair.first, message);
		//*pair.second << machiavelli::clearPrompt << message << "\r\n" << machiavelli::prompt;
	}
}

void GameController::messagePlayer(std::shared_ptr<Player> speler, std::string message)
{
	*spelers_[speler] << machiavelli::clearPrompt << message << "\r\n" << machiavelli::prompt;
}

GameController::GameController()
{
	std::cout << "GameController ctor\n";
}

bool GameController::startGame()
{
	messageAllPlayers("\33[2JHet spel begint nu");

	goudstukken_ = 30;
	kaartStapel_ = std::make_unique<KaartStapel>();
	loadCharacterCards();

	for (auto iterator = spelers_.begin(); iterator != spelers_.end(); ++iterator) {
		std::shared_ptr<Player> speler = iterator->first;

		// Verdeel bouwkaarten
		for (int i = 0; i < 4; i++)
		{
			speler->addBuildCard(kaartStapel_->getBuildCard());
		}

		// Verdeel goudstukken
		speler->set_gold(2);

		// Toon speler informatie
		messagePlayer(speler, speler->getPlayerInfo());
	}
	
	// Bepaal de koning
	auto it = spelers_.begin();
	std::advance(it, Random::getRandomNumber(0, static_cast<int>(spelers_.size() - 1)));
	koning_ = it->first;

	// Verdeel karakterkaarten
	if (cheat_) {
		cheatDistributeCharacterCards();
	}
	else {
		distributeCharacterCards();
	}

	return true;
}

void GameController::nextPlayer()
{
	auto it = std::find_if(spelers_.begin(), spelers_.end(), [&](std::pair<std::shared_ptr<Player>, std::shared_ptr<Socket>> p)
	{
		return p.first == currentPlayer_;
	});

	++it;

	if (it == spelers_.end())
		it = spelers_.begin();

	currentPlayer_ = it->first;
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
				karakterKaarten_.push_back(std::move(kaart));
		}
	}
}

// DIT GELDT ALLEEN VOOR 2 SPELERS (BIJ 3 SPELERS MOET HET ANDERS)
void GameController::distributeCharacterCards()
{
	currentState_ = GameState::ChooseCharacter;

	for (std::unique_ptr<KarakterKaart>& kaart : discardedKarakterKaarten_)
	{
		karakterKaarten_.push_back(std::move(kaart));
	}
	discardedKarakterKaarten_.clear();

	for (std::pair<std::shared_ptr<Player>, std::shared_ptr<Socket>> pair : spelers_)
	{
		for (std::unique_ptr<KarakterKaart>& kaart : pair.first->getAllCharacterCards())
		{
			karakterKaarten_.push_back(std::move(kaart));
		}
	}

	// Schud de karakterkaarten
	std::shuffle(karakterKaarten_.begin(), karakterKaarten_.end(), Random::getEngine());
	
	// Bepaal de koning
	messageAllPlayers(koning_->get_name() + " is de koning deze ronde.");
	currentPlayer_ = koning_;

	// Gooi 1 karakterkaart weg
	auto last = karakterKaarten_.end() - 1;
	messagePlayer(koning_, "Het karakter " + last->get()->getInfo() + " is weggegooid.");
	discardedKarakterKaarten_.push_back(std::move(*last));
	karakterKaarten_.erase(last, karakterKaarten_.end());
	
	messageAllPlayers(currentPlayer_->get_name() + " moet nu een characterkaart kiezen.");
	promptForCharacterCard();
}

void GameController::promptForCharacterCard()
{
	messagePlayer(currentPlayer_, "Kies een van de volgende characterkaarten: ");
	
	std::stringstream ss = std::stringstream();

	for (std::unique_ptr<KarakterKaart>& kaart : karakterKaarten_)
	{
		ss << kaart->getInfo() << ", ";
	}

	messagePlayer(currentPlayer_, ss.str());
}

void GameController::getCharacterCard(std::string name)
{
	auto it = std::find_if(karakterKaarten_.begin(), karakterKaarten_.end(), [name](std::unique_ptr<KarakterKaart>& k)
	{
		return k.get()->getName() == name;
	});

	if (it == karakterKaarten_.end())
	{
		messagePlayer(currentPlayer_, name + " is geen geldige karakterkaart.");
		promptForCharacterCard();
		return;
	}

	currentPlayer_->addCharacterCard(std::move(*it));
	karakterKaarten_.erase(it);

	if (karakterKaarten_.size() == 1)
	{
		currentState_ = GameState::PlayTurn;
		messageAllPlayers("Alle karakters zijn gekozen. De ronde begint nu");
		// TODO: ronde starten
		return;
	}
	if (karakterKaarten_.size() == 6)
	{
		nextPlayer();
		messageAllPlayers(currentPlayer_->get_name() + " moet nu een characterkaart kiezen.");
		promptForCharacterCard();
		return;
	}

	currentState_ = GameState::RemoveCharacter;
	messageAllPlayers(currentPlayer_->get_name() + " moet nu een characterkaart weggooien.");
	promptForCharacterCard();
}

void GameController::removeCharacterCard(std::string name)
{
	auto it = std::find_if(karakterKaarten_.begin(), karakterKaarten_.end(), [name](std::unique_ptr<KarakterKaart>& k)
	{
		return k.get()->getName() == name;
	});

	if (it == karakterKaarten_.end())
	{
		messagePlayer(currentPlayer_, name + " is geen geldige karakterkaart.");
		promptForCharacterCard();
		return;
	}

	discardedKarakterKaarten_.push_back(std::move(*it));
	karakterKaarten_.erase(it);

	nextPlayer();
	currentState_ = GameState::ChooseCharacter;
	messageAllPlayers(currentPlayer_->get_name() + " moet nu een characterkaart kiezen.");
	promptForCharacterCard();
}

// DIT GELDT ALLEEN VOOR 2 SPELERS (BIJ 3 SPELERS MOET HET ANDERS)
void GameController::cheatDistributeCharacterCards()
{
	if (spelers_.size() == 2) {
		std::vector<std::unique_ptr<KarakterKaart>> currentKarakterKaarten = std::move(karakterKaarten_);
		std::shuffle(currentKarakterKaarten.begin(), currentKarakterKaarten.end(), Random::getEngine());

		// Verdeel karakaterkaarten
		typedef std::map<std::shared_ptr<Player>, std::shared_ptr<Socket>>::iterator it_type;
		for (it_type iterator = spelers_.begin(); iterator != spelers_.end(); ++iterator) {
			addRandomCharacterCard(currentKarakterKaarten, iterator->first);
			addRandomCharacterCard(currentKarakterKaarten, iterator->first);
		}		
	}
}

void GameController::addRandomCharacterCard(std::vector<std::unique_ptr<KarakterKaart>> &currentKarakterKaarten, std::shared_ptr<Player> player)
{
	int index = Random::getRandomNumber(0, static_cast<int>(currentKarakterKaarten.size() - 1));
	std::unique_ptr<KarakterKaart> karakterkaart = std::move(currentKarakterKaarten.at(index));
	player->addCharacterCard(std::move(karakterkaart));
	currentKarakterKaarten.erase(remove(currentKarakterKaarten.begin(), currentKarakterKaarten.end(), karakterkaart), currentKarakterKaarten.end());
}
