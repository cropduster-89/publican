/************************************************************************************
__________     ___.   .__  .__                      	file_parser.c
\______   \__ _\_ |__ |  | |__| ____ _____    ____  	Converts an .xml file to a
 |     ___/  |  \ __ \|  | |  |/ ___\\__  \  /    \ 	struct asset_plan
 |    |   |  |  / \_\ \  |_|  \  \___ / __ \|   |  \
 |____|   |____/|___  /____/__|\___  >____  /___|  /
                    \/             \/     \/     \/
*************************************************************************************/

#define __STDC_LIB_EXT1__ 1

#include <assert.h>
#include <inttypes.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "asset_builder.h"
#include "../../publican/src/utils/utils.h"

static uint32_t GetIntFromAttr(xmlNode *node,
							   char *attrName)
{
	char *idStr = (char *)xmlGetProp(node, (xmlChar *)attrName);
	char *endptr;
	uint32_t result = strtoumax(idStr, &endptr, 10);
	xmlFree(idStr);

	return(result);
}

static void AddString(xmlNode *node,
					  struct asset_plan *plan)
{
	assert(plan->currentType);
	assert(plan->currentType->nextType < ARRAY_COUNT(plan->assets));

	struct string_id result = {plan->currentType->nextType++};
	struct asset_source *src = plan->sources + result.val;

	src->format = FORMAT_STRING;

	char *str = (char *)xmlNodeGetContent(node);
	int32_t len = strlen(str);
	if(len < MAX_STRING_SIZE) {
		src->string.text = str;
	} else {
		plan->error = ERR_STRING_EXCEEDS_BUFFER_SIZE;
	}
}

static void AddBMP(xmlNode *node,
				   struct asset_plan *plan)
{
	assert(plan->currentType);
	assert(plan->currentType->nextType < ARRAY_COUNT(plan->assets));

	struct bmp_id id = { plan->currentType->nextType++ };
	struct asset_source *src = plan->sources + id.val;

	src->format = FORMAT_BMP;

	char *pathStr = (char *)xmlGetProp(node, (xmlChar *)"path");
	int32_t len = strlen(pathStr);
	if(len < MAX_PATH_SIZE) {
		src->bmp.fileName = pathStr;
		src->bmp.sideInPxels = GetIntFromAttr(node, "size");
	} else {
		plan->error = ERR_STRING_EXCEEDS_BUFFER_SIZE;
	}
}

static void AddChar(char *fileName,
					uint32_t codepoint,
					struct asset_plan *plan)
{
	assert(plan->currentType);
	assert(plan->currentType->nextType < ARRAY_COUNT(plan->assets));

	struct bmp_id result = {plan->currentType->nextType++};
	struct asset_source *src = plan->sources + result.val;

	src->format = FORMAT_FONT;
	src->glyph.fileName = fileName;
	src->glyph.codepoint = codepoint;
}

static void StartId(enum asset_type_id type,
				    struct asset_plan *plan)
{
	assert(plan->currentType == 0);

	plan->currentType = plan->types + type;
	plan->currentType->id = type;
	plan->currentType->firstAsset = plan->assetCount;
	plan->currentType->nextType = plan->currentType->firstAsset;

}

static void EndId(struct asset_plan *plan)
{
	assert(plan->currentType);

	plan->assetCount = plan->currentType->nextType;
	plan->currentType = 0;
	plan->index = 0;
}

static void ParseFile(xmlNode *node,
					  bool isAsset,
					  struct asset_plan *plan)
{
	if(plan->error != ERR_NONE) return;

	for(xmlNode *current = node; current; current = current->next) {
		if(current->type != XML_ELEMENT_NODE) { goto escape; }

		if(!strcmp((char *)current->name, "asset")) {
			if(!isAsset) {
				isAsset = true;
				StartId(GetIntFromAttr(current, "assetTypeId"), plan);
			} else {
				EndId(plan);
				StartId(GetIntFromAttr(current, "assetTypeId"), plan);
			}
		}

		if(!strcmp((char *)current->name, "bmp")) {
			if(isAsset) {
				AddBMP(current, plan);
			} else {
				plan->error = ERR_MALFORMED_XML;
			}
		}

		if(!strcmp((char *)current->name, "string")) {
			if(isAsset) {
				AddString(current, plan);
			} else {
				plan->error = ERR_MALFORMED_XML;
			}
		}

		if(!strcmp((char *)current->name, "font")) {
			if(isAsset) {
				char *fileName = (char *)xmlGetProp(current, (xmlChar *)"path");
				for(uint32_t i = '!'; i < '~'; ++i) {
					AddChar(fileName, i, plan);
				}
			} else {
				plan->error = ERR_MALFORMED_XML;
			}
		}
escape:
		ParseFile(current->children, isAsset, plan);
	}
}

extern void GetPlanFromFile(char *fileName,
							struct asset_plan *plan)
{
	LIBXML_TEST_VERSION

	xmlDoc *doc = xmlReadFile(fileName, NULL, 0);
	if(!doc) {
		printf("Failed to load %s\n", fileName);
	}
	xmlNode *root = xmlDocGetRootElement(doc);

	ParseFile(root, false, plan);
	EndId(plan);

	xmlFreeDoc(doc);
	xmlCleanupParser();
}