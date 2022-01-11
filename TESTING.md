# CS50 The Amazing Project - TESTING.md
## Team members: Mack, Sean, Connor and Luca

1.  `designTest.c`:  This .c file tests all of our module methods in the `avatar.c` and `mazeSolver.c` modules.  We test the initialization and deletion of the avatar\_t struct, loadUpStruct\_t, and mazeTile\_t struct.  Further, we test all the helper functions declared, such as leftHandRule(), parseDirection(), and any other method that has complex functionality.  All getter and setter functions were left out of the unit testing due to their obvious functionality.
2.  `graphicstest.c`:  This .c file tests all of the module methods in the `graphics.c` module.  We test the ability for our functions to draw ASCII representations of created mazeTile\_t structures and avatat\_t structures.
3.  `testing.sh`:  This shell script calls the above files as executables and prints their outputs (drawing the output in the case of graphicstest.c).  Further, we test the command line argument validation of AMStartup in testing.sh.

General Notes:
1.  Note that to run these tests you must run `make test` from the command line
2.  See individual test files for individual tests ran
3.  In order to run testing run `make clean | make | make test`
