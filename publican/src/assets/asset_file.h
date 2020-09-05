#pragma once
#ifndef PUB_ASSET_FILE_H
#define PUB_ASSET_FILE_H

#include <stdint.h>

#define PUB_FILE(a, b, c, d) (((uint32_t)(a) << 0) |\
							  ((uint32_t)(b) << 8) |\
							  ((uint32_t)(c) << 16) |\
							  ((uint32_t)(d) << 24))

#pragma pack(push, 1)
struct pfile_header {
#define ASSET_FILE_EXT PUB_FILE('p', 'u', 'b', 'b')
	uint32_t name;

	uint32_t typeCount;
	uint32_t assetCount;

	uint64_t types;
	uint64_t assets;
};

struct pfile_bmp {
	uint32_t x, y;
	float alignX, alignY;
};

struct pfile_mesh {
	uint32_t faceCount;
	uint32_t vertexCount;
};

struct pfile_string {
	uint32_t size;
};

struct pfile_type {
	uint32_t id;
	uint32_t firstAsset;
	uint32_t nextType;
};

struct pfile_asset{
	uint64_t offset;
	union {
		struct pfile_bmp bmp;
		struct pfile_mesh mesh;
		struct pfile_string string;
	};
};
#pragma pack(pop)

#endif