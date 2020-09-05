#pragma once
#ifndef ASSET_BUILDER_H
#define ASSET_BUILDER_H

/************************************************************************************
__________     ___.   .__  .__                      	Asset builder header
\______   \__ _\_ |__ |  | |__| ____ _____    ____
 |     ___/  |  \ __ \|  | |  |/ ___\\__  \  /    \
 |    |   |  |  / \_\ \  |_|  \  \___ / __ \|   |  \
 |____|   |____/|___  /____/__|\___  >____  /___|  /
                    \/             \/     \/     \/
*************************************************************************************/

#include <stdint.h>

#include "../../publican/src/assets/assets.h"
#include "../../publican/src/assets/asset_file.h"

#define MAX_FILENAME_SIZE 32
#define MAX_PATH_SIZE 64
#define MAX_STRING_SIZE 256

enum asset_builder_error {
	ERR_NONE,
	ERR_MALFORMED_XML,
	ERR_STRING_EXCEEDS_BUFFER_SIZE,
	ERR_FILE_CREATION_FAILED
};

enum asset_format_id {
	FORMAT_BMP,
	FORMAT_STRING,
	FORMAT_FONT
};

struct asset_source_glyph {
	char *fileName;
	uint32_t codepoint;
};

struct asset_source_bmp {
	char *fileName;
	uint8_t sideInPxels;
};

struct asset_source_string {
	char *text;
};

struct asset_source {
	enum asset_format_id format;
	union {
		struct asset_source_bmp bmp;
		struct asset_source_string string;
		struct asset_source_glyph glyph;
	};
};

struct asset_plan {
	uint32_t typeCount;

	struct pfile_type types[ASSET_ID_COUNT];
	struct pfile_type *currentType;

	uint32_t assetCount;
	struct asset_source sources[4096];
	struct pfile_asset assets[4096];
	uint32_t index;

	enum asset_builder_error error;
};

struct bmp_id {
	uint32_t val;
};

struct string_id {
	uint32_t val;
};

void GetPlanFromFile(char *fileName, struct asset_plan *plan);
void GetFileName(char *inFileName, char *outFileName);
void BuildFile(struct asset_plan *plan, char *fileName);

#endif