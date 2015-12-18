#pragma once
#include "BouwKaart.h"

class KaartStapel
{
public:
	KaartStapel();
	~KaartStapel() {};
private:
	std::deque<std::unique_ptr<BouwKaart>> stapel_;
};
