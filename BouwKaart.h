#pragma once
class BouwKaart
{
public:
	BouwKaart() {};
	BouwKaart(std::string naam, int prijs, std::string kleur) : naam_{naam}, prijs_{prijs}, kleur_{kleur} {};
	~BouwKaart() {};

	bool IsValid() const { return isValid_; };
	std::string getName() { return naam_; };

	std::string getInfo() { return getColorString() + naam_ + "\33[40m (" + std::to_string(prijs_) + ')'; }

	friend std::istream& operator>>(std::istream& is, BouwKaart& kaart);
private:
	std::string naam_ = "";
	int prijs_ = -1;
	std::string kleur_ = "";

	bool isValid_ = false;

	std::string getColorString();
};

std::istream& operator>>(std::istream& is, BouwKaart& kaart);