#include "event.h"


class compareHackers {
public:
	bool operator()(const Hacker* h1, const Hacker* h2) {
		if (h1->getNumOfValidCommits() < h2->getNumOfValidCommits()) {
			return true;
		}
		else if (h1->getNumOfValidCommits() > h2->getNumOfValidCommits()) {
			return false;
		}
		else {
			if (h1->getHoodieQArriveTime() > h2->getHoodieQArriveTime() + 0.00001) {
				return true;
			}
			else if (h1->getHoodieQArriveTime() + 0.00001 < h2->getHoodieQArriveTime()) {
				return false;
			}
			else {
				return h1->getId() > h2->getId();
			}
		}
	}
};


// ======================= Event =======================

// classical consturctor
Event::Event(float time) {
	this->time = time;
}

void Event::print() {
	cout << this->time << endl;
}

float Event::getTime() const {
	return this->time;
}



// ======================= Hacker Arrive Event =======================

HackerArriveEvent::HackerArriveEvent(float time, Hacker* hacker, int hackerId) :Event(time) {
	this->hackerId = hackerId;
	this->hacker = hacker;
}

void HackerArriveEvent::print() {
	cout << this->time << ": Hacker " << this->hackerId << " is arrived." << endl;
	//this->hacker->print();
}

// Makes the hacker arrived.
vector<Event*>* HackerArriveEvent::execute() {
	this->hacker->getArrived();
	return NULL;
}



// ======================= Code Commit Event =======================

CodeCommitEvent::CodeCommitEvent(float time, Hacker* hacker, int hackerId, int commitLength) :Event(time) {
	this->hacker = hacker;
	this->hackerId = hackerId;
	this->commitLength = commitLength;
}

void CodeCommitEvent::print() {
	cout << this->time << ": Hacker " << this->hackerId << " is trying to commit code whith length " <<
		this->commitLength << endl;
}

/*
If hacker is already arrived, commits code with specified length.
*/
vector<Event*>* CodeCommitEvent::execute() {
	if (hacker->isArrived()) {
		this->hacker->commitCode(this->commitLength);
	}
	return NULL;
}


// ======================= Sticker Queue Enterance Event =======================
deque<Hacker*>* StickerQEnteranceEvent::stickerQueue = new deque<Hacker*>();
int StickerQEnteranceEvent::maxLenStickerQueue = 0;
DeskManager* StickerQEnteranceEvent::deskManager = NULL;

StickerQEnteranceEvent::StickerQEnteranceEvent(float time, Hacker* hacker, int hackerId) :Event(time) {
	this->hacker = hacker;
	this->hackerId = hackerId;
}

//Initializes the sticker queue's desk manager with the given work times.
void StickerQEnteranceEvent::initializeDeskManager(vector<float> runTimes) {
	StickerQEnteranceEvent::deskManager = new DeskManager(runTimes);
}

// getter method for maxLenStickerQueue.
int StickerQEnteranceEvent::getMaxLenStickerQueue() {
	return StickerQEnteranceEvent::maxLenStickerQueue;
}

/*
checks the lenght of the sticker queue. If it greater than the maximum
lenght of the queue, updates that value.
*/
void StickerQEnteranceEvent::updateMaxLenStickerQueue() {
	if (this->stickerQueue->size() > this->maxLenStickerQueue) {
		this->maxLenStickerQueue = this->stickerQueue->size();
	}
}

void StickerQEnteranceEvent::print() {
	cout << this->time << ": Hacker " << this->hackerId << " is trying to enter the sticker queue." << endl;
}

/*
If any sticker desk is available the hacker goes to that desk without entering the queue and
a StickerDeskNextEvent is created and returned.
Otherwise the hacker enters the queue, Null is returned.
*/
vector<Event*>* StickerQEnteranceEvent::execute() {
	if (!this->hacker->isArrived()) {
		this->hacker->increaseInvalidAttempt();
		return NULL;
	}
	if (this->hacker->canEnterStickerQueue()) {
		this->hacker->enterStickerQueue(this->time); // updates enterence time of hacker to queue
		
		if (this->deskManager->hasAvailableDesk()) {
			// If there is any available desk, the hacker goes to that desk.
			Desk* desk = this->deskManager->giveAvailableDesk(this->hacker);
			float runTime = desk->getRunTime();

			// A StickerDeskNextEvent is created because this hacker started waiting in front of a desk.
			StickerDeskNextEvent* e = new StickerDeskNextEvent(this->time + runTime, this->stickerQueue, desk);
			
			//this->hacker->addStickerWaitTime(this->time);
			vector<Event*>* v = new vector<Event*>;
			v->push_back(e);
			return v;
		}
		else {
			// If all desks are busy then put hacker to sticker queue.
			this->stickerQueue->push_back(this->hacker);
			this->updateMaxLenStickerQueue();
		}
	}
	return NULL;
}


// ======================= Sticker Desk Next Event =======================
priority_queue<Hacker*, vector<Hacker*>, compareHackers>* StickerDeskNextEvent::hoodieQueue = new priority_queue<Hacker*, vector<Hacker*>, compareHackers>();
int StickerDeskNextEvent::maxLenHoodieQueue = 0;
DeskManager* StickerDeskNextEvent::deskManager2 = NULL;

StickerDeskNextEvent::StickerDeskNextEvent(float time, deque<Hacker*>* stickerQueue, Desk* currDesk) :Event(time) {
	this->stickerQueue = stickerQueue;
	this->currDesk = currDesk;
}

/*
checks the lenght of the hoodie queue. If it greater than the maximum
lenght of the queue, updates that value.
*/
void StickerDeskNextEvent::updateMaxLenHoodieQueue() {
	if (this->hoodieQueue->size() > this->maxLenHoodieQueue) {
		this->maxLenHoodieQueue = this->hoodieQueue->size();
	}
}

//Initializes the hoodie queue's desk manager with the given work times.
void StickerDeskNextEvent::initializeDeskManager2(vector<float> runTimes) {
	StickerDeskNextEvent::deskManager2 = new DeskManager(runTimes);
}

// getter method for maxLenHoodieQueue.
int StickerDeskNextEvent::getMaxLenHoodieQueue() {
	return StickerDeskNextEvent::maxLenHoodieQueue;
}

void StickerDeskNextEvent::print() {
	cout << this->time << ": A new sticker is given to hacker " << this->currDesk->getCurrHacker()->getId() << endl;
}

/*
The hacker gets a stciker and leaves the sticker desk. If any hoodie desk is available, the hacker is go to that desk 
without entering hoodie queu and a HoodieDeskNextEvent is added to vector that returned. Otherwise the hacker enters 
the hoodie queue. If sticker queue is not empty a new hacker (the next hacker) comes to the sticker desk which is 
just got left. In this case, a new StickerDeskNextEvent is created and added to the vector that returned.
*/
vector<Event*>* StickerDeskNextEvent::execute() {
	Hacker* currHacker = this->currDesk->getCurrHacker();
	currHacker->getSticker(this->time); //the current hacker receives a sticker
	this->currDesk->removeHacker(); // the current hacker is removed from the desk
	vector<Event*>* v = new vector<Event*>;
	if (hoodieQueue->empty() && this->deskManager2->hasAvailableDesk()) {
		// If there is any available hoodie desk, the hacker goes to that desk.
		Desk* desk = this->deskManager2->giveAvailableDesk(currHacker);
		float runTime = desk->getRunTime();

		//a new HoodieDeskNextEvent is created. Because hacker is now in front of that desk.
		v->push_back(new HoodieDeskNextEvent(this->time + runTime, this->hoodieQueue, desk, this->deskManager2));
	}
	else {
		// If there is no available hoodie desk, the hacker enters to the hoodie queue.
		hoodieQueue->push(currHacker);
		this->updateMaxLenHoodieQueue();
	}
	
	if (!this->stickerQueue->empty()) {
		// If sticker queue is not empty a new hacker comes to the desk.
		Hacker* h = this->stickerQueue->front();
		this->stickerQueue->pop_front();
		this->currDesk->putHacker(h);
		float newTime = this->time + this->currDesk->getRunTime();

		//A new StickerDeskNextEvent is created. Because a new hacker is waitin in front of this desk.
		v->push_back(new StickerDeskNextEvent(newTime, this->stickerQueue, this->currDesk));
		h->addStickerWaitTime(this->time);
	}
	return v;
}



// ======================= Hoodie Desk Next Event =======================

HoodieDeskNextEvent::HoodieDeskNextEvent(float time, priority_queue<Hacker*, vector<Hacker*>, compareHackers>* hoodieQueue, Desk* currDesk, DeskManager* deskManager2) :Event(time) {
	this->currDesk = currDesk;
	this->hoodieQueue = hoodieQueue;
	this->deskManager2 = deskManager2;
}

void HoodieDeskNextEvent::print() {
	cout << this->time << ": A new hoodie is given to hacker " << this->currDesk->getCurrHacker()->getId() << endl;
}

/*
The hacker gets a hoodie and leaves the hoodie desk. If hoodie queue is not empty a new hacker (the next hacker) comes
to the hoodie desk which is just got left. In this case, a new HoodieDeskNextEvent is created and addedto the vector
that returned.
*/
vector<Event*>* HoodieDeskNextEvent::execute() {
	vector<Event*>* v = new vector<Event*>;
	Hacker *currHacker = currDesk->getCurrHacker();
	currHacker->getHoodie(); // the hacker gets a hoodie
	this->currDesk->removeHacker(); // hacker is left the hoodie desk
	if (!this->hoodieQueue->empty()) {
		// If hoodie queue is not empty a new (next) hacker is come to the desk that is just left.
		Hacker* h = this->hoodieQueue->top();
		this->hoodieQueue->pop();
		Desk* desk = this->deskManager2->giveAvailableDesk(h);
		float runTime = desk->getRunTime();

		// a new HoodieDeskNextEvent is created. Because a new hacker is in front of the desk now.
		v->push_back(new HoodieDeskNextEvent(this->time + runTime, this->hoodieQueue, desk, this->deskManager2));
		h->addHoodieWaitTime(this->time);
	}
	return v;
}
