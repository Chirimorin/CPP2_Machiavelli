#pragma once
#include "BouwKaart.h"

class KaartStapel
{
public:
	KaartStapel();
	~KaartStapel() {};
private:
	std::vector<std::unique_ptr<BouwKaart>> stapel_;
};

