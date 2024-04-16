/*
 * Wespal (codename Morning Star) - Wesnoth assets recoloring tool
 *
 * Copyright (C) 2010 - 2024 by Iris Morelle <iris@irydacea.me>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "defs.hpp"

#include <QObject>

namespace {

inline QString tr(const char* text)
{
	return QObject::tr(text);
}

} // end unnamed namespace

namespace wesnoth {

//
// Definitions taken from /data/core/team-colors.cfg
//

const BuiltinColorRanges builtinColorRanges = {
	// Unit TC
	//
	// NOTE: For some inexplicable reason, upstream Wesnoth uses rep identical
	//       to Red's for Light and Dark Red. We don't because it would be
	//       confusing to look at/result in spurious bug reports.
	{ "red",			tr("Red"),				{ 0xFF0000, 0xFFFFFF, 0x000000, 0xFF0000 } },
	{ "blue",			tr("Blue"),				{ 0x2E419B, 0xFFFFFF, 0x0F0F0F, 0x0000FF } },
	{ "green",			tr("Green"),			{ 0x62B664, 0xFFFFFF, 0x000000, 0x00FF00 } },
	{ "purple",			tr("Purple"),			{ 0x93009D, 0xFFFFFF, 0x000000, 0xFF00FF } },
	{ "black",			tr("Black"),			{ 0x5A5A5A, 0xFFFFFF, 0x000000, 0x000000 } },
	{ "brown",			tr("Brown"),			{ 0x945027, 0xFFFFFF, 0x000000, 0xAA4600 } },
	{ "orange",			tr("Orange"),			{ 0xFF7E00, 0xFFFFFF, 0x0F0F0F, 0xFFAA00 } },
	{ "white",			tr("White"),			{ 0xE1E1E1, 0xFFFFFF, 0x1E1E1E, 0xFFFFFF } },
	{ "teal",			tr("Teal"),				{ 0x30CBC0, 0xFFFFFF, 0x000000, 0x00F0C8 } },
	{ "lightred",		tr("Light Red"),		{ 0xD1620D, 0xFFFFFF, 0x000000, 0xD1620D } },
	{ "darkred",		tr("Dark Red"),			{ 0x8A0808, 0xFFFFFF, 0x000000, 0x8A0808 } },
	{ "lightblue",		tr("Light Blue"),		{ 0x00A4FF, 0xFFFFFF, 0x000A21, 0x00A4FF } },
	{ "brightgreen",	tr("Bright Green"),		{ 0x8CFF00, 0xEBFFBF, 0x2D4001, 0x8CFF00 } },
	{ "brightorange",	tr("Bright Orange"),	{ 0xFFC600, 0xFFF7E6, 0x792A00, 0xFFC600 } },
	{ "gold",			tr("Gold"),				{ 0xFFF35A, 0xFFF8D2, 0x994F13, 0xFFF35A } },
#if 0
	// Terrain icon color ranges
	{ "reef",			tr("Reef"),				{ 0x00B6E3, 0x09FFDB, 0x00090D, 0x2F8399 } },
	{ "shallow_water",	tr("Shallow Water"),	{ 0x027CC4, 0x00FFE7, 0x00223F, 0x296487 } },
	{ "deep_water",		tr("Deep Water"),		{ 0x00419E, 0x5CD6FF, 0x000B15, 0x234372 } },
	{ "swamp_water",	tr("Swamp Water"),		{ 0x33978C, 0x38FF8B, 0x092A1E, 0x3C706A } },
	{ "flat",			tr("Flat"),				{ 0x89C236, 0xD9FF62, 0x023B02, 0x73973F } },
	{ "hills",			tr("Hills"),			{ 0xC0B030, 0xFBFF95, 0x353B00, 0x71692F } },
	{ "mountains",		tr("Mountains"),		{ 0x916B09, 0xE9DA7D, 0x3B3406, 0x564823 } },
	{ "forest",			tr("Forest"),			{ 0x263E01, 0x69B70A, 0x142303, 0x314214 } },
	{ "sand",			tr("Sand"),				{ 0xF1CE6C, 0xFFFCC0, 0x4A3212, 0xB19D66 } },
	{ "frozen",			tr("Frozen"),			{ 0x95B5E6, 0xFFF6BD, 0x180E5F, 0x8495AE } },
	{ "cave",			tr("Cave"),				{ 0x5A517D, 0xBBA1FF, 0x0A0616, 0x3C355C } },
	{ "fungus",			tr("Fungus"),			{ 0x703816, 0xFB8435, 0x2B150B, 0x54301A } },
	{ "village",		tr("Village"),			{ 0x995B51, 0xFFE0DB, 0x24190F, 0x713C33 } },
	{ "castle",			tr("Castle"),			{ 0x988C85, 0xFFF0E1, 0x171514, 0x6C615B } },
	{ "keep",			tr("Keep"),				{ 0x6E6059, 0xFFF0E1, 0x171514, 0x4F423C } },
	{ "rails",			tr("Rails"),			{ 0x4C5E6E, 0xFFFFFF, 0x181E24, 0x324250 } },
	{ "unwalkable",		tr("Unwalkable"),		{ 0xBD001F, 0xFF1D29, 0x240506, 0x850016 } },
	{ "impassable",		tr("Impassable"),		{ 0x7D0013, 0xD81721, 0x240506, 0x58000D } },
	{ "fog",			tr("Fog"),				{ 0x686868, 0xFFFFFF, 0x000000, 0x686868 } },
	{ "shroud",			tr("Shroud"),			{ 0x313131, 0x7E7E7E, 0x000000, 0x313131 } },
#endif
};

const BuiltinPalettes builtinPalettes = {
	{
		"magenta", tr("Magenta TC"), {
			0xF49AC1, 0x3F0016, 0x55002A, 0x690039, 0x7B0045, 0x8C0051, 0x9E005D,
			0xB10069, 0xC30074, 0xD6007F, 0xEC008C, 0xEE3D96, 0xEF5BA1, 0xF172AC,
			0xF287B6, 0xF6ADCD, 0xF8C1D9, 0xFAD5E5, 0xFDE9F1
		}
	},
	{
		"flag_green", tr("Green Flag TC"), {
			0x00C800, 0x00FF00, 0x00FE00, 0x00FD00, 0x00FC00, 0x00FB00, 0x00FA00,
			0x00F900, 0x00F800, 0x00F700, 0x00F600, 0x00F500, 0x00F400, 0x00F300,
			0x00F200, 0x00F100, 0x00F000, 0x00EF00, 0x00EE00, 0x00ED00, 0x00EC00,
			0x00EB00, 0x00EA00, 0x00E900, 0x00E800, 0x00E700, 0x00E600, 0x00E500,
			0x00E400, 0x00E300, 0x00E200, 0x00E100, 0x00E000, 0x00DF00, 0x00DE00,
			0x00DD00, 0x00DC00, 0x00DB00, 0x00DA00, 0x00D900, 0x00D800, 0x00D700,
			0x00D600, 0x00D500, 0x00D400, 0x00D300, 0x00D200, 0x00D100, 0x00D000,
			0x00CF00, 0x00CE00, 0x00CD00, 0x00CC00, 0x00CB00, 0x00CA00, 0x00C900,
			0x00C700, 0x00C600, 0x00C500, 0x00C400, 0x00C300, 0x00C200, 0x00C100,
			0x00C000, 0x00BF00, 0x00BE00, 0x00BD00, 0x00BC00, 0x00BB00, 0x00BA00,
			0x00B900, 0x00B800, 0x00B700, 0x00B600, 0x00B500, 0x00B400, 0x00B300,
			0x00B200, 0x00B100, 0x00B000, 0x00AF00, 0x00AE00, 0x00AD00, 0x00AC00,
			0x00AB00, 0x00AA00, 0x00A900, 0x00A800, 0x00A700, 0x00A600, 0x00A500,
			0x00A400, 0x00A300, 0x00A200, 0x00A100, 0x00A000, 0x009F00, 0x009E00,
			0x009D00, 0x009C00, 0x009B00, 0x009A00, 0x009900, 0x009800, 0x009700,
			0x009600, 0x009500, 0x009400, 0x009300, 0x009200, 0x009100, 0x009000,
			0x008F00, 0x008E00, 0x008D00, 0x008C00, 0x008B00, 0x008A00, 0x008900,
			0x008800, 0x008700, 0x008600, 0x008500, 0x008400, 0x008300, 0x008200,
			0x008100, 0x008000, 0x007F00, 0x007E00, 0x007D00, 0x007C00, 0x007B00,
			0x007A00, 0x007900, 0x007800, 0x007700, 0x007600, 0x007500, 0x007400,
			0x007300, 0x007200, 0x007100, 0x007000, 0x006F00, 0x006E00, 0x006D00,
			0x006C00, 0x006B00, 0x006A00, 0x006900, 0x006800, 0x006700, 0x006600,
			0x006500, 0x006400, 0x006300, 0x006200, 0x006100, 0x006000, 0x005F00,
			0x005E00, 0x005D00, 0x005C00, 0x005B00, 0x005A00, 0x005900, 0x005800,
			0x005700, 0x005600, 0x005500, 0x005400, 0x005300, 0x005200, 0x005100,
			0x005000, 0x004F00, 0x004E00, 0x004D00, 0x004C00, 0x004B00, 0x004A00,
			0x004900, 0x004800, 0x004700, 0x004600, 0x004500, 0x004400, 0x004300,
			0x004200, 0x004100, 0x004000, 0x003F00, 0x003E00, 0x003D00, 0x003C00,
			0x003B00, 0x003A00, 0x003900, 0x003800, 0x003700, 0x003600, 0x003500,
			0x003400, 0x003300, 0x003200, 0x003100, 0x003000, 0x002F00, 0x002E00,
			0x002D00, 0x002C00, 0x002B00, 0x002A00, 0x002900, 0x002800, 0x002700,
			0x002600, 0x002500, 0x002400, 0x002300, 0x002200, 0x002100, 0x002000,
			0x001F00, 0x001E00, 0x001D00, 0x001C00, 0x001B00, 0x001A00, 0x001900,
			0x001800, 0x001700, 0x001600, 0x001500, 0x001400, 0x001300, 0x001200,
			0x001100, 0x001000, 0x000F00, 0x000E00, 0x000D00, 0x000C00, 0x000B00,
			0x000A00, 0x000900, 0x000800, 0x000700, 0x000600, 0x000500, 0x000400,
			0x000300, 0x000200, 0x000100
		}
	},
	{
		"ellipse_red", tr("Red Ellipse TC"), {
			0xC80000, 0xFF0000, 0xFE0000, 0xFD0000, 0xFC0000, 0xFB0000, 0xFA0000,
			0xF90000, 0xF80000, 0xF70000, 0xF60000, 0xF50000, 0xF40000, 0xF30000,
			0xF20000, 0xF10000, 0xF00000, 0xEF0000, 0xEE0000, 0xED0000, 0xEC0000,
			0xEB0000, 0xEA0000, 0xE90000, 0xE80000, 0xE70000, 0xE60000, 0xE50000,
			0xE40000, 0xE30000, 0xE20000, 0xE10000, 0xE00000, 0xDF0000, 0xDE0000,
			0xDD0000, 0xDC0000, 0xDB0000, 0xDA0000, 0xD90000, 0xD80000, 0xD70000,
			0xD60000, 0xD50000, 0xD40000, 0xD30000, 0xD20000, 0xD10000, 0xD00000,
			0xCF0000, 0xCE0000, 0xCD0000, 0xCC0000, 0xCB0000, 0xCA0000, 0xC90000,
			0xC70000, 0xC60000, 0xC50000, 0xC40000, 0xC30000, 0xC20000, 0xC10000,
			0xC00000, 0xBF0000, 0xBE0000, 0xBD0000, 0xBC0000, 0xBB0000, 0xBA0000,
			0xB90000, 0xB80000, 0xB70000, 0xB60000, 0xB50000, 0xB40000, 0xB30000,
			0xB20000, 0xB10000, 0xB00000, 0xAF0000, 0xAE0000, 0xAD0000, 0xAC0000,
			0xAB0000, 0xAA0000, 0xA90000, 0xA80000, 0xA70000, 0xA60000, 0xA50000,
			0xA40000, 0xA30000, 0xA20000, 0xA10000, 0xA00000, 0x9F0000, 0x9E0000,
			0x9D0000, 0x9C0000, 0x9B0000, 0x9A0000, 0x990000, 0x980000, 0x970000,
			0x960000, 0x950000, 0x940000, 0x930000, 0x920000, 0x910000, 0x900000,
			0x8F0000, 0x8E0000, 0x8D0000, 0x8C0000, 0x8B0000, 0x8A0000, 0x890000,
			0x880000, 0x870000, 0x860000, 0x850000, 0x840000, 0x830000, 0x820000,
			0x810000, 0x800000, 0x7F0000, 0x7E0000, 0x7D0000, 0x7C0000, 0x7B0000,
			0x7A0000, 0x790000, 0x780000, 0x770000, 0x760000, 0x750000, 0x740000,
			0x730000, 0x720000, 0x710000, 0x700000, 0x6F0000, 0x6E0000, 0x6D0000,
			0x6C0000, 0x6B0000, 0x6A0000, 0x690000, 0x680000, 0x670000, 0x660000,
			0x650000, 0x640000, 0x630000, 0x620000, 0x610000, 0x600000, 0x5F0000,
			0x5E0000, 0x5D0000, 0x5C0000, 0x5B0000, 0x5A0000, 0x590000, 0x580000,
			0x570000, 0x560000, 0x550000, 0x540000, 0x530000, 0x520000, 0x510000,
			0x500000, 0x4F0000, 0x4E0000, 0x4D0000, 0x4C0000, 0x4B0000, 0x4A0000,
			0x490000, 0x480000, 0x470000, 0x460000, 0x450000, 0x440000, 0x430000,
			0x420000, 0x410000, 0x400000, 0x3F0000, 0x3E0000, 0x3D0000, 0x3C0000,
			0x3B0000, 0x3A0000, 0x390000, 0x380000, 0x370000, 0x360000, 0x350000,
			0x340000, 0x330000, 0x320000, 0x310000, 0x300000, 0x2F0000, 0x2E0000,
			0x2D0000, 0x2C0000, 0x2B0000, 0x2A0000, 0x290000, 0x280000, 0x270000,
			0x260000, 0x250000, 0x240000, 0x230000, 0x220000, 0x210000, 0x200000,
			0x1F0000, 0x1E0000, 0x1D0000, 0x1C0000, 0x1B0000, 0x1A0000, 0x190000,
			0x180000, 0x170000, 0x160000, 0x150000, 0x140000, 0x130000, 0x120000,
			0x110000, 0x100000, 0x0F0000, 0x0E0000, 0x0D0000, 0x0C0000, 0x0B0000,
			0x0A0000, 0x090000, 0x080000, 0x070000, 0x060000, 0x050000, 0x040000,
			0x030000, 0x020000, 0x010000
		}
	}
};

} // end namespace wesnoth
