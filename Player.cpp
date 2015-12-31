//
//  Player.cpp
//  socketexample
//
//  Created by Bob Polis on 23-11-15.
//  Copyright © 2015 Avans Hogeschool, 's-Hertogenbosch. All rights reserved.
//

#include "stdafx.h"
#include "Player.hpp"
using namespace std;

std::vector<std::unique_ptr<KarakterKaart>> Player::addCharacterCard(std::vector<std::unique_ptr<KarakterKaart>> &currentKarakterKaarten, std::shared_ptr<Socket> &client)
{
	std::unique_ptr<KarakterKaart> karakterkaart = chooseCharacterCard(currentKarakterKaarten, client);
	karakterKaarten_.push_back(std::move(karakterkaart));
	currentKarakterKaarten.erase(std::remove(currentKarakterKaarten.begin(), currentKarakterKaarten.end(), karakterkaart), currentKarakterKaarten.end());
	return std::move(currentKarakterKaarten);
}

std::vector<std::unique_ptr<KarakterKaart>> Player::discardCharacterCard(std::vector<std::unique_ptr<KarakterKaart>> &currentKarakterKaarten, std::shared_ptr<Socket> &client)
{
	std::unique_ptr<KarakterKaart> karakterkaart = chooseCharacterCard(currentKarakterKaarten, client);
	currentKarakterKaarten.erase(std::remove(currentKarakterKaarten.begin(), currentKarakterKaarten.end(), karakterkaart), currentKarakterKaarten.end());
	return std::move(currentKarakterKaarten);
}

std::unique_ptr<KarakterKaart> Player::chooseCharacterCard(std::vector<std::unique_ptr<KarakterKaart>> &currentKarakterKaarten, std::shared_ptr<Socket> &client)
{
	*client << "De karakterkaarten zijn:\r\n";
	for (std::unique_ptr<KarakterKaart> & kaart : currentKarakterKaarten) {
		*client << kaart->getName();
		&kaart != &currentKarakterKaarten.back() ? *client << ", " : *client << "\r\n";
	}
	*client << "Welke karakterkaart wil je hebben?\r\n";

	std::string kaartNaam = client->readline();
	*client << "Ingetypt: " << kaartNaam << "\r\n"; // MOET WEG

	auto it = std::find_if(currentKarakterKaarten.begin(), currentKarakterKaarten.end(), [kaartNaam](std::unique_ptr<KarakterKaart>& kaart) {return kaart->getName() == kaartNaam; });
	std::cout << "Ingetypt: " << kaartNaam << "\r\n"; // MOET WEG

	while (it == currentKarakterKaarten.end()) {
		*client << "De ingevoerde karakterkaart wordt niet herkent. Probeer het opnieuw.\r\n";
		kaartNaam = client->readline();
		it = std::find_if(currentKarakterKaarten.begin(), currentKarakterKaarten.end(), [kaartNaam](std::unique_ptr<KarakterKaart>& kaart) {return kaart->getName() == kaartNaam; });
	}

	return std::move(*it);
}

