
#include "pch.h"
#include <iostream>
#include <Windows.h>
#include <string>
#include <fstream>
#include <time.h>
#include "Med.h"
#include <vector> 

#if defined(_WIN16) | defined(_WIN32) | defined(_WIN64)
const std::string SEP = "\\";
#else
const std::string SEP = "/";
#endif

#pragma warning(disable:4996)

int  getNumberOfDays(int month, int year);
int  calculateDayPast(std::string currentDate,std::string lastExecDate);
int main()
{

	// get the current date
	time_t now = time(0);
	tm *ltm = localtime(&now);
	std::string tempDate = std::to_string(ltm->tm_mday) + "/" + std::to_string(1+ltm->tm_mon);
	std::string year = std::to_string(1900+ltm->tm_year);
	std::string currentDate = tempDate + "/" + year;
	std::cout << "Today is:"+currentDate <<std::endl;
	
	char buff[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, buff);
	std::string pwd = buff;

	std::string lastExecDate;
	//exit program if it was executed today
	std::ifstream lastExecFile;
	lastExecFile.open(pwd+SEP+"lastExecution.txt");
	if (!lastExecFile) {
		std::ofstream outfile(pwd + SEP + "lastExecution.txt");
		outfile << currentDate;
		exit(0);
	}
	else getline(lastExecFile,lastExecDate);
	
	std::cout << "lastExecDate:"+lastExecDate;

	std::ifstream medicinesFile;
	medicinesFile.open(pwd+SEP+"medicines.txt");

	//control the file which includes medicines information
	if (!medicinesFile) {
		std::cerr << "Unable to open file medicines.txt";
		exit(1); 
	}

	//create medicine objects	
	std::string b; 
	std::vector<Med> medicines;

	while (getline(medicinesFile, b))
	{
		medicines.push_back(Med(b));
	}


	std::vector<Med> medRequest;
	
	// modify remain medicine
	int dayPast = calculateDayPast(currentDate,lastExecDate);
	for (Med med : medicines) 
	{
		med.setRemain(med.getRemain()-med.getDailyUsed()*dayPast);
		/* number 3 is for worst case scenario (weekend+national holiday)
			there is no national holiday more than 1 day in Turkey
			since religious holidays are not certain and changes year to year based on government decision
			these holidays did not take into account.
		*/
		if (med.getRemain()/med.getDailyUsed() <= 3) medRequest.push_back(med);
	}
	// create mail with required medicines and send to doctor
	if (medRequest.size() != 0) 
	{
		std::cout <<std::endl<< "size:"<<medRequest.size();
		std::ofstream mailFile(pwd + SEP + "mail.txt");
		mailFile << "From: Yunus Emre Ölmez <schedulermetu@gmail.com>" << std::endl;
		mailFile << "To: Doktor <y.eolmez21@gmail.com>" << std::endl;
		mailFile << "Subject: Yazýlmasý gereken ilaçlar" << std::endl;
		mailFile << "Hocam aþaðýda belirttiðim düzenli kullanýlan ilaçlarýn reçetesinin yazýlmasýný rica ediyorum." << std::endl;
		for (Med med : medRequest) {
			std::cout << std::endl<<med.getName();
			mailFile << med.getName() << std::endl;
		} 
	
		mailFile.close();
		const char* command = "curl --url smtp://smtp.gmail.com:587 --ssl-reqd --mail-from schedulermetu@gmail.com --mail-rcpt y.eolmez21@gmail.com --upload-file mail.txt --user schedulermetu@gmail.com:hilorik0634 --insecure";
		WinExec(command, SW_SHOW);
	}
	//modify medicine.txt
	std::ofstream medFile(pwd + SEP + "medicines.txt");
	for (int i=0;i<medicines.size()-1;i++) 
	{
		medFile << medicines.at(i).getName() + "#" + std::to_string(medicines.at(i).getRemain()) + "#" + std::to_string(medicines.at(i).getDailyUsed())<<std::endl;
	}
	medFile << medicines.at(medicines.size()-1).getName() + "#" + std::to_string(medicines.at(medicines.size() - 1).getRemain()) + "#" + std::to_string(medicines.at(medicines.size() - 1).getDailyUsed());
	// modify program last execution date in the file
	std::ofstream outfile(pwd+SEP+"lastExecution.txt");
	outfile << currentDate << std::endl;
	outfile.close();
	//exit program
	exit(2);
}
int  calculateDayPast(std::string currentDate, std::string lastExecDate)
{
	int currDate[3];
	int lastDate[3];
	int start = 0;
	int timeType = 0;
	int dayPast = 0;
	for (int i = 0; i < currentDate.length(); i++)
	{
		if (currentDate[i] == '/') {
			currDate[timeType++] = std::atoi(currentDate.substr(start, i).c_str());
			start = i + 1;
		}
		else if (timeType == 2) {
			currDate[2] = std::atoi(currentDate.substr(start, currentDate.size()).c_str());
			break;
		}

	}
	start = 0;
	timeType = 0;
	for (int i = 0; i < lastExecDate.length(); i++)
	{
		if (lastExecDate[i] == '/') {
			lastDate[timeType++] = std::atoi(lastExecDate.substr(start, i).c_str());
			start = i + 1;
		}
		else if (timeType == 2) {
			lastDate[2] = std::atoi(lastExecDate.substr(start, lastExecDate.size()).c_str());
			break;
		}


	}
	// dates are in the same year
	if (lastDate[2] == currDate[2]) 
	{
		for (int i = lastDate[1]; i < currDate[1]; i++) 
		{
			dayPast += getNumberOfDays(i,lastDate[2]);
		}
		dayPast = dayPast-lastDate[0]+currDate[0];
	
	}
	//dates are not in the same year
	else if (lastDate[2]<currDate[2]) 
	{
		
		for (int i = lastDate[1]; i <=12; i++) {
			dayPast += getNumberOfDays(i,lastDate[2]);
		}
		dayPast -= lastDate[0];
		for (int i = 1; i < currDate[1]; i++) 

		{
			dayPast += getNumberOfDays(i, currDate[2]);
		}
		dayPast += currDate[0];
	}
	// logical error
	else {
		std::cerr << "last executed date can not be after current date";
		exit(4);
	}
	std::cout << std::endl << "daypast:" << dayPast;
	return dayPast;

}
int  getNumberOfDays(int month, int year)
{
	//leap year condition, if month is 2
	if (month == 2)
	{
		if ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0))
			return 29;
		else
			return 28;
	}
	//months which has 31 days
	else if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8
		|| month == 10 || month == 12)
		return 31;
	else
		return 30;
}

