#include "Character.h"
#include <fstream>
#include <vector>
#include <string>

using namespace std;


// searching, manuplating and information functions
int searchByName(vector<Character*>&, string);
int searchByType(vector<Character*>&, string);
int findActiveMember(vector<Character*> community, int index);
void insertToCommunity(Character* c, vector<Character*>& v);
int killedNumber(vector<Character*> community);


// Every round actions
bool attackTo(Character*, Character*, bool);
void activateSpecial(Character* user, vector<Character*>& community, Character* lastKilled);
void nextTurn(Character& c); 
void endTurnOfCommunity(vector<Character*>& community, int round);
bool isEnded(vector<Character*>& community);


// printing functions
void printCharacter(Character* c, int round, ostream& out);
void printCommunity(vector<Character*>& community, int round, ostream& out);



int main(int argc, char* argv[]) {
	ifstream inFile;
	inFile.open(argv[1]);

	ofstream ofFile;
	ofFile.open(argv[2]);
	
	int maxRounds; // max number of rounds before draw

	inFile >> maxRounds; // first input

	vector<Character*> community1; // members are sorted according to their names
	vector<Character*> community2; // members are sorted according to their names

	vector<Character*> communityGiven1; // community1 but elements are in the order of input.
	vector<Character*> communityGiven2; // community2 but elements are in the order of input.

	Character* lastKilled1 = NULL; // last killed member from community1
	Character* lastKilled2 = NULL; // last killed member from community2



	// This part gets input of community members.
	for (int i=0; i<5; i++){
		string name, type;
		int attack, defense, health;
		inFile >> name >> type >> attack >> defense >> health;
		Character* c = new Character(name, type, attack, defense, health, maxRounds);
		insertToCommunity(c, community1); // inserts new Character to sorted community vector in ascending names order.
		communityGiven1.push_back(c); // pushs back new character to a vector in the given input order.
	}
	for (int i=0; i<5; i++){
		string name, type;
		int attack, defense, health;
		inFile >> name >> type >> attack >> defense >> health;
		Character* c = new Character(name, type, attack, defense, health, maxRounds);
		insertToCommunity(c, community2); // inserts new Character to sorted community vector in ascending names order.
		communityGiven2.push_back(c); // pushs back new character to a vector in the given input order.
	}


	
	int winner = 0; // 0 -> no winner yet, 1 -> fist community won, 2 -> second community won
	int round = 1; // the round which is happenning right now.
	for ( ; round<=maxRounds; round++){

		// gets input
		string attackerName, defenderName, specialText;
		inFile >> attackerName >> defenderName >> specialText;

		bool useSpecial = (specialText == "SPECIAL"); // whether input says character should use special.

		if (round%2 == 1){
			// community1 is attacking
			int a = searchByName(community1, attackerName);
			int d = searchByName(community2, defenderName);

			a = findActiveMember(community1, a); // if attacker is dead changes the attacker.
			d = findActiveMember(community2, d); // if defender is dead changes the defender.

			if(useSpecial){
				activateSpecial(community1[a], community1, lastKilled1);
			}
			bool isKilled = attackTo(community1[a], community2[d], useSpecial);
			if (isKilled){
				// if there is a killed member he/she is this round's defender character.
				lastKilled2 = community2[d];
			}

			if(isEnded(community2)){
				// checks whether commuinity1 wins.
				winner = 1;
			}

		}
		else {
			// community2 is attacking
			int a = searchByName(community2, attackerName);
			int d = searchByName(community1, defenderName);

			a = findActiveMember(community2, a); // if attacker is dead changes the attacker.
			d = findActiveMember(community1, d); // if defender is dead changes the defender.

			if(useSpecial){
				activateSpecial(community2[a], community2, lastKilled2);
			}
			bool isKilled = attackTo(community2[a], community1[d], useSpecial);
			if (isKilled){
				// if there is a killed member he/she is this round's defender character.
				lastKilled1 = community1[d];
			}

			if(isEnded(community1)){
				// checks whether community2 wins.
				winner = 2;
			}

		}
		// update healthHistories at the end of the round.
		endTurnOfCommunity(community1, round);
		endTurnOfCommunity(community2, round);

		if (winner != 0){
			// if there is a winner. War should end.
			break;
		}

	}

	if (winner == 1){
		ofFile << "Community-1" << endl;
		ofFile << round << endl;
	}
	else if(winner == 2){
		ofFile << "Community-2" << endl;
		ofFile << round << endl;
	}
	else{
		 ofFile << "Draw" << endl;
		 ofFile << --round << endl; // draw happens if round number exceeds maxRounds number. So decrease by ones
	}

	// total number of dead characters.
	int casualties = killedNumber(community1) + killedNumber(community2);

	
	ofFile << casualties << endl;


	printCommunity(communityGiven1, round+1, ofFile);
	printCommunity(communityGiven2, round+1, ofFile);


    return 0;
}


/*
	Activates specials of elves or wizards. Special of dwarfs is activated in atttacTo function.
	user is special user, community is the special user's community, last killed is the last killed
	member of that community.
*/
void activateSpecial(Character* user, vector<Character*>& community, Character* lastKilled){
	if (user->type == "Elves" && user->nRoundsSinceSpecial >= 10){
		int heal = user->remainingHealth/2;
		user->remainingHealth -= heal;
		int i = searchByType(community, "Hobbit");
		community[i]->remainingHealth += heal;
		user->nRoundsSinceSpecial = 0;
	}
	else if (user->type == "Wizards" && user->nRoundsSinceSpecial >= 50){
		if(lastKilled){
			if (!lastKilled->isAlive){
				lastKilled->nRoundsSinceSpecial = 0;
			}
			lastKilled->isAlive = true;
			lastKilled->remainingHealth = lastKilled->healthHistory[0];
			user->nRoundsSinceSpecial = 0;
		}
	}
}


/*
	attacker attacts to defender. Defender's health is decreased by attack of attacker minus defence of defender.
	This functin calculates Dwarfs' special damage if it is activated (if useSpecial==true).
	Returns true if the defender is died by this attack, otherwise returns false.
*/
bool attackTo(Character* attacker, Character* defender, bool useSpecial){
	// returns true if defender is killed.
	int damage = attacker->attack - defender->defense;
	damage = max(damage, 0);
	if (useSpecial){
		if (attacker->type == "Dwarfs" && attacker->nRoundsSinceSpecial >= 20){
			damage *= 2;
			attacker->nRoundsSinceSpecial = 0; // because at the and of the round it increases
		}
	}
	bool killed = false;
	if (defender->remainingHealth <= damage){
		defender->isAlive = false;
		defender->remainingHealth = 0;
		killed = true;
	}
	else{
		defender->remainingHealth -= damage;
	}
	return killed;
}


/*
	Searchs character of given type in given community(v).
	Returns index of position of searched character.
*/
int searchByType(vector<Character*>& v, string type){
	
	for(int i=0; i<v.size(); i++){
		if(v[i]->type == type){
			return i;
		}
	}
	return -1;
} 

/*
	Searchs character of given name in given community(v).
	Returns index of position of searched character.
*/
int searchByName(vector<Character*>& v, string name){
	for(int i=0; i<v.size(); i++){
		if(v[i]->name == name){
			return i;
		}
	}
	return -1;
} 


/*
	Updated the given character's health history at this round and increases nRoundsSinceSpecial by one.
	This funciton should be called for all characters at the end of every round.
*/
void endTurn(Character* c, int round){
	c->nRoundsSinceSpecial += 1;
	c->healthHistory[round] = c->remainingHealth;
}


/*
	Calls endTurn function for every member of the given community.
*/
void endTurnOfCommunity(vector<Character*>& community, int round){
	for (auto it = community.begin(); it<community.end(); it++){
		endTurn(*it, round);
	}
}


/*
	Prints the given character in needed output format. (name health_history_array_element1, health_history_array_element2...)
	round is the number of written health history elements.
*/
void printCharacter(Character* c, int round, ostream& out){
	out << c->name << ' ';
	for (int i=0; i<round; i++){
		out << c->healthHistory[i] << ' ';
	}
	out << endl;
}


/*
	Calls printCharacter function for every member of the given community.
*/
void printCommunity(vector<Character*>& community, int round, ostream& out){
	for (auto it=community.begin(); it!=community.end(); it++){
		printCharacter(*it, round, out);
	}
}



/*
	v should be sorted in order of increasing names.
	c is inserted to the appropriate position of v not breaking the order.
*/
void insertToCommunity(Character* c, vector<Character*>& v){
	
	for(auto it=v.begin(); it<v.end(); it++){
		if(*c < **it){
			v.insert(it, c);
			return;
		}
	}
	v.push_back(c);
}


/*
	If the character of the given index of the given community is alive returns that index.
	If it is not, returns index of the next alive character in the given community.
	If there is no next alive character, then looks at the previous characters.
	If there is no alive character in the community then returns -1 but this case shouldn't take place.
*/
int findActiveMember(vector<Character*> community, int index){
	for (int i = index; i<community.size(); i++){
		if (community[i]->isAlive){
			return i;
		}
	}
	for (int i = index-1; i>=0; i--){
		if (community[i]->isAlive){
			return i;
		}
	}
	return -1;
}


/*
	Checks if the given community is lost the game. (if hobbit is or all other 4 characters are dead)
	returns true if the community lost the game, otherwise returns false.
*/
bool isEnded(vector<Character*>& community){
	int dead_num = 0;
	for (auto c : community){
		if (c->isAlive){
			continue;
		}
		if (c->name == "Hobbit"){
			return true;
		}
		else {
			dead_num ++;
		}
	}
	return (dead_num == 4);
}


/*
	returns number of killled characters of the given community.
*/
int killedNumber(vector<Character*> community){
	int result = 0;
	for (auto c: community){
		if (!c->isAlive){
			result++;
		}
	}
	return result;
}