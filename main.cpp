#include "gmock/gmock.h"
#include "BrainfuckVM.h"

int main(int argc, char **argv) {
/*
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();*/
    BrainfuckVM vm;
    vm.JIT("");
}