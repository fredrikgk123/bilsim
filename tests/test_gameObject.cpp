#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "core/vehicle.hpp"
#include "core/powerup.hpp"

using Catch::Approx;

TEST_CASE("GameObject initialization", "[gameobject]") {
    Vehicle vehicle(10.0f, 5.0f, -3.0f);

    SECTION("Position is set correctly") {
        auto position = vehicle.getPosition();
        REQUIRE(position[0] == 10.0f);
        REQUIRE(position[1] == 5.0f);
        REQUIRE(position[2] == -3.0f);
    }

    SECTION("Object is active by default") {
        REQUIRE(vehicle.isActive());
    }

    SECTION("Size is set correctly") {
        auto size = vehicle.getSize();
        REQUIRE(size[0] > 0.0f); // Width
        REQUIRE(size[1] > 0.0f); // Height
        REQUIRE(size[2] > 0.0f); // Length
    }
}

TEST_CASE("GameObject position modification", "[gameobject]") {
    Powerup powerup(0.0f, 0.0f, 0.0f, PowerupType::NITROUS);

    SECTION("setPosition updates position correctly") {
        powerup.setPosition(15.0f, 2.0f, -8.0f);
        auto position = powerup.getPosition();

        REQUIRE(position[0] == 15.0f);
        REQUIRE(position[1] == 2.0f);
        REQUIRE(position[2] == -8.0f);
    }

    SECTION("Multiple position changes work correctly") {
        powerup.setPosition(1.0f, 1.0f, 1.0f);
        powerup.setPosition(2.0f, 2.0f, 2.0f);
        powerup.setPosition(3.0f, 3.0f, 3.0f);

        auto position = powerup.getPosition();
        REQUIRE(position[0] == 3.0f);
        REQUIRE(position[1] == 3.0f);
        REQUIRE(position[2] == 3.0f);
    }
}

TEST_CASE("GameObject rotation", "[gameobject]") {
    Vehicle vehicle(0.0f, 0.0f, 0.0f);

    SECTION("Initial rotation is set") {
        float rotation = vehicle.getRotation();
        REQUIRE(rotation == Approx(3.14159f).epsilon(0.01)); // π radians (180°)
    }

    SECTION("setRotation changes rotation") {
        vehicle.setRotation(1.5f);
        REQUIRE(vehicle.getRotation() == Approx(1.5f).epsilon(0.001));
    }
}

TEST_CASE("GameObject active state", "[gameobject]") {
    Powerup powerup(0.0f, 0.0f, 0.0f, PowerupType::NITROUS);

    SECTION("Can deactivate object") {
        powerup.setActive(false);
        REQUIRE_FALSE(powerup.isActive());
    }

    SECTION("Can reactivate object") {
        powerup.setActive(false);
        powerup.setActive(true);
        REQUIRE(powerup.isActive());
    }

    SECTION("Active state toggles correctly") {
        bool initialState = powerup.isActive();
        powerup.setActive(!initialState);
        REQUIRE(powerup.isActive() == !initialState);
    }
}

TEST_CASE("GameObject reset functionality", "[gameobject]") {
    SECTION("Vehicle reset returns to initial position") {
        Vehicle vehicle(5.0f, 0.0f, 10.0f);
        auto initialPos = vehicle.getPosition();

        // Move the vehicle
        vehicle.setPosition(50.0f, 0.0f, 50.0f);
        REQUIRE(vehicle.getPosition()[0] == 50.0f);

        // Reset should restore initial position
        vehicle.reset();
        auto resetPos = vehicle.getPosition();
        REQUIRE(resetPos[0] == Approx(initialPos[0]).epsilon(0.01));
        REQUIRE(resetPos[1] == Approx(initialPos[1]).epsilon(0.01));
        REQUIRE(resetPos[2] == Approx(initialPos[2]).epsilon(0.01));
    }

    SECTION("Powerup reset restores active state") {
        Powerup powerup(0.0f, 0.0f, 0.0f, PowerupType::NITROUS);

        // Deactivate powerup (simulating collection)
        powerup.setActive(false);
        REQUIRE_FALSE(powerup.isActive());

        // Reset should reactivate
        powerup.reset();
        REQUIRE(powerup.isActive());
    }

    SECTION("Vehicle reset clears velocity") {
        Vehicle vehicle(0.0f, 0.0f, 0.0f);

        // Accelerate the vehicle
        for (int i = 0; i < 10; i++) {
            vehicle.accelerateForward();
            vehicle.update(0.1f);
        }
        REQUIRE(vehicle.getVelocity() > 0.0f);

        // Reset should clear velocity
        vehicle.reset();
        REQUIRE(vehicle.getVelocity() == 0.0f);
    }
}

TEST_CASE("GameObject size properties", "[gameobject]") {
    Vehicle vehicle(0.0f, 0.0f, 0.0f);
    Powerup powerup(0.0f, 0.0f, 0.0f, PowerupType::NITROUS);

    SECTION("Vehicle has larger size than powerup") {
        auto vehicleSize = vehicle.getSize();
        auto powerupSize = powerup.getSize();

        // Vehicle should be bigger in at least one dimension
        bool isLarger = (vehicleSize[0] > powerupSize[0]) ||
                       (vehicleSize[1] > powerupSize[1]) ||
                       (vehicleSize[2] > powerupSize[2]);
        REQUIRE(isLarger);
    }

    SECTION("Size is consistent across calls") {
        auto size1 = vehicle.getSize();
        auto size2 = vehicle.getSize();

        REQUIRE(size1[0] == size2[0]);
        REQUIRE(size1[1] == size2[1]);
        REQUIRE(size1[2] == size2[2]);
    }
}

