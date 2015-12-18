#pragma once
class KarakterKaart
{
public:
	KarakterKaart() {};
	KarakterKaart(int nummer, std::string naam) : nummer_{ nummer }, naam_{ naam } {};
	~KarakterKaart() {};

	bool isValid() const { return isValid_; };

	friend std::istream& operator>>(std::istream& is, KarakterKaart& kaart);
private:
	int nummer_  = -1;
	std::string naam_ = "";

	bool isValid_ = false;
};

std::istream& operator>>(std::istream& is, KarakterKaart& kaart);
