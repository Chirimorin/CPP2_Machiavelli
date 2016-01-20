#pragma once
#include "BouwKaart.h"

class KaartStapel
{
public:
	KaartStapel();
	~KaartStapel() { std::cout << "KaartStapel dtor\n"; }
	std::unique_ptr<BouwKaart> getBuildCard();
	void addBuildCard(std::unique_ptr<BouwKaart> bouwkaart);

private:
	std::deque<std::unique_ptr<BouwKaart>> stapel_;
};
