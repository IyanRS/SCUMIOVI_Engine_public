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