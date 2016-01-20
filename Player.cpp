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

void Player::addCharacterCard(std::unique_ptr<KarakterKaart> characterCard) {
	karakterKaarten_.push_back(move(characterCard));
}

std::string Player::getPlayerInfo()
{
	return getCharacterCardInfo() + "\r\n" + getBuildCardInfo() + "\r\n" + getGoldInfo();
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