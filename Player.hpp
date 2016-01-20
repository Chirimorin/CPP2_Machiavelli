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
	std::string getBuildingInfo();
	std::string getBuildCardInfo();
	std::string getGoldInfo();

	std::vector<std::unique_ptr<KarakterKaart>> getAllCharacterCards();
	bool hasCharacterCard(int number);
	std::string getCharacterInfo(int number);
	std::string addGold(int gold);

	std::string useAbility(int currentCharacter);
	std::string buildCard(std::string card);

	std::string newTurn(int currentCharacter);
private:
	std::string name;
	int goudstukken_ = 0;
	std::vector<std::unique_ptr<KarakterKaart>> karakterKaarten_;
	std::vector<std::unique_ptr<BouwKaart>> bouwKaarten_;
	std::vector<std::unique_ptr<BouwKaart>> gebouwen_;

	int maxBuilds_ = 1;
	bool usedAbility_ = false;

	std::string getGoldForColor(std::string color);
};

#endif /* Player_hpp */
