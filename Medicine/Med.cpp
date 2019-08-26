#include "pch.h"
#include "Med.h"
#include <string>
#include <iostream>
Med::Med()
{
}

Med::Med(std::string identity) 
{
	this->identity = identity;
	analyzeMedicine();
	
}
Med::~Med()
{
}
void Med::setRemain(int remain) {
	this->remain = remain;
}
void Med::setDailyUsed(int dailyUsed) {
	this->dailyUsed = dailyUsed;
}
int Med::getDailyUsed() {
	return dailyUsed;
}
int Med::getRemain() {
	return remain;
}
std::string Med::getName()
{
	return name;
}
void Med::analyzeMedicine() {
	std::string med = this->identity;
	int splitIndex[2];
	int splitNo = 0;
	std::cout << std::endl <<"medlength"<<med.length();
	for (int i = 0; i < med.length(); i++) {
		if (identity[i] == '#')
		{
			splitIndex[splitNo++] = i;
		}
	}
	if (splitIndex[0] < 0) return;
	name = identity.substr(0, splitIndex[0]);

	remain = std::atoi(identity.substr(splitIndex[0]+1,splitIndex[1]).c_str());
	std::cout << std::endl << name+":"<<remain<<":" ;
	dailyUsed = std::atoi(identity.substr(splitIndex[1] + 1, identity.length()).c_str());



}
