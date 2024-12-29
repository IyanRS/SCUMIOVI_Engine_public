#include "Player.hpp"

#include "Game.hpp"

void Player::disableMovement() {
    canPlayerMove = false;
}

void Player::enableMovement() {
    canPlayerMove = true;
}

bool Player::canMove() {
    return canPlayerMove;
}

void Player::setVerb(std::string verbName) {
	Game& game = Game::getInstance();
	selectedVerb = game.getVerb(verbName);
}

void Player::setVerb(Verb* verb) {
	selectedVerb = verb;
}

Verb* Player::getVerb() {
	return selectedVerb;
}