#pragma once
class KarakterKaart
{
public:
	KarakterKaart() {};
	KarakterKaart(int nummer, std::string naam) : nummer_{ nummer }, naam_{ naam } {};
	~KarakterKaart() {};

	bool isValid() const { return isValid_; }
	std::string getName() { return naam_; }
	std::string getInfo() { return getColorString() + naam_ + "\33[40m"; }

	friend std::istream& operator>>(std::istream& is, KarakterKaart& kaart);
private:
	int nummer_  = -1;
	std::string naam_ = "";

	bool isValid_ = false;

	std::string getColorString();
};

std::istream& operator>>(std::istream& is, KarakterKaart& kaart);
