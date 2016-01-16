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
#include "Socket.h"
#include <string>

class Player {
public:
	Player() {}
	Player(const std::string& name) : name {name} {}
	
	std::string get_name() const { return name; }
	void set_name(const std::string& new_name) { name = new_name; }

	void addCharacterCard(std::unique_ptr<KarakterKaart> characterCard);
	std::vector<std::unique_ptr<KarakterKaart>> addCharacterCard(std::vector<std::unique_ptr<KarakterKaart>> &currentKarakterKaarten, std::shared_ptr<Socket> &client);
	std::vector<std::unique_ptr<KarakterKaart>> discardCharacterCard(std::vector<std::unique_ptr<KarakterKaart>> &currentKarakterKaarten, std::shared_ptr<Socket> &client);
	void viewCharacterCards(std::shared_ptr<Socket> &client);

private:
	std::string name;
	std::vector<std::unique_ptr<KarakterKaart>> karakterKaarten_;

	std::unique_ptr<KarakterKaart> chooseCharacterCard(std::vector<std::unique_ptr<KarakterKaart>> &currentKarakterKaarten, std::shared_ptr<Socket> &client);
};

#endif /* Player_hpp */
