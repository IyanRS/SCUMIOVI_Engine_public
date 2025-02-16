#include "Player.hpp"

#include "Game.hpp"

void Player::disableMovement() {
    canPlayerMove = false;
	Game& game = Game::getInstance();
	game.clearClicksPool();
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

Inventory& Player::getInventory() {
	return inventory;
}

GameObject* Player::getSelectedObject() {
	return selectedObject;
}

void Player::setSelectedObject(GameObject* gameObject) {
	this->selectedObject = gameObject;
}

void Player::setTargetObject(GameObject* gameObject) {
	this->targetObject = gameObject;
}

GameObject* Player::getTargetObject() {
	return targetObject;
}