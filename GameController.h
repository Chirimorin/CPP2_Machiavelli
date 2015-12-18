#pragma once
class KaartStapel;

class GameController
{
public:
	static GameController& getInstance();
private:
	GameController();
	~GameController() {};
	
	std::unique_ptr<KaartStapel> kaartStapel_;
};

