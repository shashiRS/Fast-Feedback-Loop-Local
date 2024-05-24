Create Test {#integration_test_create_test}
==============

[TOC]

* @ref integration_test_main
* @ref setup_test_environment
* @ref integration_test_run_contest

# Naming Conventions
 
Test case scripts need to follow a special format.<br>
Name your test files starting in this format: swt_<test_name>.pytest<br>

Each test case is implemented with a python function.<br>
Name your test cases in this format: SWT_<test_case_name><br>

A helper file will be created at the time of new config creation.<br>

# Test Case Template

```python
"""
Copyright 20xx Continental Corporation

:file: swt_sample_test.pytest
:platform: Windows, Linux
:synopsis:
    <fill_details>

:author:
    - Max Musterman (abc1234) <max.musterman@continental-corporation.com>
"""

# standard Python import area


# PTF or custom import area
# NOTE : ALWAYS IMPORT (ptf_expects, ptf_asserts, report and get_parameter) AS BELOW.
#        FOR PROPER DOCUMENTATION AND ERROR REPORTING
from ptf.ptf_utils.decorator.prioritization import priority, Priority
from ptf.verify_utils import ptf_asserts
from ptf.verify_utils import ptf_expects
from report import *
from ptf.ptf_utils.global_params import get_parameter

@priority(Priority.HIGH)  # This test is executed with high priority
def SWT_SAMPLE_TESTv1():
    DETAILS("A sample test case")

    PRECONDITION("Fill in testcase Precondition")

    VERIFIES("Mention requirement satisfied by this test case")

    TESTTAG("Mention test-tag i.e. hil, design-driven, blackbox, integration")

    TESTCASE()

    TESTSTEP("Accessing variable created in global_setup and verifying its value")
    EXPECTED("Value shall be True")
    value = get_parameter("demo_value")
    ptf_asserts.verify(value, , True, "Demo value has wrong value")
```

DETAILS, PRECONDITION, VERIFIES, TESTTAG, TESTSTEP, EXPECTED are reporting functions. The string inside these functions will be used in the HTML and JSON reports. Also it helps CI scripts to document your test case.

The docstring at the top of the file shall be filled with information about the test and the test developer.<br>
The @ref create_test_helper_files provides helper functions and can be imported to be used in the test.<br>
Information about the TESTTAG can be found in the @ref create_test_tags page.<br>
Each TESTSTEP shall be verified. There are multiple functions for this.<br>
They can be found at the @ref create_test_verify_APIs page.<br>
Some further demo tests can be found in repository ConTest/ptf/demo_tests.
