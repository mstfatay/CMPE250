#include "desk.h"

// ======================= Desk =======================

// classical constractor
Desk::Desk(int id, float runTime) {
	this->id = id;
	this->runTime = runTime;
}

// returns whether this desk is available for a new hacker.
bool Desk::isAvailable() {
	return this->available;
}

// puts a hacker to this desk. Makes this desk unavailable.
void Desk::putHacker(Hacker* h) {
	this->currHacker = h;
	this->available = false;
	h->addDeskWaitTime(this->runTime);
}

// removes the hacker that curently using this desk.
void Desk::removeHacker() {
	this->currHacker = NULL;
	this->available = true;
}

// returns work time of this desk.
float Desk::getRunTime() {
	return this->runTime;
}

// returns the hacker that currently using this desk.
Hacker* Desk::getCurrHacker() {
	return this->currHacker;
}


// ======================= DeskManager =======================

// classical constructor
DeskManager::DeskManager(vector<float> runTimes) {
	this->size = runTimes.size();
	for (int i = 0; i < this->size; i++) {
		desks.push_back(new Desk(i, runTimes[i]));
	}
}

// returns true if at least a desk is available, false otherwise
bool DeskManager::hasAvailableDesk() {
	for (auto a : desks) {
		if (a->isAvailable()) {
			return true;
		}
	}
	return false;
}


/*
puts the given hacker to first available desk and returns that desk.
If there is no available desk returns NULL instead.
*/
Desk* DeskManager::giveAvailableDesk(Hacker* h) {
	for (auto desk : desks) {
		if (desk->isAvailable()) {
			desk->putHacker(h);
			return desk;
		}
	}
	return NULL;
}



