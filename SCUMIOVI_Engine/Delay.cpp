#include "Delay.hpp"

Delay::Delay(float seconds) {
	this->seconds = seconds;
}

float Delay::getSeconds() {
	return seconds;
}