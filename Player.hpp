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

class Player {
public:
	Player() {}
	Player(const std::string& name) : name {name} {}

	std::string get_name() const { return name; }
	void set_name(const std::string& new_name) { name = new_name; }

	int get_gold() const { return goudstukken_; }
	void set_gold(const int& new_gold) { goudstukken_ = new_gold; }

	void addBuildCard(std::unique_ptr<BouwKaart> buildCard);
	std::unique_ptr<BouwKaart> takeBuildCard();
	void addCharacterCard(std::unique_ptr<KarakterKaart> characterCard);

	std::string getPlayerInfo();
	std::string getCharacterCardInfo();
	std::string getBuildingInfo(bool isCurrentPlayer = true);
	std::string getBuildCardInfo();
	std::string getGoldInfo();

	std::vector<std::unique_ptr<KarakterKaart>> getAllCharacterCards();
	bool hasCharacterCard(int number);
	std::string getCharacterInfo(int number);
	std::string addGold(int gold);

	std::string useAbility(int currentCharacter);
	void cheatBuildCard(std::unique_ptr<BouwKaart> b);
	std::string buildCard(std::string card);

	std::string newTurn(int currentCharacter);

	int getAmountOfBuildCards();
	std::vector<std::unique_ptr<BouwKaart>> getAllBuildCards();
	int getAmountOfBuildings() { return gebouwen_.size(); }
	bool hasEightOrMoreBuildings();
	void calculateScore(bool isWinner);
	int getScore() const { return score_; }
	int getRawScore() const { return rawScore_; }

	bool tryDestroyBuilding(std::string name, int& gold);

	void newGame();
private:
	std::string name;
	int goudstukken_ = 0;
	int rawScore_ = 0;
	int score_ = 0;

	std::vector<std::unique_ptr<KarakterKaart>> karakterKaarten_;
	std::vector<std::unique_ptr<BouwKaart>> bouwKaarten_;
	std::vector<std::unique_ptr<BouwKaart>> gebouwen_;

	int maxBuilds_ = 1;
	bool usedAbility_ = false;

	std::string getGoldForColor(std::string color);
};

struct less_than_by_score
{
	bool operator() (const std::shared_ptr<Player>& p1, const std::shared_ptr<Player> & p2) const {
		if (p1->getScore() == p2->getScore())
		{
			return p1->getRawScore() > p2->getRawScore();
		}

		return p1->getScore() > p2->getScore();
	}
};

#endif /* Player_hpp */
