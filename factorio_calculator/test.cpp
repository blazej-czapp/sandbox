#include <gtest/gtest.h>

#include "factorio_dsl.hpp"

TEST(DSL, CountSingleRawComponent) {
    let iron_ore = raw;
    let iron_plate = iron_ore*1;

    EXPECT_EQ(1, iron_plate/iron_ore);
}

TEST(DSL, CountSingleFabricator) {
    let iron_ore = raw;
    let iron_plate = iron_ore*1;

    stone_furnace produces 1 of iron_plate every 3_s; //TODO 3.2s actually!
    EXPECT_EQ(200, calculate stone_furnace producing 400 of iron_plate every 6_s);
}