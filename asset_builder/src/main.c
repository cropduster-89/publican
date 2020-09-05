/************************************************************************************
__________     ___.   .__  .__                      	Asset builder
\______   \__ _\_ |__ |  | |__| ____ _____    ____  	converts an xml file
 |     ___/  |  \ __ \|  | |  |/ ___\\__  \  /    \ 	into the engines .pubb
 |    |   |  |  / \_\ \  |_|  \  \___ / __ \|   |  \	asset file format
 |____|   |____/|___  /____/__|\___  >____  /___|  /
                    \/             \/     \/     \/
*************************************************************************************/

#include <stdio.h>

#include "asset_builder.h"

/*
	Can convert multiple xml files into individual asset files
*/
int main(int argc, char *argv[])
{
	printf("\n**********************************\n");
	printf("- Publican Asset Builder\n- Tom Blicq 2020, Zenburp\n");
	printf("**********************************\n\n");
	printf("%d files passed\n", argc - 1);

	for(int32_t i = 1; i < argc; ++i) {
		struct asset_plan plan = {
			.assetCount = 1,
			.currentType = 0,
			.index = 0,
			.typeCount = ASSET_ID_COUNT
		};

		GetPlanFromFile(argv[i], &plan);
		if(plan.error != ERR_NONE) {
			printf("ERROR: %x, aborting...\n", plan.error);
			break;
		}

		BuildFile(&plan, argv[i]);
	}
}