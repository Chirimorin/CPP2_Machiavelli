#pragma once
class Player;
class Socket;
class KaartStapel;

class GameController
{
public:
	static GameController& getInstance();

	void addPlayer(std::shared_ptr<Player> player, std::shared_ptr<Socket> client);
	void removePlayer(std::shared_ptr<Player> player);

	
private:
	GameController();
	~GameController() {};
	
	std::unique_ptr<KaartStapel> kaartStapel_;
	std::map<std::shared_ptr<Player>, std::shared_ptr<Socket>> spelers_;
};
