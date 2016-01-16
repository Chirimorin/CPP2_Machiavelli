#include "stdafx.h"
#include "GameController.h"
#include "ClientCommand.h"
#include "Player.hpp"
#include "Socket.h"
#include "Random.h"

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

	if (command.get_cmd() == "start")
	{
		if (spelers_.size() >= 2)
			return startGame();
	}

	return false;
}

void GameController::messageAllPlayers(std::string message)
{
	for (std::pair<std::shared_ptr<Player>, std::shared_ptr<Socket>> pair : spelers_)
	{
		*pair.second << message << "\r\n";
	}
}

GameController::GameController()
{
	std::cout << "GameController ctor\n";
}

bool GameController::startGame()
{
	if (gameStarted_)
		return false;

	gameStarted_ = true;
	kaartStapel_ = std::make_unique<KaartStapel>();
	loadCharacterCards();

	// Bepaal de koning
	auto it = spelers_.begin();
	std::advance(it, Random::getRandomNumber(0, static_cast<int>(spelers_.size()-1)));
	koning_ = it->first;

	messageAllPlayers("Speler " + koning_->get_name() + " is de koning.");

	distributeCharacterCards();

	return true;
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
	if (spelers_.size() == 2) {
		std::vector<std::unique_ptr<KarakterKaart>> currentKarakterKaarten = std::move(karakterKaarten_);
		std::shuffle(currentKarakterKaarten.begin(), currentKarakterKaarten.end(), Random::getEngine());

		// De koning bekijkt de bovenste karakterkaart en legt deze gedekt op tafel
		*spelers_.find(koning_)->second << "De afgelegde karakterkaart is: " << currentKarakterKaarten.at(0)->getName() << "\r\n";
		currentKarakterKaarten.erase(std::remove(currentKarakterKaarten.begin(), currentKarakterKaarten.end(), currentKarakterKaarten.at(0)), currentKarakterKaarten.end());

		// De koning kiest 1 van de 7 overgebleven karakterkaarten
		currentKarakterKaarten = koning_->addCharacterCard(currentKarakterKaarten, spelers_.find(koning_)->second);

		// TODO: Moet op een betere manier
		// Van de 6 overgebleven karakterkaarten kiest de andere speler 1 kaart en legt 1 kaart gedekt op tafel
		typedef std::map<std::shared_ptr<Player>, std::shared_ptr<Socket>>::iterator it_type;
		for (it_type iterator = spelers_.begin(); iterator != spelers_.end(); iterator++) {
			if (iterator->first != koning_) {
				currentKarakterKaarten = iterator->first->addCharacterCard(currentKarakterKaarten, iterator->second);
				currentKarakterKaarten = iterator->first->discardCharacterCard(currentKarakterKaarten, iterator->second);
			}
		}

		// Van de 4 overgebleven karakterkaarten kiest de koning  1 kaart en legt 1 kaart gedekt op tafel
		currentKarakterKaarten = koning_->addCharacterCard(currentKarakterKaarten, spelers_.find(koning_)->second);
		currentKarakterKaarten = koning_->discardCharacterCard(currentKarakterKaarten, spelers_.find(koning_)->second);

		// TODO: Moet op een betere manier
		// Van de 2 overgebleven karakterkaarten kiest de andere speler 1 kaart en legt 1 kaart gedekt op tafel
		typedef std::map<std::shared_ptr<Player>, std::shared_ptr<Socket>>::iterator it_type;
		for (it_type iterator = spelers_.begin(); iterator != spelers_.end(); iterator++) {
			if (iterator->first != koning_) {
				currentKarakterKaarten = iterator->first->addCharacterCard(currentKarakterKaarten, iterator->second);
				currentKarakterKaarten = iterator->first->discardCharacterCard(currentKarakterKaarten, iterator->second);
			}
		}
	}

}
