#include "hacker.h"

//classical constructer
Hacker::Hacker(int id) {
	this->id = id;
	this->commits = vector<int>();
}

// sets arrived true
void Hacker::getArrived() {
	this->arrived = true;
}


/*
This hacker commits code with the given length. Also number of commits is increased by one.
If commit length >= 20 then number of valid commits is also increased by one.
*/
void Hacker::commitCode(int commitLength) {
	this->commits.push_back(commitLength);
	this->numOfCommits ++;
	if (commitLength >= 20) {
		this->numOfValidCommits++;
	}
}

void Hacker::print() {
	cout << "Hacker id: " << this->id << endl;
	cout << "commits: ";
	for (auto a : this->commits) {
		cout << a << " ";
	}
	cout << endl;
	cout << "arrived: " << this->arrived << endl;
	cout << "received stickers: " << this->receivedStickers << endl;
	cout << "received gifts: " << this->receivedGifts << endl;
	cout << "totalStickerWaitTime: " << this->totalStickerWaitTime << endl;
	cout << "totalHoodieWaitTime: " << this->totalHoodieWaitTime << endl;
	cout << "invalidQueueAttempt: " << this->invalidQueueAttempt << endl;
	cout << "invalid4thAttempt: " << this->invalid4thAttempt << endl;
	cout << "num of valid commits: " << this->numOfValidCommits << endl;
	cout << endl;
}

/*
returns whether this hacker can enter the sticker queue.
This is tought as a queue enterance attempt. If this attempt is invalid, appropriate invalid
attempt of this hacker is increased.
*/
bool Hacker::canEnterStickerQueue() {
	if (this->receivedGifts >= 3) {
		increase4thAttempt();
		return false;
	}
	bool valid = !this->inStickerQueue;

	if (this->inHoodieQueue) {
		valid = false;
	}

	int validCommits = 0;
	for (auto a : this->commits) {
		if (a >= 20) {
			validCommits++;
		}
	}
	if (validCommits < 3) {
		valid = false;
	}
	
	if (!valid) {
		increaseInvalidAttempt();
		return false;
	}

	return true;
}


//this hacker's statue is set to be in a sticker queue and his sticker queue arrive time is updated.
void Hacker::enterStickerQueue(float time) {
	this->inStickerQueue = true;
	this->stickerQArriveTime = time;
}


//this hackers gets a sticker and enters the hoodie queue.
void Hacker::getSticker(float time) {
	this->receivedStickers++;
	this->inStickerQueue = false;
	this->inHoodieQueue = true;
	this->hoodieQArriveTime = time;
}

// this hacker gets a hoodie and leaves the hoodie queue.
void Hacker::getHoodie() {
	this->receivedGifts++;
	this->inHoodieQueue = false;
}

void  Hacker::increaseInvalidAttempt() {
	this->invalidQueueAttempt++;
}

void Hacker::increase4thAttempt() {
	this->invalid4thAttempt++;
}

/*
Calculates time passed this hacker from entering to hoodie queue to exiting.
Then adds this time value to total hoodie queue wait time.
The exiting time should be given as an input. 
*/
void Hacker::addHoodieWaitTime(float time) {
	float waitTime = time - this->hoodieQArriveTime;
	this->totalHoodieWaitTime += waitTime;
}

/*
Calculates time passed this hacker from entering to sticker queue to exiting.
Then adds this time value to total sticker queue wait time.
The exiting time should be given as an input.
*/
void Hacker::addStickerWaitTime(float time) {
	float waitTime = time - this->stickerQArriveTime;
	this->totalStickerWaitTime += waitTime;
}

/*
calculates the time spent by this hacker in front of a desk.
Adds this value to total time spent in front of a desk.
This desk may be a sticker or hoodie desk.
*/
void Hacker::addDeskWaitTime(float runTime) {
	this->deskWaitTime += runTime;
}


//  Getter Methods

bool Hacker::isArrived() {
	return this->arrived;
}

int Hacker::getTotalCommitLines() {
	int result = 0;
	for (auto a : commits) {
		result += a;
	}
	return result;
}

int Hacker::getId() const {
	return this->id;
}

int Hacker::getNumOfValidCommits() const {
	return this->numOfValidCommits;
}

int Hacker::getNumOfCommits() {
	return this->numOfCommits;
}

int Hacker::getReceivedStickers() {
	return this->receivedStickers;
}

int Hacker::getReceivedGifts() {
	return this->receivedGifts;
}

int Hacker::getInvalidQueueAttempt() {
	return this->invalidQueueAttempt;
}

int Hacker::getInvalid4thAttempt() {
	return this->invalid4thAttempt;
}

float Hacker::getTotalStickerWaitTime() {
	return this->totalStickerWaitTime;
}

float Hacker::getTotalHoodieWaitTime() {
	return this->totalHoodieWaitTime;
}

float Hacker::getDeskWaitTime() {
	return this->deskWaitTime;
}

float Hacker::getHoodieQArriveTime() const {
	return this->hoodieQArriveTime;
}