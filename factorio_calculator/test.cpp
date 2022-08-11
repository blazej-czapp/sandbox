#include <gtest/gtest.h>

#include "factorio_dsl.hpp"

class CoutTest : public ::testing::Test {
public:
    CoutTest() {
        // the DSL implicitly prints the results to stdout, so capture what's being printed
        std::cout.rdbuf(out.rdbuf());
    }
protected:
    std::stringstream out;
};

TEST_F(CoutTest, SingleRawComponent) {
    let iron_ore = basic;
    let iron_plate = iron_ore*1;

    EXPECT_EQ(1, iron_ore in iron_plate);
}

TEST_F(CoutTest, Fabricators) {
    let iron_plate = basic;
    let iron_gear_wheel = iron_plate*2;

    grey_fabricator produces 2 of iron_gear_wheel every 1s;
    grey_fabricator number producing 400 of iron_gear_wheel every 6s;
    EXPECT_EQ("33.33\n", out.str());
}

TEST_F(CoutTest, tFabricatorsFractionalDuration) {
    let iron_ore = basic;
    let iron_plate = iron_ore*1;

    stone_furnace produces 1 of iron_plate every 3.2s;
    stone_furnace number producing 400 of iron_plate every 6s;
    EXPECT_EQ("213.33\n", out.str());
}

TEST_F(CoutTest, CompoundComponents) {
    let iron_plate = basic;
    let copper_plate = basic;
    let copper_wire = copper_plate*0.5;
    let green_circuit = copper_wire*3 + iron_plate*1;

    EXPECT_EQ(1.5, copper_plate in green_circuit);
}

TEST_F(CoutTest, MultiplyCompoundComponents) {
    let iron_plate = basic;
    let copper_plate = basic;
    let copper_wire = copper_plate*0.5;
    let green_circuit = (copper_wire*3 + iron_plate*1)*3;

    EXPECT_EQ(4.5, copper_plate in green_circuit);
}

TEST_F(CoutTest, DivideCompoundComponents) {
    let iron_plate = basic;
    let copper_plate = basic;
    let copper_wire = copper_plate*0.5;
    let green_circuit = (copper_wire*3 + iron_plate*1)/2;

    EXPECT_EQ(0.75, copper_plate in green_circuit);
}
