#include <iostream>
#include <vector>
#include <deque>
#include "hacker.h"
#include <queue>
#include "desk.h"
using namespace std;

#ifndef EVENT
#define EVENT

//A class to compare hackers (needed for priority_queue)
class compareHackers;


/*
Represents an event. This the parent of HackerArriveEvent, CodeCommitEvent,
StickerQEnteranceEvent, StickerDeskNextEvent, HoodieDeskNextEvent classes
*/
class Event {
protected:
	float time; //execution time of this event
public:
	Event(float time);
	float getTime() const;
	virtual void print();

	//the event having lower importance is first executed.
	virtual int getImportance() const = 0; 

	/*
	Executes this event. Every event is created to a different purpose so
	every events' execute method differs. If this event creates new events.
	Returns them as a vector of events.
	*/
	virtual vector<Event*>* execute() = 0;
};

/*
Handles a hacker's arrival.
*/
class HackerArriveEvent : public Event {
	// id of hacker that arrives
	int hackerId;

	// hacker that arrives
	Hacker* hacker;
public:
	HackerArriveEvent(float time, Hacker* hacker, int hackerId);
	void print();
	int getImportance() const { return this->hackerId; };
	vector<Event*>* execute();
};

/*
handles a hacker's one code commit.
*/
class CodeCommitEvent : public Event {
	int hackerId;
	Hacker* hacker;

	// length of the code the hacker commmits
	int commitLength;
public:
	CodeCommitEvent(float time, Hacker* hacker, int hackerId, int commitLength);
	void print();
	int getImportance() const { return this->hackerId; };
	vector<Event*>* execute();
};

/*
Puts a hacker to the sticker queue if it is possible.
*/
class StickerQEnteranceEvent : public Event {
	int hackerId;
	Hacker* hacker;

	//This is one and only sticker queue.
	static deque<Hacker*>* stickerQueue;
	
	//This is one and only desk manager of sticker desks
	static DeskManager* deskManager;

	//This is a variable to keep max length of the sticker queue
	static int maxLenStickerQueue;
	
public:
	StickerQEnteranceEvent(float time, Hacker* hacker, int hackerId);
	void updateMaxLenStickerQueue();
	void static initializeDeskManager(vector<float> runTimes);
	int static getMaxLenStickerQueue();
	void print();
	int getImportance() const { return this->hackerId; };
	vector<Event*>* execute();
};

/*
When a hacker is in front of a sticker desk, he is waiting for this event to execute. 
This event makes the hacker leave the sticker desk and enter the hoodie queue.
*/
class StickerDeskNextEvent : public Event {
	// a pointer to sticker queue
	deque<Hacker*>* stickerQueue;

	// The desk which works.
	Desk* currDesk;

	//one and only hoodie queue
	static priority_queue<Hacker*, vector<Hacker*>, compareHackers>* hoodieQueue;

	//This is one and only desk manager of hoodie desks
	static DeskManager* deskManager2;

	//This is a variable to keep max length of the hoodie queue
	static int maxLenHoodieQueue;
public:
	StickerDeskNextEvent(float time, deque<Hacker*>* stickerQueue, Desk* currDesk);
	void updateMaxLenHoodieQueue();
	void static initializeDeskManager2(vector<float> runTimes);
	int static getMaxLenHoodieQueue();
	void print();
	int getImportance() const { return this->currDesk->getCurrHacker()->getId(); };
	vector<Event*>* execute();
};


/*
When a hacker is in front of a hoodie desk, he is waiting for this event to execute.
This event makes the hacker leave the sticker desk and enter the hoodie queue.
*/
class HoodieDeskNextEvent : public Event {
	// The desk which works.
	Desk* currDesk;

	//a pointer to hoodie desk manager
	DeskManager* deskManager2;

	//a pointer to hoodie queue
	priority_queue<Hacker*, vector<Hacker*>, compareHackers>* hoodieQueue;
public:
	HoodieDeskNextEvent(float time, priority_queue<Hacker*, vector<Hacker*>, compareHackers>* hoodieQueue, Desk* currDesk, DeskManager* deskManager2);
	void print();
	int getImportance() const { return this->currDesk->getCurrHacker()->getId(); };
	vector<Event*>* execute();
};



#endif