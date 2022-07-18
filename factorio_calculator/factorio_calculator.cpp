#include "factorio_dsl.hpp"

int main() {

//     let copper_ore = raw;
//     let copper_plate = copper_ore;
//     let iron_ore = raw;
//     let iron_plate = iron_ore;
//     let iron_stick = iron_plate*0.5;
//     let copper_wire = copper_plate*0.5;
//     let green_circuit = copper_wire + iron_plate*2;
//     let stone = raw;
//     let steel_plate = raw; // we'll never smelt steel directly, so just assume it comes from the bus

// //    let rail = (stone + steel_plate + iron_stick)*0.5;
// //    let red_circuit = raw;
// //    let green_circuit = raw;
// //    let stone_brick = stone;
// //    let electric_furnace = steel_plate*10 + red_circuit*5 + stone_brick*10;
// //    let productivity_module = green_circuit*5 + red_circuit*5;
// //
// //    let purple_science = (rail*30 + electric_furnace + productivity_module)/3/(21/1.25)*6;
// //    let low_density_structure = copper_plate*20 + plastic*5 + steel_plate*2;
// //

//     // --------------------------------------
//     // dedicated oil processing for the rocket - no light oil cracking, will need a separate gas production
//     // on top of this for plastic, but the ratios will be different :/
//     // 100 oil -> 63.75 light oil + 55 gas
//     let crude_oil = raw;
//     let light_oil = crude_oil*(100/63.75);
//     let petroleum_gas = crude_oil*(100/55);
//     // --------------------------------------

//     let coal = raw;

//     let plastic_bar = (coal + petroleum_gas*20)/2;
//     let red_circuit = copper_wire*4 + green_circuit*2 + plastic_bar*2;
//     let water = raw;
//     let sulfur = (petroleum_gas*30 + water*30)/2;
//     let sulfuric_acid = (iron_plate + sulfur*5 + water*100)/50;
//     let blue_circuit = red_circuit*2 + green_circuit*20 + sulfuric_acid*5; // 20s

//     let solid_fuel = light_oil*10;
//     let rocket_fuel = light_oil*10 + solid_fuel*10;
//     let low_density_structure = copper_plate*20 + plastic_bar*5 + steel_plate*2;// in 20s;
//     let speed_module_1 = red_circuit*5 + green_circuit*5;
//     let rocket_control_unit = blue_circuit + speed_module_1;
//     let rocket_part = low_density_structure*10 + rocket_fuel*10 + rocket_control_unit*10;
//     let rocket = rocket_part*100;

//     green_fabricator produces 2 of green_circuit every 2_s;

//     calculate green_fabricator producing 4 of green_circuit every 4_s;
//     //green_fabricator satisfying 4 green_circuit every 4s; // 1

//     let target = rocket; // 5 rockets per hour consumes 5000 blue circuits and 35000 red circuits
//     std::cout << (rocket/iron_plate)/3600 << std::endl;
//     std::cout << target/(copper_plate)/3600 << std::endl;
//     std::cout << target/(blue_circuit)/3600 << std::endl;
//     std::cout << target/(red_circuit)/3600 << std::endl;
//     std::cout << target/(petroleum_gas)/3600 << std::endl;
//     std::cout << target/(crude_oil) << std::endl;

        let LDS = raw*1;
        green_fabricator produces 1 of LDS every 16_s;
        std::cout << "LDS: " << calculate green_fabricator producing 1000 of LDS every 1000_s << std::endl;

        let RCU = raw*1;
        green_fabricator produces 1 of RCU every 24_s;
        std::cout << "RCU: " << calculate green_fabricator producing 1000 of RCU every 1000_s << std::endl;

        let copper_ore = raw;
        let iron_ore = raw;
        let copper_plate = copper_ore*1;
        let iron_plate = iron_ore*1;
        let copper_wire = copper_plate*0.5;

        let green_circuit = copper_wire*3 + iron_plate*1;

        green_fabricator produces 2 of copper_wire every 0.4_s;
        green_fabricator produces 1 of green_circuit every 0.4_s;

        std::cout << "fabs for green chips: " << calculate green_fabricator producing 15 of green_circuit every 1_s << std::endl;
}
