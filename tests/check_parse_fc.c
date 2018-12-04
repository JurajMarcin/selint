#include <check.h>
#include <stdio.h>

#include "../src/tree.h"
#include "../src/parse_fc.h"

START_TEST (test_parse_context) {

	char context_str[] = "staff_u:staff_r:foo_t";

	struct sel_context *ctx = parse_context(context_str);

	ck_assert_ptr_nonnull(ctx);
	ck_assert_str_eq("staff_u", ctx->user);
	ck_assert_str_eq("staff_r", ctx->role);
	ck_assert_str_eq("foo_t", ctx->type);
	ck_assert_ptr_null(ctx->range);
	ck_assert_int_eq(0, ctx->has_gen_context);

	free_sel_context(ctx);

}
END_TEST

START_TEST (test_parse_context_missing_field) {

	char context_str[] = "staff_u:foo_t";

	struct sel_context *ctx = parse_context(context_str);

	ck_assert_ptr_null(ctx);

}
END_TEST

START_TEST (test_parse_fc_line_with_gen_context) {
	char line[] = "/usr/bin(/.*)?		gen_context(system_u:object_r:bin_t, s0)";

	struct fc_entry *out= parse_fc_line(line);

	ck_assert_ptr_nonnull(out);
	ck_assert_str_eq("/usr/bin(/.*)?", out->path);
	ck_assert(out->obj == '\0');
	ck_assert_ptr_nonnull(out->context);
	ck_assert_int_eq(1, out->context->has_gen_context);
	ck_assert_str_eq("system_u", out->context->user);
	ck_assert_str_eq("object_r", out->context->role);
	ck_assert_str_eq("bin_t", out->context->type);
	ck_assert_str_eq("s0", out->context->range);

	free_fc_entry(out);

}
END_TEST

START_TEST (test_parse_fc_line) {
	char line[] = "/usr/bin(/.*)?		system_u:object_r:bin_t:s0";

	struct fc_entry *out= parse_fc_line(line);

	ck_assert_ptr_nonnull(out);
	ck_assert_str_eq("/usr/bin(/.*)?", out->path);
	ck_assert(out->obj == '\0');
	ck_assert_ptr_nonnull(out->context);
	ck_assert_int_eq(0, out->context->has_gen_context);
	ck_assert_str_eq("system_u", out->context->user);
	ck_assert_str_eq("object_r", out->context->role);
	ck_assert_str_eq("bin_t", out->context->type);
	ck_assert_str_eq("s0", out->context->range);

	free_fc_entry(out);

}
END_TEST

START_TEST (test_parse_fc_line_with_obj) {
	char line[] = "/usr/bin(/.*)?		-d	system_u:object_r:bin_t:s0";

	struct fc_entry *out= parse_fc_line(line);

	ck_assert_ptr_nonnull(out);
	ck_assert_str_eq("/usr/bin(/.*)?", out->path);
	ck_assert(out->obj == 'd');
	ck_assert_ptr_nonnull(out->context);
	ck_assert_int_eq(0, out->context->has_gen_context);
	ck_assert_str_eq("system_u", out->context->user);
	ck_assert_str_eq("object_r", out->context->role);
	ck_assert_str_eq("bin_t", out->context->type);
	ck_assert_str_eq("s0", out->context->range);

	free_fc_entry(out);
}
END_TEST

Suite *parse_fc_suite(void) {
	Suite *s;
	TCase *tc_core;

	s = suite_create("Parse_fc");

	tc_core = tcase_create("Core");

	tcase_add_test(tc_core, test_parse_context);
	tcase_add_test(tc_core, test_parse_context_missing_field);
	tcase_add_test(tc_core, test_parse_fc_line_with_gen_context);
	tcase_add_test(tc_core, test_parse_fc_line);
	tcase_add_test(tc_core, test_parse_fc_line_with_obj);
	suite_add_tcase(s, tc_core);

	return s;
}

int main(void) {

	int number_failed = 0;
	Suite *s;
	SRunner *sr;

	s = parse_fc_suite();
	sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0)? 0 : -1;
}
