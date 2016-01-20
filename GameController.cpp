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
		if (command.get_cmd() == "end turn")
		{
			nextPlayer();
			return true;
		}
		// TODO: handle command voor de beurt van een speler
		if (command.get_cmd() == "goud")
		{
			if (currentPlayer_->hasChosenGoldOrBuidCard()) {
				return false;
			}
			addGold();
			currentPlayer_->setHasChosenGoldOrBuidCard(true);
			return true;
		}
		if (command.get_cmd() == "bouwkaart")
		{
			if (currentPlayer_->hasChosenGoldOrBuidCard()) {
				return false;
			}
			chooseNewBuildCard();
			currentPlayer_->setHasChosenGoldOrBuidCard(true);
			return true;
		}
		if (command.get_cmd() == "bouwen")
		{
			// TODO: boukaart neerleggen
			return true;
		}
		if (command.get_cmd() == "eigenschap")
		{
			// TODO: gebruik karaktereigenschap
			return true;
		}
	}
	else if (currentState_ == GameState::ChooseNewBuildCard &&
		currentPlayer_ == player)
	{
		getNewBuildCard(command.get_cmd());
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
		messagePlayer(speler, speler->getBuildCardInfo());
		messagePlayer(speler, speler->getGoldInfo());
	}
	
	// Bepaal de koning
	auto it = spelers_.begin();
	std::advance(it, Random::getRandomNumber(0, static_cast<int>(spelers_.size() - 1)));
	koning_ = it->first;

	// Verdeel karakterkaarten
	distributeCharacterCards();

	return true;
}

void GameController::nextPlayer()
{
	if (currentState_ == GameState::ChooseCharacter ||
		currentState_ == GameState::RemoveCharacter)
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
	else
	{
		++currentCharacter_;
		if (currentCharacter_ > 8)
		{
			messageAllPlayers("Alle characters zijn aan de beurt geweest.");
			distributeCharacterCards();
			return;
		}
		auto it = std::find_if(spelers_.begin(), spelers_.end(), [&](std::pair<std::shared_ptr<Player>, std::shared_ptr<Socket>> p)
		{
			return p.first->hasCharacterCard(currentCharacter_);
		});

		if (it == spelers_.end())
		{
			nextPlayer();
		}
		else
		{
			currentPlayer_ = it->first;
			newTurn();
		}
	}
		
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
	if (cheat_)
	{
		cheatDistributeCharacterCards();
		return;
	}

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
		messageAllPlayers("Alle karakters zijn gekozen. De ronde begint nu");
		startRound();
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
		std::shuffle(karakterKaarten_.begin(), karakterKaarten_.end(), Random::getEngine());

		// Verdeel karakaterkaarten
		for (auto iterator = spelers_.begin(); iterator != spelers_.end(); ++iterator) {
			for (int i = 0; i < 2; ++i)
			{
				auto cardIt = --karakterKaarten_.end();
				iterator->first->addCharacterCard(std::move(*cardIt));
				karakterKaarten_.erase(cardIt);
			}
		}

		messageAllPlayers("Karakterkaarten zijn automatisch verdeeld");

		startRound();
	}
}

void GameController::startRound()
{
	currentState_ = GameState::PlayTurn;
	currentCharacter_ = 0;
	nextPlayer();
}

void GameController::newTurn()
{
	messageAllPlayers(currentPlayer_->get_name() + ", de " + currentPlayer_->getCharacterInfo(currentCharacter_) + ", is nu aan de beurt.");
	messagePlayer(currentPlayer_, currentPlayer_->getPlayerInfo());

	// TODO: info over mogelijkheden laten zien
	promptForChoiseGoldOrBuildCard();

	// TODO: zorgen dat ze speler meerdere acties kan doen tijdens zijn beurt in plaats van 1:
	// - 2 goudstukken of bouwkaart
	// - bouwen
	// - karakter eigenschap gebruiken
}

void GameController::addRandomCharacterCard(std::vector<std::unique_ptr<KarakterKaart>> &currentKarakterKaarten, std::shared_ptr<Player> player)
{
	int index = Random::getRandomNumber(0, static_cast<int>(currentKarakterKaarten.size() - 1));
	std::unique_ptr<KarakterKaart> karakterkaart = std::move(currentKarakterKaarten.at(index));
	player->addCharacterCard(std::move(karakterkaart));
	currentKarakterKaarten.erase(remove(currentKarakterKaarten.begin(), currentKarakterKaarten.end(), karakterkaart), currentKarakterKaarten.end());
}

void GameController::promptForChoiseGoldOrBuildCard()
{
	// TODO: alleen de mogelijke opties tonen

	messagePlayer(currentPlayer_, "Wil je 2 goudstukken, een bouwkaart of je karaktereigenschap gebruiken?\r\n[goud | bouwkaart | eigenschap]");
}

void GameController::addGold()
{
	goudstukken_ -= 2;
	messagePlayer(currentPlayer_, currentPlayer_->addGold(2));
}

void GameController::chooseNewBuildCard()
{
	currentState_ = GameState::ChooseNewBuildCard;

	nieuweBouwKaart1_ = kaartStapel_->getBuildCard();
	nieuweBouwKaart2_ = kaartStapel_->getBuildCard();

	promptForGetNewBuildCard();
}

void GameController::promptForGetNewBuildCard()
{
	std::string result = "Kies een van de volgende bouwkaarten:\r\n" + nieuweBouwKaart1_->getName() + ", " + nieuweBouwKaart2_->getName();
	messagePlayer(currentPlayer_, result);
}

void GameController::getNewBuildCard(std::string name)
{
	if (name == nieuweBouwKaart1_->getName()) {
		currentPlayer_->addBuildCard(std::move(nieuweBouwKaart1_));
		kaartStapel_->addBuildCard(std::move(nieuweBouwKaart2_));

		nieuweBouwKaart1_ = nullptr;
		nieuweBouwKaart2_ = nullptr;
	}
	else if (name == nieuweBouwKaart2_->getName()) {
		currentPlayer_->addBuildCard(std::move(nieuweBouwKaart2_));
		kaartStapel_->addBuildCard(std::move(nieuweBouwKaart1_));

		nieuweBouwKaart1_ = nullptr;
		nieuweBouwKaart2_ = nullptr;
	}
	else {
		messagePlayer(currentPlayer_, name + " is geen geldige bouwkaart.");
		promptForGetNewBuildCard();
		return;
	}
}

