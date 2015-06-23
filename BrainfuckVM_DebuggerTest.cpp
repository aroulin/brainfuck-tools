#include "gmock/gmock.h"
#include "BrainfuckVM.h"

using namespace testing;

class BrainfuckVMDebugger : public Test {
public:
    BrainfuckVM vm;
};

TEST_F(BrainfuckVMDebugger, CanStepInstructions) {
    vm.Start("+++");
    vm.Step(2);
    ASSERT_THAT(vm.GetMemory().at(0), Eq(2));
}

TEST_F(BrainfuckVMDebugger, KeepStateBetweenSteps) {
    vm.Start(">+>+");
    vm.Step(2);
    EXPECT_THAT(vm.GetMemory().at(1), Eq(1));
    EXPECT_THAT(vm.GetDataPointer(), Eq(1));
    EXPECT_THAT(vm.GetInstrPointer(), Eq(2));
    vm.Step(2);
    EXPECT_THAT(vm.GetMemory().at(1), Eq(1));
    EXPECT_THAT(vm.GetMemory().at(2), Eq(1));
    EXPECT_THAT(vm.GetDataPointer(), Eq(2));
    ASSERT_THAT(vm.GetInstrPointer(), Eq(4));
}

TEST_F(BrainfuckVMDebugger, IgnoresStepsAfterProgramEnd) {
    vm.Start("+");
    vm.Step(3);
}

TEST_F(BrainfuckVMDebugger, DontCrashOnEmptyProgram) {
    vm.Step(3);
    EXPECT_THAT(vm.GetInstrPointer(), Eq(0));
    vm.Start("");
    ASSERT_THAT(vm.GetInstrPointer(), Eq(0));
    vm.Step(3);
}

TEST_F(BrainfuckVMDebugger, StartingDebuggerResetsInstrPointer) {
    vm.Start(">+");
    vm.Step(2);
    vm.Start("-<");
    ASSERT_THAT(vm.GetInstrPointer(), Eq(0));
}

TEST_F(BrainfuckVMDebugger, GoUntilGivenInstrPointer) {
    vm.Start("+-<>");
    vm.GoUntil(2, false);
    EXPECT_THAT(vm.GetInstrPointer(), Eq(2));
    vm.GoUntil(10, false);
    ASSERT_THAT(vm.GetInstrPointer(), Eq(4));
}

TEST_F(BrainfuckVMDebugger, GoUntilGivenInstrPointerFromEndOfProgram) {
    vm.Start("+-<>");
    vm.GoUntil(1, true);
    EXPECT_THAT(vm.GetInstrPointer(), Eq(3));
    vm.GoUntil(0, true);
    ASSERT_THAT(vm.GetInstrPointer(), Eq(4));
}
