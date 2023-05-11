#pragma once

#include "ourGraphics.h"

//Freetype library needed to include this header

#include <ft2build.h>
#include FT_FREETYPE_H  

#include <map>

struct Character {
	unsigned int textureID;  // ID handle of the glyph texture
	glm::ivec2   Size;       // Size of glyph
	glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
	unsigned int Advance;    // Offset to advance to next glyph
};

class FontWorker
{
private:
	std::map<char, Character> Characters;
	FT_Library ft;
	FT_Face face;
public:
	FontWorker(string fontPath);
	void RenderText(Shader * s, std::string text, float x, float y, float scale, glm::vec3 color);
};
