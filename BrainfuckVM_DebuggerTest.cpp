#include "gmock/gmock.h"
#include "Brainfuck.h"

using namespace testing;

class BrainfuckVMDebugger : public Test {
public:
    Brainfuck::Interpreter vm;
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
    EXPECT_THAT(vm.GetDataPointer(), Eq(1u));
    EXPECT_THAT(vm.GetInstrPointer(), Eq(2u));
    vm.Step(2);
    EXPECT_THAT(vm.GetMemory().at(1), Eq(1));
    EXPECT_THAT(vm.GetMemory().at(2), Eq(1));
    EXPECT_THAT(vm.GetDataPointer(), Eq(2u));
    ASSERT_THAT(vm.GetInstrPointer(), Eq(4u));
}

TEST_F(BrainfuckVMDebugger, IgnoresStepsAfterProgramEnd) {
    vm.Start("+");
    vm.Step(3);
}

TEST_F(BrainfuckVMDebugger, DontCrashOnEmptyProgram) {
    vm.Step(3);
    EXPECT_THAT(vm.GetInstrPointer(), Eq(0u));
    vm.Start("");
    ASSERT_THAT(vm.GetInstrPointer(), Eq(0u));
    vm.Step(3);
}

TEST_F(BrainfuckVMDebugger, StartingDebuggerResetsInstrPointer) {
    vm.Start(">+");
    vm.Step(2);
    vm.Start("-<");
    ASSERT_THAT(vm.GetInstrPointer(), Eq(0u));
}

TEST_F(BrainfuckVMDebugger, GoUntilGivenInstrPointer) {
    vm.Start("+-<>");
    vm.GoUntil(2);
    EXPECT_THAT(vm.GetInstrPointer(), Eq(2u));
    vm.GoUntil(10);
    ASSERT_THAT(vm.GetInstrPointer(), Eq(4u));
}

TEST_F(BrainfuckVMDebugger, GoUntilGivenInstrPointerFromEndOfProgram) {
    vm.Start("+-<>");
    vm.GoUntil(1, true);
    EXPECT_THAT(vm.GetInstrPointer(), Eq(3u));
    vm.GoUntil(0, true);
    ASSERT_THAT(vm.GetInstrPointer(), Eq(4u));
}
