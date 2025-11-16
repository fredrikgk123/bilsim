#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "core/game_object.hpp"
#include "core/vehicle.hpp"
#include "core/powerup.hpp"

using Catch::Approx;

TEST_CASE("Basic AABB collision detection", "[collision]") {
    Vehicle vehicle(0.0f, 0.0f, 0.0f);
    Powerup powerup(0.0f, 0.0f, 0.0f, PowerupType::NITROUS);

    SECTION("Objects at same position collide") {
        REQUIRE(vehicle.intersects(powerup));
        REQUIRE(powerup.intersects(vehicle));
    }

    SECTION("Objects far apart do not collide") {
        Powerup farPowerup(100.0f, 0.0f, 100.0f, PowerupType::NITROUS);
        REQUIRE_FALSE(vehicle.intersects(farPowerup));
        REQUIRE_FALSE(farPowerup.intersects(vehicle));
    }
}

TEST_CASE("Collision with inactive objects", "[collision]") {
    Vehicle vehicle(0.0f, 0.0f, 0.0f);
    Powerup powerup(0.0f, 0.0f, 0.0f, PowerupType::NITROUS);

    SECTION("Inactive powerup still reports collision geometrically") {
        powerup.setActive(false);
        // Collision detection is geometric - active state is handled by game logic
        REQUIRE(vehicle.intersects(powerup));
    }
}

TEST_CASE("Collision after movement", "[collision]") {
    Vehicle vehicle(0.0f, 0.0f, 0.0f);
    Powerup powerup(5.0f, 0.0f, 0.0f, PowerupType::NITROUS);

    SECTION("No collision initially") {
        REQUIRE_FALSE(vehicle.intersects(powerup));
    }

    SECTION("Collision after moving toward powerup") {
        vehicle.setPosition(5.0f, 0.0f, 0.0f);
        REQUIRE(vehicle.intersects(powerup));
    }

    SECTION("No collision after moving away") {
        vehicle.setPosition(5.0f, 0.0f, 0.0f);
        REQUIRE(vehicle.intersects(powerup));

        vehicle.setPosition(-10.0f, 0.0f, 0.0f);
        REQUIRE_FALSE(vehicle.intersects(powerup));
    }
}

TEST_CASE("Multiple object collision scenarios", "[collision]") {
    Vehicle vehicle(0.0f, 0.0f, 0.0f);

    SECTION("Vehicle collides with multiple nearby powerups") {
        Powerup powerup1(0.5f, 0.0f, 0.5f, PowerupType::NITROUS);
        Powerup powerup2(-0.5f, 0.0f, -0.5f, PowerupType::NITROUS);

        REQUIRE(vehicle.intersects(powerup1));
        REQUIRE(vehicle.intersects(powerup2));
    }

    SECTION("Vehicle doesn't collide with distant powerups") {
        Powerup powerup1(50.0f, 0.0f, 50.0f, PowerupType::NITROUS);
        Powerup powerup2(-50.0f, 0.0f, -50.0f, PowerupType::NITROUS);

        REQUIRE_FALSE(vehicle.intersects(powerup1));
        REQUIRE_FALSE(vehicle.intersects(powerup2));
    }
}

