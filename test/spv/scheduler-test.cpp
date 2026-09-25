/* © SPIRV-Interpreter @ https://github.com/mmoult/SPIRV-Interpreter
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <catch2/catch_test_macros.hpp>
#include "../../src/spv/scheduler.hpp"

struct TestScheduler : public Scheduler {
    unsigned getPatternSize() const {
        return pattern.size();
    }

    void checkSpecial(unsigned index) const {
        REQUIRE(pattern[pattern.size() - 1 - index].special);
    }

    void checkValue(unsigned index, unsigned value) const {
        const auto& token = pattern[pattern.size() - 1 - index];
        REQUIRE(!token.special);
        CHECK(token.value == value);
    }
};

TEST_CASE("pattern parsing", "[scheduler]") {
    TestScheduler scheduler;

    SECTION("empty") {
        REQUIRE(!scheduler.applyPattern(""));
        CHECK(scheduler.getPatternSize() == 0);
    }

    SECTION("bad syntax") {
        REQUIRE(!scheduler.applyPattern("."));
        CHECK(scheduler.getPatternSize() == 0);
    }

    SECTION("multi-digit invocation") {
        REQUIRE(scheduler.applyPattern("394."));
        unsigned index = 0;

        scheduler.checkValue(index++, 394);
        scheduler.checkSpecial(index++);
        CHECK(scheduler.getPatternSize() == index);
    }

    SECTION("sequence") {
        REQUIRE(scheduler.applyPattern("0.1.2."));
        unsigned index = 0;

        scheduler.checkValue(index++, 0);
        scheduler.checkSpecial(index++);
        scheduler.checkValue(index++, 1);
        scheduler.checkSpecial(index++);
        scheduler.checkValue(index++, 2);
        scheduler.checkSpecial(index++);
        CHECK(scheduler.getPatternSize() == index);
    }

    SECTION("sequence spaced") {
        REQUIRE(scheduler.applyPattern("7. 8 .9 ."));
        unsigned index = 0;

        scheduler.checkValue(index++, 7);
        scheduler.checkSpecial(index++);
        scheduler.checkValue(index++, 8);
        scheduler.checkSpecial(index++);
        scheduler.checkValue(index++, 9);
        scheduler.checkSpecial(index++);
        CHECK(scheduler.getPatternSize() == index);
    }

    SECTION("list") {
        REQUIRE(scheduler.applyPattern("0,1,2"));
        unsigned index = 0;

        scheduler.checkValue(index++, 0);
        scheduler.checkValue(index++, 1);
        scheduler.checkValue(index++, 2);
        CHECK(scheduler.getPatternSize() == 3);
    }

    SECTION("trailing list") {
        REQUIRE(scheduler.applyPattern("0,1,2,"));
        unsigned index = 0;

        scheduler.checkValue(index++, 0);
        scheduler.checkValue(index++, 1);
        scheduler.checkValue(index++, 2);
        CHECK(scheduler.getPatternSize() == index);
    }

    SECTION("range list") {
        REQUIRE(scheduler.applyPattern("0:,2"));
        unsigned index = 0;

        scheduler.checkValue(index++, 0);
        scheduler.checkValue(index++, 1);
        scheduler.checkValue(index++, 2);
        CHECK(scheduler.getPatternSize() == index);
    }

    SECTION("range sequence") {
        REQUIRE(scheduler.applyPattern("0:.2."));
        unsigned index = 0;

        scheduler.checkValue(index++, 0);
        scheduler.checkSpecial(index++);
        scheduler.checkValue(index++, 1);
        scheduler.checkSpecial(index++);
        scheduler.checkValue(index++, 2);
        scheduler.checkSpecial(index++);
        CHECK(scheduler.getPatternSize() == index);
    }

    SECTION("range list sequence") {
        REQUIRE(scheduler.applyPattern("0:,2."));
        unsigned index = 0;

        scheduler.checkValue(index++, 0);
        scheduler.checkValue(index++, 1);
        scheduler.checkValue(index++, 2);
        scheduler.checkSpecial(index++);
        CHECK(scheduler.getPatternSize() == index);
    }

    SECTION("range sequence weak end") {
        REQUIRE(scheduler.applyPattern("0:.2"));
        unsigned index = 0;

        scheduler.checkValue(index++, 0);
        scheduler.checkSpecial(index++);
        scheduler.checkValue(index++, 1);
        scheduler.checkSpecial(index++);
        scheduler.checkValue(index++, 2);
        CHECK(scheduler.getPatternSize() == index);
    }

    SECTION("range list step") {
        REQUIRE(scheduler.applyPattern("0:,6:3"));
        unsigned index = 0;

        scheduler.checkValue(index++, 0);
        scheduler.checkValue(index++, 3);
        scheduler.checkValue(index++, 6);
        CHECK(scheduler.getPatternSize() == index);
    }

    SECTION("range backwards list") {
        REQUIRE(scheduler.applyPattern("8:,3"));
        unsigned index = 0;

        scheduler.checkValue(index++, 8);
        scheduler.checkValue(index++, 7);
        scheduler.checkValue(index++, 6);
        scheduler.checkValue(index++, 5);
        scheduler.checkValue(index++, 4);
        scheduler.checkValue(index++, 3);
        CHECK(scheduler.getPatternSize() == index);
    }

    SECTION("range backwards list step") {
        REQUIRE(scheduler.applyPattern("8:,3:2"));
        unsigned index = 0;

        scheduler.checkValue(index++, 8);
        scheduler.checkValue(index++, 6);
        scheduler.checkValue(index++, 4);
        CHECK(scheduler.getPatternSize() == index);
    }
}
