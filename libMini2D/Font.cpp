/*
 * Font.cpp
 *
 *  Created on: November 6th, 2016
 *  Author: Daniel Gerendasy
 */

#include <Mini2D/Font.hpp>

Font::Font(Mini2D * mini) :
		_mini(mini) {

}

Font::~Font() {
	_mini = NULL;
}
