#pragma once
#include <string>
class Med
{

public:
	Med(std::string identity);
	Med();
	~Med();
	int getRemain();
	void setRemain(int remain);
	std::string getName();
	int getDailyUsed();
	void setDailyUsed(int dailyUsed);
	std::string identity;
	

private:
	std::string name;
	int remain;
	int dailyUsed;
	void analyzeMedicine();



};

