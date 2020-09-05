#include <setjmp.h>
#include <cmocka.h>

#include "win32_file_tests.h"
#include "win32_memory_tests.h"
#include "utils_string_tests.h"
#include "platform_memory_tests.h"
#include "maths_tests.h"

int main(void)
{
	const struct CMUnitTest tests[] = {
		// win32 file io
		cmocka_unit_test(Win32_GetFilesTypeStart_gets_all_files_of_type),
		cmocka_unit_test(Win32_GetFilesTypeEnd_frees_all_created_items),
		cmocka_unit_test(Win32_OpenNextFile_can_open_a_file),
		cmocka_unit_test(Win32_ReadDataFromFile_can_extract_correct_data_from_handle),

		// win32 memory
		cmocka_unit_test(Win32_Alloc_allocates_a_given_amount_of_memory),
		cmocka_unit_test(Win32_Alloc_rounds_up_on_overflow),
		cmocka_unit_test(Win32_Alloc_rounds_down_on_underflow),
		cmocka_unit_test(Win32_Alloc_creates_additional_space_after_generic_block_for_win32_block),

		// utils string
		cmocka_unit_test(IsLineEnd_can_identify_newline),
		cmocka_unit_test(IsLineEnd_can_identify_non_newline),
		cmocka_unit_test(IsWhitespace_can_identify_whitespace),
		cmocka_unit_test(IsWhitespace_can_identify_non_whitespace),
		cmocka_unit_test(StringsAreEqual_can_identify_same_string),
		cmocka_unit_test(StringsAreEqual_can_identify_different_string),

		// platform memory
		cmocka_unit_test(ZERO_STRUCT__sets_all_bytes_in_a_struct_to_zero),
		cmocka_unit_test(ZERO_ARRAY__sets_all_bytes_in_an_array_to_zero),

		cmocka_unit_test(MULT_VEC_can_multiply_a_vector),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}