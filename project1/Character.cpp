#include "Character.h"


/**
	* Constructs a Character having the given qualities.
	*
	* @param _name name of the character
	* @param _type type of the cahracter
	* @param _attack attack number of the character
	* @param _defense defense number of the character
	* @param _remainingHealth intial health of the character
	* @param _nMaxRounds max number of rounds this game can take
*/
Character::Character(string _name, string _type, int _attack, int _defense, int _remainingHealth, int _nMaxRounds) {
	this->name = _name;
	this->type = _type;
	this->attack = _attack;
	this->defense = _defense;
	this->remainingHealth = _remainingHealth;
	this->nMaxRounds = _nMaxRounds;

	this->nRoundsSinceSpecial = 0;
	this->healthHistory = new int[_nMaxRounds+1];
	this->healthHistory[0] = _remainingHealth;
}

/**
	* Copies the given character not changing it.
	*
	* @param character is a the copied character.
*/
Character::Character(const Character& character) {
	this->name = character.name;	
	this->type = character.type;
	this->attack = character.attack;
	this->defense = character.defense;
	this->remainingHealth = character.remainingHealth;
	this->nMaxRounds = character.nMaxRounds;

	this->nRoundsSinceSpecial = character.nRoundsSinceSpecial;
	this->healthHistory = new int[character.nMaxRounds+1];
	for (int i=0; i<character.nMaxRounds+1; i++){
		this->healthHistory[i] = character.healthHistory[i];
	}
}

/**
	* Makes the values of all variables and arrays of this character the same with the given character.
	*
	* @param character the assigned character
	* @return this character as a Character reference
*/
Character& Character::operator=(const Character& character) {
	// if healthHistory's size is the same then don't delete and recreate it.
	if (this == &character){
		return *this;
	}
	this->name = character.name;	
	this->type = character.type;
	this->attack = character.attack;
	this->defense = character.defense;
	this->remainingHealth = character.remainingHealth;
	this->nMaxRounds = character.nMaxRounds;

	this->nRoundsSinceSpecial = character.nRoundsSinceSpecial;
	delete this->healthHistory;
	this->healthHistory = new int[character.nMaxRounds+1];
	for (int i=0; i<character.nMaxRounds+1; i++){
		this->healthHistory[i] = character.healthHistory[i];
	}
	return *this;
}

/**
	* Compares names of the characters. If this character's name comes in dictinary before, returns true.
	* Otherwise returns false.
	* return true if this character's name comes in dictinary before, otherwise false.
*/
bool Character::operator<(const Character& other) {
	// Upper and lower case changes the result.	
	return this->name.compare(other.name) < 0;

}


/**
	* Deconstructs this character objects.
*/
Character::~Character() {
	delete this->healthHistory;
}
