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
	else if (currentPlayer_ == player)
	{
		if (currentState_ == GameState::ChooseCharacter)
		{
			getCharacterCard(command.get_cmd());
			return true;
		}
		if (currentState_ == GameState::RemoveCharacter)
		{
			removeCharacterCard(command.get_cmd());
			return true;
		}
		if (currentState_ == GameState::ChooseGoldOrCard)
		{
			if (command.get_cmd() == "goud")
			{
				addGold();
				return true;
			}
			if (command.get_cmd() == "bouwkaart")
			{
				chooseNewBuildCard();
				return true;
			}
		}
		if (currentState_ == GameState::PickBuildCard)
		{
			getNewBuildCard(command.get_cmd());
			return true;
		}
	
		if (command.get_cmd() == "end turn")
		{
			// Check of de speler als eerste 8 gebouwen heeft
			if (winnaar_ == nullptr && currentPlayer_->hasEightOrMoreBuildings()) {
				winnaar_ = currentPlayer_;
			}
			nextPlayer();
			return true;
		}
		
		if (command.get_cmd().length() > 5 &&
			command.get_cmd().substr(0,5) == "bouw ")
		{
			buildCard(command.get_cmd().substr(5));
			return true;
		}
		if (command.get_cmd() == "karaktereigenschap")
		{
			useAbility();
			return true;
		}
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

	winnaar_ = nullptr;
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

			if (winnaar_ != nullptr)
				endGame();
			else
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
	// Pak de afgelegde karakterkaarten terug
	for (std::unique_ptr<KarakterKaart>& kaart : discardedKarakterKaarten_)
	{
		karakterKaarten_.push_back(std::move(kaart));
	}
	discardedKarakterKaarten_.clear();

	// Pak de gekozen karakterkaarten terug
	for (std::pair<std::shared_ptr<Player>, std::shared_ptr<Socket>> pair : spelers_)
	{
		for (std::unique_ptr<KarakterKaart>& kaart : pair.first->getAllCharacterCards())
		{
			karakterKaarten_.push_back(std::move(kaart));
		}
	}

	// Schud de karakterkaarten
	std::shuffle(karakterKaarten_.begin(), karakterKaarten_.end(), Random::getEngine());

	if (cheat_)
	{
		cheatDistributeCharacterCards();
		return;
	}

	currentState_ = GameState::ChooseCharacter;

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
	currentState_ = GameState::ChooseGoldOrCard;
	messageAllPlayers(currentPlayer_->get_name() + ", de " + currentPlayer_->getCharacterInfo(currentCharacter_) + ", is nu aan de beurt.");
	messagePlayer(currentPlayer_, currentPlayer_->getPlayerInfo());
	messagePlayer(currentPlayer_, currentPlayer_->newTurn(currentCharacter_));

	messagePlayer(currentPlayer_, "Wil je 2 goudstukken, een bouwkaart of je karaktereigenschap gebruiken?\r\n[goud | bouwkaart | eigenschap]");

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

void GameController::addGold()
{
	goudstukken_ -= 2;
	messageAllPlayers(currentPlayer_->get_name() + " pakt 2 goud.");
	messagePlayer(currentPlayer_, currentPlayer_->addGold(2));

	promptPlayTurn();
}

void GameController::chooseNewBuildCard()
{
	currentState_ = GameState::PickBuildCard;

	for (int i = 0; i < 2; ++i)
	{
		mogelijkeNieuweBouwkaarten_.push_back(std::move(kaartStapel_->getBuildCard()));
	}

	promptForGetNewBuildCard();
}

void GameController::promptForGetNewBuildCard()
{
	if (currentCharacter_ == 7) // bouwmeester krijgt beide kaarten
	{
		std::string result = "Je pakt de volgende bouwkaarten:\r\n";
		for (auto iter = mogelijkeNieuweBouwkaarten_.begin(); iter < mogelijkeNieuweBouwkaarten_.end(); ++iter)
		{
			result += (*iter)->getInfo() + ", ";
			currentPlayer_->addBuildCard(std::move(*iter));
		}
		mogelijkeNieuweBouwkaarten_.clear();

		messageAllPlayers(currentPlayer_->get_name() + " pakt twee bouwkaarten.");
		messagePlayer(currentPlayer_, result);
		messagePlayer(currentPlayer_, currentPlayer_->getBuildCardInfo());

		promptPlayTurn();
		return;
	}

	std::string result = "Kies een van de volgende bouwkaarten:\r\n";
	for (auto it = mogelijkeNieuweBouwkaarten_.begin(); it < mogelijkeNieuweBouwkaarten_.end(); ++it)
	{
		result += (*it)->getInfo() + ", ";
	}
	messagePlayer(currentPlayer_, result);
}

void GameController::getNewBuildCard(std::string name)
{
	auto it = std::find_if(mogelijkeNieuweBouwkaarten_.begin(), mogelijkeNieuweBouwkaarten_.end(), [&name](std::unique_ptr<BouwKaart>& k)
	{
		return k->getName() == name;
	});

	if (it == mogelijkeNieuweBouwkaarten_.end())
	{
		messagePlayer(currentPlayer_, name + " is geen geldige bouwkaart.");
		promptForGetNewBuildCard();
	}
	else
	{
		currentPlayer_->addBuildCard(std::move(*it));
		mogelijkeNieuweBouwkaarten_.erase(it);

		for (auto iter = mogelijkeNieuweBouwkaarten_.begin(); iter < mogelijkeNieuweBouwkaarten_.end(); ++iter)
		{
			kaartStapel_->addBuildCard(std::move(*iter));
		}
		mogelijkeNieuweBouwkaarten_.clear();

		messageAllPlayers(currentPlayer_->get_name() + " pakt een bouwkaart.");
		messagePlayer(currentPlayer_, currentPlayer_->getBuildCardInfo());

		promptPlayTurn();
	}
}

void GameController::promptPlayTurn()
{
	currentState_ = GameState::PlayTurn;
	messagePlayer(currentPlayer_, "Kies een optie:\r\n( bouw [kaart] | karaktereigenschap | end turn ]");
}

void GameController::buildCard(std::string card)
{
	messagePlayer(currentPlayer_, currentPlayer_->buildCard(card));
}

void GameController::useAbility()
{
	messagePlayer(currentPlayer_, currentPlayer_->useAbility(currentCharacter_));
}

void GameController::endGame()
{
	std::set<std::shared_ptr<Player>, less_than_by_score> leaderboard;

	for (auto iterator = spelers_.begin(); iterator != spelers_.end(); ++iterator) {
		iterator->first->calculateScore(iterator->first == winnaar_);
		leaderboard.insert(iterator->first);
	}

	messageAllPlayers("Het spel is geeindigd. Eindscores:");
	int position = 1;
	for (auto it = leaderboard.begin(); it != leaderboard.end(); ++it)
	{
		messageAllPlayers(std::to_string(position) + ": " + (*it)->get_name() + ", score: " + std::to_string((*it)->getScore()));
		++position;
	}

	currentState_ = GameState::Ended;
}