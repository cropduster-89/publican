#pragma once
#ifndef PUB_ASSETS_H
#define PUB_ASSETS_H

#include <stdint.h>

enum asset_type_id {
	ASSET_ID_TILE,
	ASSET_ID_FONT_DEBUG,
	ASSET_ID_FONT_GAME,
	ASSET_ID_COUNT,
};

struct loaded_bmp {
	int32_t x;
	int32_t y;
	int32_t alignX;
	int32_t alignY;

	uint16_t pitch;
	void *data;
};

#endif