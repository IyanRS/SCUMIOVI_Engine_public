#include "Player.hpp"
#include "Game.hpp"
#include <memory>

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
	game.getUI().updateUI();
}

void Player::setVerb(std::shared_ptr<Verb> verb) {
	selectedVerb = verb;
	Game::getInstance().getUI().updateUI();
}

std::shared_ptr<Verb> Player::getVerb() {
	return selectedVerb;
}

Inventory& Player::getInventory() {
	return inventory;
}

std::shared_ptr<GameObject> Player::getSelectedObject() {
	return selectedObject.lock();
}

void Player::setSelectedObject(std::shared_ptr<GameObject> gameObject) {
	this->selectedObject = gameObject;
	Game::getInstance().getUI().updateUI();
}

void Player::setTargetObject(std::shared_ptr<GameObject> gameObject) {
	this->targetObject = gameObject;
	Game::getInstance().getUI().updateUI();
}

std::shared_ptr<GameObject> Player::getTargetObject() {
	return targetObject.lock();
}