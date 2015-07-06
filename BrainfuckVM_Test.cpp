#include "gmock/gmock.h"
#include "Brainfuck.h"

using namespace testing;

class BrainfuckVMImpl : public Test {
public:
    Brainfuck::Interpreter vm;
};

TEST_F(BrainfuckVMImpl, ConstructedWithInstrPointerZero) {
    ASSERT_THAT(vm.GetInstrPointer(), Eq(0));
}

TEST_F(BrainfuckVMImpl, ConstructedWithDataPointerZero) {
    ASSERT_THAT(vm.GetDataPointer(), Eq(0));
}

TEST_F(BrainfuckVMImpl, ConstructedWithMemoryInitializedToZeroes) {
    const size_t verify_length = 20;
    std::vector<char> memory_subset = vm.GetMemory();
    memory_subset.resize(verify_length);
    ASSERT_THAT(memory_subset, Eq(std::vector<char>(verify_length, 0)));
}

TEST_F(BrainfuckVMImpl, IncrementsDataPointerOnCommand) {
    vm.Interpret(">>>");
    ASSERT_THAT(vm.GetDataPointer(), Eq(3));
}

TEST_F(BrainfuckVMImpl, DecrementsDataPointerOnCommand) {
    vm.Interpret(">>><<");
    ASSERT_THAT(vm.GetDataPointer(), Eq(1));
}

TEST_F(BrainfuckVMImpl, IncrementsMemoryOnCommand) {
    vm.Interpret("+>++>+++");
    EXPECT_THAT(vm.GetMemory().at(0), Eq(1));
    EXPECT_THAT(vm.GetMemory().at(1), Eq(2));
    ASSERT_THAT(vm.GetMemory().at(2), Eq(3));
}

TEST_F(BrainfuckVMImpl, DecrementsMemoryOnCommand) {
    vm.Interpret("++-");
    EXPECT_THAT(vm.GetMemory().at(0), Eq(1));
}

TEST_F(BrainfuckVMImpl, AddingTwoValuesWithJumps) {
    vm.Interpret("+++>++<");
    vm.Interpret("[->+<]");
    ASSERT_THAT(vm.GetMemory().at(1), Eq(5));
}

TEST_F(BrainfuckVMImpl, DoesNotResetAfterRunning) {
    vm.Interpret(">+");
    EXPECT_THAT(vm.GetInstrPointer(), Eq(2));
    EXPECT_THAT(vm.GetDataPointer(), Eq(1));
    ASSERT_THAT(vm.GetMemory().at(1), Eq(1));
}

TEST_F(BrainfuckVMImpl, ResetsOnlyInstrPointerWhenStarting) {
    vm.Interpret(">+");
    vm.Interpret(">>>");
    ASSERT_THAT(vm.GetInstrPointer(), Eq(3));
    ASSERT_THAT(vm.GetDataPointer(), Eq(4));
    ASSERT_THAT(vm.GetMemory().at(1), Eq(1));
}

TEST_F(BrainfuckVMImpl, IgnoresNonCommandCharacters) {
    vm.Interpret(">>#Hj>");
    ASSERT_THAT(vm.GetInstrPointer(), Eq(3));
    ASSERT_THAT(vm.GetDataPointer(), Eq(3));
}

TEST_F(BrainfuckVMImpl, DontCrashOnEmptyProgram) {
    vm.Interpret("");
    ASSERT_THAT(vm.GetInstrPointer(), Eq(0));
}

TEST_F(BrainfuckVMImpl, CanResetState) {
    vm.Interpret("+>+");
    vm.Reset();
    EXPECT_THAT(vm.GetInstrPointer(), Eq(0));
    EXPECT_THAT(vm.GetDataPointer(), Eq(0));
    EXPECT_THAT(vm.GetMemory().at(0), Eq(0));
    EXPECT_THAT(vm.GetMemory().at(1), Eq(0));
}
