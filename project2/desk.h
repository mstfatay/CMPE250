#include <vector>
#include "hacker.h"
using namespace std;

#ifndef DESK
#define DESK

/*
Desk class is created to represent hoodie and sticker desks. Every desk has a specific
work time and id. A hacker may come to and leave a desk.
*/
class Desk {
	int id;
	float runTime; // work time of this desk
	bool available = true; //whether this desk is available for a new hacker
	Hacker* currHacker = NULL;	//The hacker currently using this desk
public:
	Desk(int id, float runTime);
	void putHacker(Hacker* h);
	void removeHacker();
	bool isAvailable();
	float getRunTime();
	Hacker* getCurrHacker();
};

/*
This class contains a group of desks and manages their operations.
*/
class DeskManager {
	vector<Desk*> desks;
	int size; //num of desks
public:
	DeskManager(vector<float> runTimeList);
	bool hasAvailableDesk();
	Desk* giveAvailableDesk(Hacker* h);
};

#endif
