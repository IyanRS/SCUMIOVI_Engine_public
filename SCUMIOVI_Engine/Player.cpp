#include "Player.hpp"

void Player::disableMovement() {
    canPlayerMove = false;
}

void Player::enableMovement() {
    canPlayerMove = true;
}

bool Player::canMove() {
    return canPlayerMove;
}

Verb* Player::getVerb(std::string verbName) {
	for (Verb& v : verbs) {
		if (v.getName() == verbName) {
			return &v;
		}
	}
	return nullptr;
}

void Player::addVerb(Verb& verb) {
	verbs.push_back(verb);
}

void Player::setVerb(std::string verbName) {
	for (Verb& v : verbs) {
		if (v.getName() == verbName) {
			selectedVerb = &v;
		}
	}
}

void Player::setVerb(Verb* verb) {
	selectedVerb = verb;
}

Verb* Player::getSelectedVerb() {
	return selectedVerb;
}