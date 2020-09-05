/************************************************************************************
__________     ___.   .__  .__                      	file_builder.c
\______   \__ _\_ |__ |  | |__| ____ _____    ____      converts asset plan to file
 |     ___/  |  \ __ \|  | |  |/ ___\\__  \  /    \
 |    |   |  |  / \_\ \  |_|  \  \___ / __ \|   |  \
 |____|   |____/|___  /____/__|\___  >____  /___|  /
                    \/             \/     \/     \/
*************************************************************************************/

#define __STDC_LIB_EXT1__ 1

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#define STB_ONLY_TGA

#include "asset_builder.h"
#include "stb_image.h"
#include "stb_truetype.h"

#define OUTPUT_PATH "../publican/data"

extern void GetFileName(char *inFileName,
						char *outFileName)
{
	int32_t inLen = strlen(inFileName);
	int32_t outLen = inLen + 1;

	strncpy_s(outFileName, outLen, inFileName, inLen);
	strncpy_s(&outFileName[outLen - 4], outLen, "pubb\0", 5);
}

static uint8_t *GetGlyphBytes(char *fileName)
{
	FILE *fh = NULL;
	fopen_s(&fh, fileName, "rb");
	fseek(fh , 0, SEEK_END);
	int32_t glyphSize = ftell(fh);
	rewind(fh);
	uint8_t *glyphBytes = malloc(glyphSize);
	fread(glyphBytes, glyphSize, 1, fh);
	fclose(fh);

	return glyphBytes;
}

static struct loaded_bmp LoadGlyphBMP(char *fileName,
									  uint32_t codepoint)
{
	struct loaded_bmp result = {};

	uint8_t *glyphBytes = GetGlyphBytes(fileName);

	stbtt_fontinfo font;
	stbtt_InitFont(&font, glyphBytes, stbtt_GetFontOffsetForIndex(glyphBytes, 0));
	uint8_t *monoBmp = stbtt_GetCodepointBitmap(&font, 0,
		stbtt_ScaleForPixelHeight(&font, 64.0f), codepoint,
		&result.x, &result.y, &result.alignX, &result.alignY);

	result.pitch = result.x * 4;
	result.data = malloc(result.y * result.pitch);
	uint8_t *src = monoBmp;
	uint8_t *destRow = (uint8_t *)result.data + (result.y - 1) * result.pitch;
	for(int32_t y = 0; y < result.y; ++y) {
		uint32_t *dest = (uint32_t *)destRow;
		for(int32_t x = 0; x < result.x; ++x) {
			uint8_t alpha = *src++;
			*dest++ = ((alpha << 24) |
				   (alpha << 16) |
				   (alpha <<  8) |
				   (alpha <<  0));
		}
		destRow -= result.pitch;
	}
	int32_t adv, left;
	stbtt_GetCodepointHMetrics(&font, codepoint, &adv, &left);
	stbtt_FreeBitmap(monoBmp, 0);
	free(glyphBytes);

	return(result);
}


static void LoadGlyph(struct asset_source *src,
			   		  struct pfile_asset *dest,
			   		  FILE *outFile)
{
	printf("* Loading glyph %c from %s\n", src->glyph.codepoint, src->glyph.fileName);

	struct loaded_bmp bmp = {};

	bmp = LoadGlyphBMP(src->glyph.fileName, src->glyph.codepoint);
	dest->bmp.x = bmp.x;
	dest->bmp.y = bmp.y;
	dest->bmp.alignX = bmp.alignX;
	dest->bmp.alignY = bmp.alignY;

	fwrite(bmp.data, bmp.x * bmp.y * 4, 1, outFile);
	free(bmp.data);

	printf("* Loaded.\n");
}

static void LoadBmp(struct asset_source *src,
			   		struct pfile_asset *dest,
			   		FILE *outFile)
{
	printf("* Loading .tgs file from %s\n", src->bmp.fileName);

	struct loaded_bmp bmp = {};

	int c;
	bmp.data = stbi_load(src->bmp.fileName, &bmp.x, &bmp.y, &c, 0);
	assert(bmp.data);
	bmp.pitch = bmp.x * 4;
	dest->bmp.x = bmp.x;
	dest->bmp.y = bmp.y;
	fwrite(bmp.data, bmp.x * bmp.y * 4, 1, outFile);

	stbi_image_free(bmp.data);

	printf("* Loaded.\n");
}

static struct pfile_header GetNewHeader(FILE *outFile,
										struct asset_plan *plan)
{
	struct pfile_header header = {
		.name = ASSET_FILE_EXT,
		.typeCount = ASSET_ID_COUNT,
		.assetCount = plan->assetCount
	};
	fwrite(&header, sizeof(header), 1, outFile);

	uint32_t assetTypeArraySize = header.typeCount * sizeof(struct pfile_type);
	uint32_t assetArraySize = plan->assetCount * sizeof(struct pfile_asset);
	header.types = sizeof(struct pfile_header);
	header.assets = header.types + assetTypeArraySize;
	fwrite(&plan->types, assetTypeArraySize, 1, outFile);
	fseek(outFile, assetArraySize, SEEK_CUR);

	return header;
}

extern void BuildFile(struct asset_plan *plan,
					  char *fileName)
{
	stbi_set_flip_vertically_on_load(1);

	int32_t len = strlen(fileName) + 1;
	char outFileName[len];
	GetFileName(fileName, outFileName);

	FILE *outFile = NULL;
	fopen_s(&outFile, outFileName, "wb");

	if(!outFile) {
		plan->error = ERR_FILE_CREATION_FAILED;
		return;
	}

	struct pfile_header header = GetNewHeader(outFile, plan);

	printf("Building %d assets for %d types...\n", plan->assetCount, plan->typeCount);

	for(uint32_t i = 1; i < header.assetCount; ++i) {
		struct asset_source *src = plan->sources + i;
		struct pfile_asset *dest = plan->assets + i;
		dest->offset = ftell(outFile);

		switch(src->format) {
		case FORMAT_BMP:
			LoadBmp(src, dest, outFile);
			break;
		case FORMAT_STRING:
			break;
		case FORMAT_FONT:
			LoadGlyph(src, dest, outFile);
			break;
		default: assert(0);
		}
	}
	uint32_t assetArraySize = plan->assetCount * sizeof(struct pfile_asset);
	fseek(outFile, (uint32_t)header.assets, SEEK_SET);
	fwrite(plan->assets, assetArraySize, 1, outFile);

	fclose(outFile);
}