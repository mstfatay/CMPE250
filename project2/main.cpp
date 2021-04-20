#include "hacker.h"
#include "event.h"
#include <iostream>
#include <queue>
#include <fstream>
#include <limits>
#include <string>

using namespace std;


class compareEvents {
public:
	bool operator()(const Event* e1, const Event* e2) {
		if (e1->getTime() > e2->getTime() + 0.00001) {
			return true;
		}
		else if (e1->getTime() + 0.00001  < e2->getTime()) {
			return false;
		}
		else {
			return e1->getImportance() > e2->getImportance();
		}
	}
};


// returns a string of given float number with 3 decimals after dot.
string round3(float value) {
	char text[40];
	snprintf(text, 40, "%.3f", value);
	return string(text);
}


int main(int nArg, char* args[])
{
	ifstream infile;
	ofstream outfile;

	infile.open(args[1]);
	outfile.open(args[2]);

	int numOfHackers;
	infile >> numOfHackers; // number of hackers

	vector<Hacker*> hackerList;  // all hackers are here. Indexes are hackerIDs.
	hackerList.push_back(NULL);  // there is no hacker with id 0 so add a NULL pointer to 0th index.
	priority_queue<Event*, vector<Event*>, compareEvents> events; // heap of events

	// get arrive times of hackers and create hackers
	for (int i = 1; i < numOfHackers + 1; i++) {
		float arriveTime;
		infile >> arriveTime;
		Hacker* h = new Hacker(i);
		hackerList.push_back(h);
		Event* hae = new HackerArriveEvent(arriveTime, h, i);
		events.push(hae);
	}

	int numOfCommits;
	infile >> numOfCommits;

	// get all commit events information and create code commit events
	for (int i = 0; i < numOfCommits; i++) {
		int hackerId, commitLength;
		float time;
		infile >> hackerId >> commitLength >> time;
		Hacker* h = hackerList[hackerId];
		Event* cce = new CodeCommitEvent(time, h, hackerId, commitLength);
		events.push(cce);
	}


	int numOfEnterence;
	infile >> numOfEnterence;

	// get all sticker queue enterence information and create sticker queue enterance event
	for (int i = 0; i < numOfEnterence; i++) {
		int hackerId;
		float time;
		infile >> hackerId >> time;
		Hacker* h = hackerList[hackerId];
		Event* sqee = new StickerQEnteranceEvent(time, h, hackerId);
		events.push(sqee);
	}

	int nStickerDesk;
	int nHoodieDesk;

	infile >> nStickerDesk; // num of sticker desk
	vector<float> sdRunTimes;  // sticker desks' work times (= run times)
	
	// get sticker desks' work times
	for (int i = 0; i < nStickerDesk; i++) {
		float sdRunTime;
		infile >> sdRunTime;
		sdRunTimes.push_back(sdRunTime);
	}
	StickerQEnteranceEvent::initializeDeskManager(sdRunTimes); // initialize desk manager of sticker desks

	infile >> nHoodieDesk; // num of hoodie desk
	vector<float> hdRunTimes; // hoodie desks' work times (= run times)

	// get hoodie desks' work times
	for (int i = 0; i < nHoodieDesk; i++) {
		float hdRunTime;
		infile >> hdRunTime;
		hdRunTimes.push_back(hdRunTime);
	}
	StickerDeskNextEvent::initializeDeskManager2(hdRunTimes); // initialize desk manager of hoodie desks


	float lastEventTime = 0; //time of last executed event

	// execude all events
	while (!events.empty()) {
		Event* e = events.top();
		events.pop();
		//e->print();
		lastEventTime = e->getTime(); //update time of last executed event
		vector<Event*>* newEvents = e->execute(); //execute the event
		if (newEvents != NULL) {
			for (auto a : *newEvents) {
				// add new events to events queue
				events.push(a);
			}
		}
	}

	// metrics needed for output
	int totalGifts = 0;
	int totalCommits = 0;
	int totalCommitLines = 0;
	int totalInvalid = 0; // invalid queue enterance
	int total4thInvalid = 0; //invalid queue enterance because hacker already has 3 gifts.
	float totalHoodieWaitTime = 0;
	float totalStickerWaitTime = 0;
	float totalDeskWaitTime = 0;

	float maxQueueWaitTime = -1;
	int maxQueueWaitId = 0;
	float minQueueWaitTime = numeric_limits<float>::max();
	int minQueueWaitId = 0;

	// iterate in hackerList and calculate all metrics about hacker
	for (auto a : hackerList) {
		if (a == NULL) {
			// because first element of hackerList is Null in my implamentation
			continue;
		}
		totalGifts += a->getReceivedGifts();
		totalCommits += a->getNumOfCommits();
		totalCommitLines += a->getTotalCommitLines();
		totalInvalid += a->getInvalidQueueAttempt();
		total4thInvalid += a->getInvalid4thAttempt();
		totalHoodieWaitTime += a->getTotalHoodieWaitTime();
		totalStickerWaitTime += a->getTotalStickerWaitTime();
		totalDeskWaitTime += a->getDeskWaitTime();

		float queueWaitTime = a->getTotalHoodieWaitTime() + a->getTotalStickerWaitTime();
		if (queueWaitTime > maxQueueWaitTime+0.00001) {
			maxQueueWaitTime = queueWaitTime;
			maxQueueWaitId = a->getId();
		}
		if (a->getReceivedGifts() == 3 && queueWaitTime + 0.00001 < minQueueWaitTime) {
			minQueueWaitTime = queueWaitTime;
			minQueueWaitId = a->getId();
		}

	}

	// averaging some metrics
	float averageGifts = (float)totalGifts / (float)numOfHackers;
	float averageCommits = float(totalCommits) / (float)numOfHackers;
	float averageCommitLines = float(totalCommitLines) / (float)totalCommits;
	float averageHoodieWaitTime = totalHoodieWaitTime / (float)totalGifts;
	float averageStickerWaitTime = totalStickerWaitTime / (float)totalGifts;
	float averageTurnaroundTime = totalDeskWaitTime/float(totalGifts) + averageStickerWaitTime + averageHoodieWaitTime;
	
	// get maximum length of queues from static variables
	int maxLenStickerQueue = StickerQEnteranceEvent::getMaxLenStickerQueue();
	int maxLenHoodieQueue = StickerDeskNextEvent::getMaxLenHoodieQueue();

	if (minQueueWaitId == 0) {
		minQueueWaitId = -1;
		minQueueWaitTime = -1;
	}
	
	// write output to output file
	outfile << maxLenStickerQueue << endl;
	outfile << maxLenHoodieQueue << endl;
	outfile << round3(averageGifts) << endl;
	outfile << round3(averageStickerWaitTime) << endl;
	outfile << round3(averageHoodieWaitTime) << endl;
	outfile << round3(averageCommits) << endl;
	outfile << round3(averageCommitLines) << endl;
	outfile << round3(averageTurnaroundTime) << endl;
	outfile << totalInvalid << endl;
	outfile << total4thInvalid << endl;
	outfile << maxQueueWaitId << " " << round3(maxQueueWaitTime) << endl;
	outfile << minQueueWaitId << " " << round3(minQueueWaitTime) << endl;
	outfile << round3(lastEventTime) << endl;


	infile.close();
	outfile.close();
}