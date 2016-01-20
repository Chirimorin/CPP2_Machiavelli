//
//  Player.hpp
//  socketexample
//
//  Created by Bob Polis on 23-11-15.
//  Copyright © 2015 Avans Hogeschool, 's-Hertogenbosch. All rights reserved.
//

#ifndef Player_hpp
#define Player_hpp

#include "KarakterKaart.h"
#include "BouwKaart.h"
#include "Socket.h"
#include <string>

class Player {
public:
	Player() {}
	Player(const std::string& name) : name {name} {}
	
	std::string get_name() const { return name; }
	void set_name(const std::string& new_name) { name = new_name; }

	int get_gold() const { return goudstukken_; }
	void set_gold(const int& new_gold) { goudstukken_ = new_gold; }

	void addBuildCard(std::unique_ptr<BouwKaart> buildCard);
	void addCharacterCard(std::unique_ptr<KarakterKaart> characterCard);

	std::string getPlayerInfo();
	std::string getCharacterCardInfo();
	std::string getBuildCardInfo();
	std::string getGoldInfo();

	std::vector<std::unique_ptr<KarakterKaart>> getAllCharacterCards();
	bool hasCharacterCard(int number);
	std::string getCharacterInfo(int number);

	//std::vector<std::unique_ptr<KarakterKaart>> addCharacterCard(std::vector<std::unique_ptr<KarakterKaart>> &currentKarakterKaarten);
	//std::vector<std::unique_ptr<KarakterKaart>> discardCharacterCard(std::vector<std::unique_ptr<KarakterKaart>> &currentKarakterKaarten);
	//
	//void viewCharacterCards();
	//void viewBuildCards();
	//void viewGold();
	//void viewAllPlayerInfo();

	std::string addGold(int gold);
	void setHasChosenGoldOrBuidCard(bool hasChosenGoldOrBuidCard) { hasChosenGoldOrBuidCard_ = hasChosenGoldOrBuidCard;  }
	bool hasChosenGoldOrBuidCard() { return hasChosenGoldOrBuidCard_; }

private:
	std::string name;
	int goudstukken_ = 0;
	std::vector<std::unique_ptr<KarakterKaart>> karakterKaarten_;
	std::vector<std::unique_ptr<BouwKaart>> bouwKaarten_;

	//std::unique_ptr<KarakterKaart> chooseCharacterCard(std::vector<std::unique_ptr<KarakterKaart>> &currentKarakterKaarten);

	bool hasChosenGoldOrBuidCard_ = false;

};

#endif /* Player_hpp */
