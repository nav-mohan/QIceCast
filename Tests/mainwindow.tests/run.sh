rm -rf ./build;\
cmake -S . -B ./build;\
make -C ./build; \
./build/test_mainwindow;
