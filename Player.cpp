//
//  Player.cpp
//  socketexample
//
//  Created by Bob Polis on 23-11-15.
//  Copyright © 2015 Avans Hogeschool, 's-Hertogenbosch. All rights reserved.
//

#include "stdafx.h"
#include "Player.hpp"
#include "GameController.h"
using namespace std;

void Player::addBuildCard(std::unique_ptr<BouwKaart> buildCard) {
	bouwKaarten_.push_back(move(buildCard));
}

std::unique_ptr<BouwKaart> Player::takeBuildCard()
{
	auto it = bouwKaarten_.begin();
	std::unique_ptr<BouwKaart> result = std::move(*it);
	bouwKaarten_.erase(it);
	return result;
}

void Player::addCharacterCard(std::unique_ptr<KarakterKaart> characterCard) {
	karakterKaarten_.push_back(move(characterCard));
}

std::string Player::getPlayerInfo()
{
	return getCharacterCardInfo() + "\r\n" + getBuildingInfo() + "\r\n" + getBuildCardInfo() + "\r\n" + getGoldInfo();
}

std::string Player::getCharacterCardInfo()
{
	std::string result = "Je hebt de volgende karakterkaarten: \r\n";

	for (unique_ptr<KarakterKaart>& kaart : karakterKaarten_) {
		result += kaart->getInfo();
		if (&kaart != &karakterKaarten_.back())
			result += ", ";
	}

	return result;
}

std::string Player::getBuildingInfo(bool isCurrentPlayer)
{
	std::string result = "";

	if (isCurrentPlayer)
		result = "Je hebt de volgende gebouwen: \r\n";
	else
		result = "De tegenstander heeft de volgende gebouwen: \r\n";

	for (unique_ptr<BouwKaart> & kaart : gebouwen_) {
		result += kaart->getInfo();
		if (&kaart != &gebouwen_.back())
			result += ", ";
	}

	return result;
}

std::string Player::getBuildCardInfo()
{
	std::string result = "Je hebt de volgende bouwkaarten: \r\n";

	for (unique_ptr<BouwKaart> & kaart : bouwKaarten_) {
		result += kaart->getInfo();
		if (&kaart != &bouwKaarten_.back())
			result += ", ";
	}

	return result;
}

std::string Player::getGoldInfo()
{
	return "Je hebt momenteel " + std::to_string(goudstukken_) + " goudstuk" + (goudstukken_ == 1 ? "" : "ken");
}

std::vector<std::unique_ptr<KarakterKaart>> Player::getAllCharacterCards()
{
	std::vector<std::unique_ptr<KarakterKaart>> result = std::move(karakterKaarten_);
	karakterKaarten_.clear();
	return result;
}

bool Player::hasCharacterCard(int number)
{
	for (unique_ptr<KarakterKaart> & kaart : karakterKaarten_) {
		if (kaart->getNumber() == number)
			return true;
	}

	return false;
}

std::string Player::getCharacterInfo(int number)
{
	for (unique_ptr<KarakterKaart> & kaart : karakterKaarten_) {
		if (kaart->getNumber() == number)
			return kaart->getInfo();
	}

	return "SPELER HEEFT CHARACTER " + std::to_string(number) + " NIET!";
}

std::string Player::addGold(int gold)
{
	goudstukken_ += gold;
	return "Je hebt nu " + std::to_string(goudstukken_) + " goudstuk" + (goudstukken_ == 1 ? "" : "ken");
}

std::string Player::useAbility(int currentCharacter)
{
	if (usedAbility_)
	{
		return "Je hebt je karaktereigenschap al gebruikt deze beurt.";
	}
	usedAbility_ = true;

	switch (currentCharacter)
	{
	case 1: // Moordenaar
		GameController::getInstance().killCharacter();
		break;
	case 2: // Dief
		GameController::getInstance().robCharacter();
		break;
	case 3: // Magier
		GameController::getInstance().mageAbility();
		break;
	case 4: // Koning
		return getGoldForColor("geel");
	case 5: // Prediker
		return getGoldForColor("blauw");
	case 6: // Koopman
		return getGoldForColor("groen");
	case 7: // Bouwmeester
		// Bouwmeester heeft geen actieve ability
		return "De bouwmeester heeft geen karaktereigenschap om te gebruiken.";
	case 8: // Condotierre
		GameController::getInstance().destroyBuilding();
		return getGoldForColor("rood");
	}

	return "";
}

void Player::cheatBuildCard(std::unique_ptr<BouwKaart> b)
{
	gebouwen_.push_back(std::move(b));
}

std::string Player::buildCard(std::string card)
{
	if (maxBuilds_ <= 0)
	{
		return "Je mag deze beurt niet meer bouwen.";
	}

	auto it = std::find_if(bouwKaarten_.begin(), bouwKaarten_.end(), [&card](std::unique_ptr<BouwKaart>& b)
	{
		return b->getName() == card;
	});

	if (it == bouwKaarten_.end())
	{
		return "'" + card + "' is geen geldige bouwkaart.";
	}

	auto it2 = std::find_if(gebouwen_.begin(), gebouwen_.end(), [&card](std::unique_ptr<BouwKaart>& b)
	{
		return b->getName() == card;
	});

	if (it2 != gebouwen_.end())
	{
		return "Je hebt al een " + card + " in je stad. Je mag maar 1 gebouw van elk type hebben.";
	}

	if (goudstukken_ >= (*it)->getPrice())
	{
		GameController::getInstance().messageAllPlayers(name + " bouwt een " + card + " voor " + std::to_string((*it)->getPrice()) + " goud.");
		goudstukken_ -= (*it)->getPrice();
		gebouwen_.push_back(std::move(*it));
		bouwKaarten_.erase(it);
		--maxBuilds_;

		return getGoldInfo();
	}

	return "Je hebt niet genoeg goud om " + card + " te bouwen.";
}

std::string Player::newTurn(int currentCharacter)
{
	maxBuilds_ = 1;
	usedAbility_ = false;

	switch (currentCharacter)
	{
	case 6: // koopman
		goudstukken_++;
		return "Als koopman heb je 1 extra goud gekregen.\r\n" + getGoldInfo();
	case 7: // bouwmeester
		maxBuilds_ += 2;
		break;
	}

	return "Je krijgt geen extra goud deze beurt.";
}

int Player::getAmountOfBuildCards()
{
	return bouwKaarten_.size();
}

std::vector<std::unique_ptr<BouwKaart>> Player::getAllBuildCards()
{
	std::vector<std::unique_ptr<BouwKaart>> result = std::move(bouwKaarten_);
	bouwKaarten_.clear();
	return result;
}

std::string Player::getGoldForColor(std::string color)
{
	int numGold = std::count_if(gebouwen_.begin(), gebouwen_.end(), [color](std::unique_ptr<BouwKaart>& g)
	{
		return g->getColor() == color;
	});
	goudstukken_ += numGold;

	return "Je hebt " + std::to_string(numGold) + " goud gekregen voor je gebouwen die " + color + " zijn.\r\n" + getGoldInfo();
}

bool Player::hasEightOrMoreBuildings()
{
	if (gebouwen_.size() > 7) {
		return true;
	}
	return false;
}

void Player::calculateScore(bool isWinner)
{
	score_ = 0;

	std::set<std::string> colors;

	// Alle punten voor de gebouwen in zijn stad
	for (auto iterator = gebouwen_.begin(); iterator != gebouwen_.end(); ++iterator) {
		score_ += (*iterator)->getPrice();
		colors.insert((*iterator)->getColor());
	}

	rawScore_ = score_;

	// 3 punten als de speler gebouwen van alle 5 kleuren bezit
	if (colors.size() >= 5)
	{
		score_ += 3;
	}

	if (gebouwen_.size() >= 8)
	{
		score_ += 2;
	}
	if (isWinner)
	{
		score_ += 2; // 2 extra punten, totaal 4
	}
}

bool Player::tryDestroyBuilding(std::string name, int& gold)
{
	auto it = std::find_if(gebouwen_.begin(), gebouwen_.end(), [name](std::unique_ptr<BouwKaart>& b)
	{
		return name == b->getName();
	});

	if (it == gebouwen_.end())
	{
		GameController::getInstance().messageCurrentPlayer("Je tegenstander heeft geen " + name + " in zijn stad.");
		return false;
	}

	if ((*it)->getPrice() - 1 > gold)
	{
		GameController::getInstance().messageCurrentPlayer("Je hebt niet genoeg goud om een " + name + " te slopen.");
		return false;
	}

	gold -= ((*it)->getPrice() - 1);
	GameController::getInstance().addBuildCard(std::move(*it));
	gebouwen_.erase(it);
	GameController::getInstance().messageAllPlayers("De condottiere sloopt de " + name + " van " + get_name() + ".");

	return true;
}

void Player::newGame()
{
	goudstukken_ = 2;
	rawScore_ = 0;
	score_ = 0;
	gebouwen_.clear();
	bouwKaarten_.clear();
	karakterKaarten_.clear();
}