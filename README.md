# TestDrivenDevelopment

## Implementing a TDD library by using TDD concepts.


## Desired Test Declaration
* What do we want tests to do dor us?
* What should a test look like?
* What information does a test need?
* How can we use C++ to write tests?
* How will the first test be used?

## Test Results
* Reporting a single test result based on exceptions.
* Enhancing the test library to support multiple tests.
* Summarizing the test results to clearly see what failed and what passed.
* Redirecting the test result so the output can gon to any stream.

## Test Results
* How build failures come first and should be seen as part of the process.
* Why you should write only enough code to pass your tests.
* How to enhance a test and get another pass.

## Adding Tests to a Project
* How to detect whether a test passes or fails.
* Enhancing the testing library to support confirmations.
* Should error cases be tested, too?

## Adding More Confirm Types
* Fixing the bool confirms.
* Confirming equality.  
* Changing the code to fix a problem that line numbers are causing with tests failures.
* Adding more confirm types.
* Confirming string literals.
* Confirming floating-point values.
* How to write confirms.

## Test Setup and Teardown
* Supporting test setup ans teardown
* Enhancing test setup ans teardown for multiple tests
* Handling errors in setup and teardown

## What makes a Good Test?
* Be easy to understand - a good understanding will lead to better ideas for more tests and make tests easier to maintain.
* Be focused on a specific scenario - don't try to test everything in one giant test. Doing too much in a test will break the first guidance of understandability.
* Be repeatable - tests that use random behavior to sometimes catch problems can miss issues at the worst times.
* Be kept close to the project - make sure that test belong to the project they are testing.
* Should test what should happen instead of how it happens - if a test relies too much an internal workings, then it will be brittle and cause more work when the code is refactored. 