//camera.h
//created by SteDeshain
//created on 2016/08/13
//latest updated on 2016/08/13
//sources from the Internet

#ifndef __STE_COLOR_H__
#define __STE_COLOR_H__

#include "SDL2/SDL.h"

#define DRAW_RECT_COLOR             Green
#define CENTER_COLOR                Yellow
#define COLLIDE_BOX_COLOR           Cyan
#define SLEEP_COLLIDE_BOX_COLOR     Gray
#define GROUND_TOUCH_COLOR          Black
#define PHYPOS_COLOR                Blue

#define GET_R(color)        (color >> 16)
#define GET_G(color)        ((color >> 8) & 0xFF)
#define GET_B(color)        (color & 0xFF)

namespace steg
{
	enum RGBColor
	{
		Black = 0x000000,
		Navy = 0x000080,
		DarkBlue = 0x00008B,
		MediumBlue = 0x0000CD,
		Blue = 0x0000FF,
		DarkGreen = 0x006400,
		Green = 0x008000,
		Teal = 0x008080,
		DarkCyan = 0x008B8B,
		DeepSkyBlue = 0x00BFFF,
		DarkTurquoise = 0x00CED1,
		MediumSpringGreen = 0x00FA9A,
		Lime = 0x00FF00,
		SpringGreen = 0x00FF7F,
		Aqua = 0x00FFFF,
		Cyan = 0x00FFFF,
		MidnightBlue = 0x191970,
		DodgerBlue = 0x1E90FF,
		LightSeaGreen = 0x20B2AA,
		ForestGreen = 0x228B22,
		SeaGreen = 0x2E8B57,
		DarkSlateGray = 0x2F4F4F,
		LimeGreen = 0x32CD32,
		MediumSeaGreen = 0x3CB371,
		Turquoise = 0x40E0D0,
		RoyalBlue = 0x4169E1,
		SteelBlue = 0x4682B4,
		DarkSlateBlue = 0x483D8B,
		MediumTurquoise = 0x48D1CC,
		Indigo = 0x4B0082,
		DarkOliveGreen = 0x556B2F,
		CadetBlue = 0x5F9EA0,
		CornflowerBlue = 0x6495ED,
		MediumAquaMarine = 0x66CDAA,
		DimGray = 0x696969,
		SlateBlue = 0x6A5ACD,
		OliveDrab = 0x6B8E23,
		SlateGray = 0x708090,
		LightSlateGray = 0x778899,
		MediumSlateBlue = 0x7B68EE,
		LawnGreen = 0x7CFC00,
		Chartreuse = 0x7FFF00,
		Aquamarine = 0x7FFFD4,
		Maroon = 0x800000,
		Purple = 0x800080,
		Olive = 0x808000,
		Gray = 0x808080,
		SkyBlue = 0x87CEEB,
		LightSkyBlue = 0x87CEFA,
		BlueViolet = 0x8A2BE2,
		DarkRed = 0x8B0000,
		DarkMagenta = 0x8B008B,
		SaddleBrown = 0x8B4513,
		DarkSeaGreen = 0x8FBC8F,
		LightGreen = 0x90EE90,
		MediumPurple = 0x9370DB,
		DarkViolet = 0x9400D3,
		PaleGreen = 0x98FB98,
		DarkOrchid = 0x9932CC,
		YellowGreen = 0x9ACD32,
		Sienna = 0xA0522D,
		Brown = 0xA52A2A,
		DarkGray = 0xA9A9A9,
		LightBlue = 0xADD8E6,
		GreenYellow = 0xADFF2F,
		PaleTurquoise = 0xAFEEEE,
		LightSteelBlue = 0xB0C4DE,
		PowderBlue = 0xB0E0E6,
		FireBrick = 0xB22222,
		DarkGoldenRod = 0xB8860B,
		MediumOrchid = 0xBA55D3,
		RosyBrown = 0xBC8F8F,
		DarkKhaki = 0xBDB76B,
		Silver = 0xC0C0C0,
		MediumVioletRed = 0xC71585,
		IndianRed = 0xCD5C5C,
		Peru = 0xCD853F,
		Chocolate = 0xD2691E,
		Tan = 0xD2B48C,
		LightGray = 0xD3D3D3,
		Thistle = 0xD8BFD8,
		Orchid = 0xDA70D6,
		GoldenRod = 0xDAA520,
		PaleVioletRed = 0xDB7093,
		Crimson = 0xDC143C,
		Gainsboro = 0xDCDCDC,
		Plum = 0xDDA0DD,
		BurlyWood = 0xDEB887,
		LightCyan = 0xE0FFFF,
		Lavender = 0xE6E6FA,
		DarkSalmon = 0xE9967A,
		Violet = 0xEE82EE,
		PaleGoldenRod = 0xEEE8AA,
		LightCoral = 0xF08080,
		Khaki = 0xF0E68C,
		AliceBlue = 0xF0F8FF,
		HoneyDew = 0xF0FFF0,
		Azure = 0xF0FFFF,
		SandyBrown = 0xF4A460,
		Wheat = 0xF5DEB3,
		Beige = 0xF5F5DC,
		WhiteSmoke = 0xF5F5F5,
		MintCream = 0xF5FFFA,
		GhostWhite = 0xF8F8FF,
		Salmon = 0xFA8072,
		AntiqueWhite = 0xFAEBD7,
		Linen = 0xFAF0E6,
		LightGoldenRodYellow = 0xFAFAD2,
		OldLace = 0xFDF5E6,
		Red = 0xFF0000,
		Fuchsia = 0xFF00FF,
		Magenta = 0xFF00FF,
		DeepPink = 0xFF1493,
		OrangeRed = 0xFF4500,
		Tomato = 0xFF6347,
		HotPink = 0xFF69B4,
		Coral = 0xFF7F50,
		DarkOrange = 0xFF8C00,
		LightSalmon = 0xFFA07A,
		Orange = 0xFFA500,
		LightPink = 0xFFB6C1,
		Pink = 0xFFC0CB,
		Gold = 0xFFD700,
		PeachPuff = 0xFFDAB9,
		NavajoWhite = 0xFFDEAD,
		Moccasin = 0xFFE4B5,
		Bisque = 0xFFE4C4,
		MistyRose = 0xFFE4E1,
		BlanchedAlmond = 0xFFEBCD,
		PapayaWhip = 0xFFEFD5,
		LavenderBlush = 0xFFF0F5,
		SeaShell = 0xFFF5EE,
		Cornsilk = 0xFFF8DC,
		LemonChiffon = 0xFFFACD,
		FloralWhite = 0xFFFAF0,
		Snow = 0xFFFAFA,
		Yellow = 0xFFFF00,
		LightYellow = 0xFFFFE0,
		Ivory = 0xFFFFF0,
		White = 0xFFFFFF,
	};

	unsigned int ToRGBA(RGBColor c);

    SDL_Color GetColor(RGBColor c);
}

#endif // __STE_COLOR_H__