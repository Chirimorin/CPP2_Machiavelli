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

//vector<unique_ptr<KarakterKaart>> Player::addCharacterCard(vector<unique_ptr<KarakterKaart>> &currentKarakterKaarten)
//{
//	unique_ptr<KarakterKaart> karakterkaart = chooseCharacterCard(currentKarakterKaarten);
//	karakterKaarten_.push_back(move(karakterkaart));
//	currentKarakterKaarten.erase(remove(currentKarakterKaarten.begin(), currentKarakterKaarten.end(), karakterkaart), currentKarakterKaarten.end());
//	return move(currentKarakterKaarten);
//}
//
//vector<unique_ptr<KarakterKaart>> Player::discardCharacterCard(vector<unique_ptr<KarakterKaart>> &currentKarakterKaarten)
//{
//	unique_ptr<KarakterKaart> karakterkaart = chooseCharacterCard(currentKarakterKaarten);
//	currentKarakterKaarten.erase(remove(currentKarakterKaarten.begin(), currentKarakterKaarten.end(), karakterkaart), currentKarakterKaarten.end());
//	return move(currentKarakterKaarten);
//}
//
//unique_ptr<KarakterKaart> Player::chooseCharacterCard(vector<unique_ptr<KarakterKaart>> &currentKarakterKaarten)
//{
//	*client << "De karakterkaarten zijn:\r\n";
//	for (unique_ptr<KarakterKaart> & kaart : currentKarakterKaarten) {
//		*client << kaart->getInfo();
//		&kaart != &currentKarakterKaarten.back() ? *client << ", " : *client << "\r\n";
//	}
//	*client << "Welke karakterkaart wil je hebben?\r\n";
//
//	string kaartNaam = client->readline();
//	*client << "Ingetypt: " << kaartNaam << "\r\n"; // MOET WEG
//
//	auto it = find_if(currentKarakterKaarten.begin(), currentKarakterKaarten.end(), [kaartNaam](unique_ptr<KarakterKaart>& kaart) {return kaart->getName() == kaartNaam; });
//	cout << "Ingetypt: " << kaartNaam << "\r\n"; // MOET WEG
//
//	while (it == currentKarakterKaarten.end()) {
//		*client << "De ingevoerde karakterkaart wordt niet herkent. Probeer het opnieuw.\r\n";
//		kaartNaam = client->readline();
//		it = find_if(currentKarakterKaarten.begin(), currentKarakterKaarten.end(), [kaartNaam](unique_ptr<KarakterKaart>& kaart) {return kaart->getName() == kaartNaam; });
//	}
//
//	return move(*it);
//}
//
//void Player::viewCharacterCards(std::shared_ptr<Socket> &client) {
//	*client << "Je hebt de volgende karakterkaarten: \r\n";
//	for (unique_ptr<KarakterKaart> & kaart : karakterKaarten_) {
//		*client << kaart->getName();
//		&kaart != &karakterKaarten_.back() ? *client << ", " : *client << ".\r\n";
//	}
//}
//
//void Player::viewBuildCards(std::shared_ptr<Socket> &client) {
//	*client << "Je hebt de volgende bouwkaarten: \r\n";
//	for (unique_ptr<BouwKaart> & kaart : bouwKaarten_) {
//		*client << kaart->getInfo();
//		&kaart != &bouwKaarten_.back() ? *client << ", " : *client << ".\r\n";
//	}
//}
//
//void Player::viewGold(std::shared_ptr<Socket> &client) {
//	*client << "Je hebt op dit moment " << goudstukken_ << " goudstuk(ken).\r\n";
//}
//
//void Player::viewAllPlayerInfo(std::shared_ptr<Socket> &client) {
//	viewCharacterCards(client);
//	viewBuildCards(client);
//	viewGold(client);
//}