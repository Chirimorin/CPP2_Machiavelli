#pragma once
#include "BouwKaart.h"

class KaartStapel
{
public:
	KaartStapel();
	~KaartStapel() { std::cout << "KaartStapel dtor\n"; }
private:
	std::deque<std::unique_ptr<BouwKaart>> stapel_;
};
