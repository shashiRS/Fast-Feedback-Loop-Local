Verify API {#create_test_verify_APIs}
==============

[TOC]

<-- @ref integration_test_create_test
 
In ConTest there are two different types to verify the data:

# PTF_ASSERTS

If the assert fails, the test will stop execution immediately and does not finish.

* [ptf_asserts.fail](https://cip-docs.cmo.conti.de/static/docfiles/ConTest/v2.3.0/verify_doc/verify_utils.html#verify_utils.ptf_asserts.fail)
* [ptf_asserts.print_exception_info](https://cip-docs.cmo.conti.de/static/docfiles/ConTest/v2.3.0/verify_doc/verify_utils.html#verify_utils.ptf_asserts.print_exception_info)
* [ptf_asserts.verify](https://cip-docs.cmo.conti.de/static/docfiles/ConTest/v2.3.0/verify_doc/verify_utils.html#verify_utils.ptf_asserts.verify)
* [ptf_asserts.verify_gt](https://cip-docs.cmo.conti.de/static/docfiles/ConTest/v2.3.0/verify_doc/verify_utils.html#verify_utils.ptf_asserts.verify_gt)
* [ptf_asserts.verify_gt_eq](https://cip-docs.cmo.conti.de/static/docfiles/ConTest/v2.3.0/verify_doc/verify_utils.html#verify_utils.ptf_asserts.verify_gt_eq)
* [ptf_asserts.verify_in_list](https://cip-docs.cmo.conti.de/static/docfiles/ConTest/v2.3.0/verify_doc/verify_utils.html#verify_utils.ptf_asserts.verify_in_list)
* [ptf_asserts.verify_in_range](https://cip-docs.cmo.conti.de/static/docfiles/ConTest/v2.3.0/verify_doc/verify_utils.html#verify_utils.ptf_asserts.verify_in_range)
* [ptf_asserts.verify_l](https://cip-docs.cmo.conti.de/static/docfiles/ConTest/v2.3.0/verify_doc/verify_utils.html#verify_utils.ptf_asserts.verify_l)
* [ptf_asserts.verify_lt_eq](https://cip-docs.cmo.conti.de/static/docfiles/ConTest/v2.3.0/verify_doc/verify_utils.html#verify_utils.ptf_asserts.verify_lt_eq)
* [ptf_asserts.verify_neq](https://cip-docs.cmo.conti.de/static/docfiles/ConTest/v2.3.0/verify_doc/verify_utils.html#verify_utils.ptf_asserts.verify_neq)
* [ptf_asserts.verify_starts_with](https://cip-docs.cmo.conti.de/static/docfiles/ConTest/v2.3.0/verify_doc/verify_utils.html#verify_utils.ptf_asserts.verify_starts_with)
* [ptf_asserts.verify_with_tol](https://cip-docs.cmo.conti.de/static/docfiles/ConTest/v2.3.0/verify_doc/verify_utils.html#verify_utils.ptf_asserts.verify_with_tol)

# PTF_EXPECTS

If the expects fails, the test will be shown as failed, but execution is continued.

* [ptf_expects.expect_eq](https://cip-docs.cmo.conti.de/static/docfiles/ConTest/v2.3.0/verify_doc/verify_utils.html#verify_utils.ptf_expects.expect_eq)
* [ptf_expects.expect_gt](https://cip-docs.cmo.conti.de/static/docfiles/ConTest/v2.3.0/verify_doc/verify_utils.html#verify_utils.ptf_expects.expect_gt)
* [ptf_expects.expect_gt_eq](https://cip-docs.cmo.conti.de/static/docfiles/ConTest/v2.3.0/verify_doc/verify_utils.html#verify_utils.ptf_expects.expect_gt_eq)
* [ptf_expects.expect_in_list](https://cip-docs.cmo.conti.de/static/docfiles/ConTest/v2.3.0/verify_doc/verify_utils.html#verify_utils.ptf_expects.expect_in_list)
* [ptf_expects.expect_in_range](https://cip-docs.cmo.conti.de/static/docfiles/ConTest/v2.3.0/verify_doc/verify_utils.html#verify_utils.ptf_expects.expect_in_range)
* [ptf_expects.expect_lt](https://cip-docs.cmo.conti.de/static/docfiles/ConTest/v2.3.0/verify_doc/verify_utils.html#verify_utils.ptf_expects.expect_lt)
* [ptf_expects.expect_lt_eq](https://cip-docs.cmo.conti.de/static/docfiles/ConTest/v2.3.0/verify_doc/verify_utils.html#verify_utils.ptf_expects.expect_lt_eq)
* [ptf_expects.expect_neq](https://cip-docs.cmo.conti.de/static/docfiles/ConTest/v2.3.0/verify_doc/verify_utils.html#verify_utils.ptf_expects.expect_neq)
* [ptf_expects.expect_starts_with](https://cip-docs.cmo.conti.de/static/docfiles/ConTest/v2.3.0/verify_doc/verify_utils.html#verify_utils.ptf_expects.starts_with)
* [ptf_expects.expect_with_tol](https://cip-docs.cmo.conti.de/static/docfiles/ConTest/v2.3.0/verify_doc/verify_utils.html#verify_utils.ptf_expects.expect_with_tol)
* [ptf_expects.fail](https://cip-docs.cmo.conti.de/static/docfiles/ConTest/v2.3.0/verify_doc/verify_utils.html#verify_utils.ptf_expects.fail)
* [ptf_expects.print_expectations](https://cip-docs.cmo.conti.de/static/docfiles/ConTest/v2.3.0/verify_doc/verify_utils.html#verify_utils.ptf_expects.print_expectations)
