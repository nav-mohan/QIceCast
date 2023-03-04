# Testing Approach - Using `QTest`

## UI Tests
* I've written tests for verifying each individual UI component separately _eg_: `test_mainwindow.h` and `test_mountpotinsettingdialog.h` test the UI of the `MainWindow` class and `MountpointSettingDialog` class. 
* They test whether the window is initialized with the expected values. 
* They test whether button clicks popup windows, whether input boxes update the corresponding _method_ variables of the class, and whether there are  guard-rails to protect against undefined user-inputs.

## InputBuffer class
* I've written tests for testing the behaviour of `InputBuffer` class. 
* Tests are run in a `single-thread` and `multi-thread` mode. 
* `single-thread` mode tests whether the _method_ variable `m_buffer` updates as expected when data is written/read synchronously. 
* `multi-thread` mode tests repeat the same tests as `single-thread` tests but with a `procuder` and `consumer` thread. It mainly tests that the `QReadWriteLock` has been implemented correctly to prevent race conditions. 