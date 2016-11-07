/*
 * Font.hpp
 *
 *  Created on: November 6th, 2016
 *  Author: Daniel Gerendasy
 */

#ifndef FONT_HPP_
#define FONT_HPP_

#include <Mini2D/Mini2D.hpp>				// Class definition

class Font {
public:

	Font(Mini2D * mini);
	virtual ~Font();

private:
	Mini2D * _mini;
};

#endif /* FONT_HPP_ */