#pragma once

#include "Vector2.hpp"

class Player;
class Interactable;
class Game;

class EventInfo {
public:
	Vector2<> clickPos;
	Player& player;
	Interactable& interactable;
	Game& game;
	EventInfo(Vector2<> clickPos, Player& player, Interactable& interactable);
};