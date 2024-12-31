#include "EventInfo.hpp"

#include "Game.hpp"

EventInfo::EventInfo(Vector2<> clickPos, Player& player, Interactable& interactable) : player(player), interactable(interactable), game(Game::getInstance()) {
	this->clickPos = clickPos;
}