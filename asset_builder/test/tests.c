#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <cmocka.h>

#include "../../publican/src/assets/assets.h"
#include "../src/asset_builder.h"

static char testFileName[] = "mock.xml";
static char testFile[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<assets>\n"
	"<asset name=\"tile\" assetTypeId=\"0\">\n"
		"<bmp path=\"../assets_bmp/tile1.tga\" size=\"64\" />\n"
		"<string>A tile!</string>\n"
	"</asset>\n"
	"<asset name=\"tile\" assetTypeId=\"1\">\n"
		"<bmp path=\"../assets_bmp/tile2.tga\" size=\"64\" />\n"
	"</asset>\n"
	"<asset name=\"tile\" assetTypeId=\"2\">\n"
		"<bmp path=\"../assets_bmp/tile3.tga\" size=\"64\" />\n"
	"</asset>\n"
	"<asset name=\"tile\" assetTypeId=\"3\">\n"
		"<bmp path=\"../assets_bmp/tile4.tga\" size=\"64\" />\n"
	"</asset>\n"
"</assets>";
static char testFileWrong[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<assets>\n"
	"<bmp path=\"../assets_bmp/tile1.tga\" size=\"64\" />\n"
	"<asset name=\"tile\" assetTypeId=\"0\">\n"
		"<bmp path=\"../assets_bmp/tile1.tga\" size=\"64\" />\n"
	"</asset>\n"
"</assets>";
static char testFileTooBig[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<assets>\n"
	"<asset name=\"tile\" assetTypeId=\"0\">\n"
		"<bmp path=\"../assets_bmp/tile1.tga../assets_bmp/tile1.tga../assets_bmp/tile1.tga\" size=\"64\" />\n"
		"<string>A tile!</string>\n"
	"</asset>\n"
"</assets>";

static void ParseFile_can_get_the_correct_asset_ids(void **state)
{
	FILE *fh;
	fopen_s(&fh, testFileName, "w");
	fwrite(testFile, sizeof(testFile), 1, fh);
	fclose(fh);

	struct asset_plan plan = {
		.assetCount = 1,
		.currentType = 0,
		.index = 0,
		.typeCount = ASSET_ID_COUNT
	};

	GetPlanFromFile(testFileName, &plan);

	assert_int_equal(0, plan.types[0].id);
	assert_int_equal(3, plan.types[3].id);

	remove(testFileName);
}

static void ParseFile_can_get_the_correct_bmp_paths(void **state)
{
	FILE *fh;
	fopen_s(&fh, testFileName, "w");
	fwrite(testFile, sizeof(testFile), 1, fh);
	fclose(fh);

	struct asset_plan plan = {
		.assetCount = 1,
		.currentType = 0,
		.index = 0,
		.typeCount = ASSET_ID_COUNT
	};

	GetPlanFromFile(testFileName, &plan);

	assert_string_equal("../assets_bmp/tile1.tga", plan.sources[1].bmp.fileName);
	assert_string_equal("../assets_bmp/tile4.tga", plan.sources[5].bmp.fileName);

	remove(testFileName);
}

static void ParseFile_can_get_the_correct_asset_count(void **state)
{
	FILE *fh;
	fopen_s(&fh, testFileName, "w");
	fwrite(testFile, sizeof(testFile), 1, fh);
	fclose(fh);

	struct asset_plan plan = {
		.assetCount = 1,
		.currentType = 0,
		.index = 0,
		.typeCount = ASSET_ID_COUNT
	};

	GetPlanFromFile(testFileName, &plan);

	assert_int_equal(1, plan.typeCount);

	remove(testFileName);
}

static void ParseFile_can_get_the_correct_type_count(void **state)
{
	FILE *fh;
	fopen_s(&fh, testFileName, "w");
	fwrite(testFile, sizeof(testFile), 1, fh);
	fclose(fh);

	struct asset_plan plan = {
		.assetCount = 1,
		.currentType = 0,
		.index = 0,
		.typeCount = ASSET_ID_COUNT
	};

	GetPlanFromFile(testFileName, &plan);

	assert_int_equal(6, plan.assetCount);

	remove(testFileName);
}

static void ParseFile_errors_when_bmp_outside_asset_block(void **state)
{
	FILE *fh;
	fopen_s(&fh, testFileName, "w");
	fwrite(testFileWrong, sizeof(testFileWrong), 1, fh);
	fclose(fh);

	struct asset_plan plan = {
		.assetCount = 1,
		.currentType = 0,
		.index = 0,
		.typeCount = ASSET_ID_COUNT
	};

	GetPlanFromFile(testFileName, &plan);

	assert_int_equal(ERR_MALFORMED_XML, plan.error);

	remove(testFileName);
}

static void ParseFile_can_get_the_correct_bmp_sizes(void **state)
{
	FILE *fh;
	fopen_s(&fh, testFileName, "w");
	fwrite(testFile, sizeof(testFile), 1, fh);
	fclose(fh);

	struct asset_plan plan = {
		.assetCount = 1,
		.currentType = 0,
		.index = 0,
		.typeCount = ASSET_ID_COUNT
	};

	GetPlanFromFile(testFileName, &plan);

	assert_int_equal(64, plan.sources[1].bmp.sideInPxels);
	assert_int_equal(64, plan.sources[4].bmp.sideInPxels);

	remove(testFileName);
}

static void ParseFile_errors_when_given_string_too_big(void **state)
{
	FILE *fh;
	fopen_s(&fh, testFileName, "w");
	fwrite(testFileTooBig, sizeof(testFileTooBig), 1, fh);
	fclose(fh);

	struct asset_plan plan = {
		.assetCount = 1,
		.currentType = 0,
		.index = 0,
		.typeCount = ASSET_ID_COUNT
	};

	GetPlanFromFile(testFileName, &plan);

	assert_int_equal(ERR_STRING_EXCEEDS_BUFFER_SIZE, plan.error);

	remove(testFileName);
}

static void ParseFile_can_get_the_content_from_a_string_asset(void **state)
{
	FILE *fh;
	fopen_s(&fh, testFileName, "w");
	fwrite(testFile, sizeof(testFile), 1, fh);
	fclose(fh);

	struct asset_plan plan = {
		.assetCount = 1,
		.currentType = 0,
		.index = 0,
		.typeCount = ASSET_ID_COUNT
	};

	GetPlanFromFile(testFileName, &plan);

	assert_string_equal("A tile!", plan.sources[2].string.text);

	remove(testFileName);
}

static void GetFileName_converts_xml_filename_to_pubb(void **state)
{
	char inName[] = "test.xml";
	int32_t len = strlen(inName) + 1;
	char outName[len];

	GetFileName(inName, outName);

	assert_string_equal("test.pubb", outName);

	remove(testFileName);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(ParseFile_can_get_the_correct_asset_ids),
		cmocka_unit_test(ParseFile_can_get_the_correct_bmp_paths),
		cmocka_unit_test(ParseFile_can_get_the_correct_asset_count),
		cmocka_unit_test(ParseFile_can_get_the_correct_type_count),
		cmocka_unit_test(ParseFile_errors_when_bmp_outside_asset_block),
		cmocka_unit_test(ParseFile_can_get_the_correct_bmp_sizes),
		cmocka_unit_test(ParseFile_errors_when_given_string_too_big),
		cmocka_unit_test(ParseFile_can_get_the_content_from_a_string_asset),
		cmocka_unit_test(GetFileName_converts_xml_filename_to_pubb),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}