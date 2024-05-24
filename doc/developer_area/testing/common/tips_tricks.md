Tips and tricks for testing {#testing_tips_and_tricks}
===

# Affecting shuffling of the gtest test cases

By default bricks will tell gtest to shuffle the test functions, executing them every run in a different order. For this bricks sets the variable `GTEST_SHUFFLE` to 1 and the variable `GTEST_RANDOM_SEED` to a random value. The default test execution timeout is set to 300 seconds.

To change this, a file **CTestCustom.cmake** has to be created in the root directory of the CMake project (in most cases the root directory of the repository). The file is installed by bricks into the tests folder. The content might look like following example:
```
# set the default test timeout to 30 seconds for all tests
# (yes, this time it is no environment variable)
set(CTEST_TEST_TIMEOUT 60)

# disable the shuffling of the test functions within one test application
set(ENV{GTEST_SHUFFLE} 0)

# setting the seed for to fixed value
set(ENV{GTEST_RANDOM_SEED} 123456)
```
If the `GETST_SHUFFLE` is set to 0 the value of `GTEST_RANDOM_SEED` does not have any effect, since the tests are no longer shuffled. Setting `GTEST_RANDOM_SEED` can help if a certain order of exection shall be run again. For example if a test on Jenkins failed and it shall be reproduced by using the same order.
