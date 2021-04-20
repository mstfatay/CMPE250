#include <vector>
#include <deque>
#include <iostream>
using namespace std;

#ifndef HACKER
#define HACKER

/*
This class represents a hacker and does all operations a hacker does.
*/
class Hacker {
	// ids are from 1 to numOfHackers
	int id;

	// legths of all code commits this hacker made
	vector<int> commits; 

	// number of commits this hacker made
	int numOfCommits = 0;
	// number of commits with length>=20 this hacker made
	int numOfValidCommits = 0;

	//whether this hacker is arrived
	bool arrived = false;

	//number of received stickers and hoodies
	int receivedGifts = 0;

	//number of received stickers
	int receivedStickers = 0;

	// whether in the sticker queue
	bool inStickerQueue = false;
	// whether in the hoodie queue
	bool inHoodieQueue = false;

	//number of invalid sticker queue enterance attempts
	int invalidQueueAttempt = 0;
	//nuber of sticker queue enterence attemps with 3 received gifts.
	int invalid4thAttempt = 0;

	// Total time spent in sticker queue by this hacker
	float totalStickerWaitTime = 0;
	// Total time spent in hoodie queue by this hacker
	float totalHoodieWaitTime = 0;
	// Total time spent in front of desks by this hacker
	float deskWaitTime = 0;

	//The time when this hacker is entered to hoodie queue
	float hoodieQArriveTime = 0;
	//The time when this hacker is entered to sticker queue
	float stickerQArriveTime = 0;

	


public:
	Hacker(int id);
	void getArrived();
	void enterStickerQueue(float time);
	bool canEnterStickerQueue();
	void commitCode(int commitLength);
	void getSticker(float time);
	void getHoodie();
	void increaseInvalidAttempt();
	void increase4thAttempt();
	void addHoodieWaitTime(float time);
	void addStickerWaitTime(float time);
	void addDeskWaitTime(float runTime);

	//getter methods
	bool isArrived();
	int getTotalCommitLines();
	int getId() const;
	int getNumOfValidCommits() const;
	int getNumOfCommits();
	int getReceivedStickers();
	int getReceivedGifts();
	int getInvalidQueueAttempt();
	int getInvalid4thAttempt();
	float getTotalStickerWaitTime();
	float getTotalHoodieWaitTime();
	float getDeskWaitTime();
	float getHoodieQArriveTime() const;

	void print();
};


#endif