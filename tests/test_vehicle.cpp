#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "../src/core/vehicle.hpp"

using Catch::Approx;

TEST_CASE("Vehicle initialization", "[vehicle]") {
    Vehicle vehicle(0.0f, 0.0f, 0.0f);

    SECTION("Initial position is correct") {
        auto position = vehicle.getPosition();
        REQUIRE(position[0] == 0.0f);
        REQUIRE(position[1] == 0.0f);
        REQUIRE(position[2] == 0.0f);
    }

    SECTION("Initial velocity is zero") {
        REQUIRE(vehicle.getVelocity() == 0.0f);
    }

    SECTION("Initial rotation is zero") {
        REQUIRE(vehicle.getRotation() == 0.0f);
    }
}

TEST_CASE("Vehicle acceleration", "[vehicle]") {
    Vehicle vehicle(0.0f, 0.0f, 0.0f);

    SECTION("Forward acceleration increases velocity") {
        vehicle.accelerateForward();
        vehicle.update(0.1f);
        REQUIRE(vehicle.getVelocity() > 0.0f);
    }

    SECTION("Backward acceleration decreases velocity") {
        vehicle.accelerateBackward();
        vehicle.update(0.1f);
        REQUIRE(vehicle.getVelocity() < 0.0f);
    }

    SECTION("Multiple updates continue acceleration") {
        for (int i = 0; i < 5; i = i + 1) {
            vehicle.accelerateForward();
            vehicle.update(0.1f);
        }
        REQUIRE(vehicle.getVelocity() > 1.0f);
    }
}

TEST_CASE("Vehicle max speed limits", "[vehicle]") {
    Vehicle vehicle(0.0f, 0.0f, 0.0f);

    SECTION("Forward speed is clamped to max speed") {
        // Accelerate for a long time
        for (int i = 0; i < 100; i = i + 1) {
            vehicle.accelerateForward();
            vehicle.update(0.1f);
        }
        REQUIRE(vehicle.getVelocity() <= vehicle.getMaxSpeed());
    }

    SECTION("Backward speed is limited") {
        // Accelerate backward for a long time
        for (int i = 0; i < 100; i = i + 1) {
            vehicle.accelerateBackward();
            vehicle.update(0.1f);
        }
        // Reverse speed should be less than forward max speed
        REQUIRE(vehicle.getVelocity() >= -vehicle.getMaxSpeed());
    }
}

TEST_CASE("Vehicle friction", "[vehicle]") {
    Vehicle vehicle(0.0f, 0.0f, 0.0f);

    SECTION("Vehicle slows down without acceleration") {
        // Accelerate first
        for (int i = 0; i < 10; i = i + 1) {
            vehicle.accelerateForward();
            vehicle.update(0.1f);
        }

        float velocityBefore = vehicle.getVelocity();

        // Let it coast (no acceleration)
        for (int i = 0; i < 10; i = i + 1) {
            vehicle.update(0.1f);
        }

        float velocityAfter = vehicle.getVelocity();
        REQUIRE(velocityAfter < velocityBefore);
    }
}

TEST_CASE("Vehicle turning", "[vehicle]") {
    Vehicle vehicle(0.0f, 0.0f, 0.0f);

    SECTION("Turning changes rotation") {
        float initialRotation = vehicle.getRotation();

        // Accelerate until velocity exceeds MIN_TURN_SPEED
        while (vehicle.getVelocity() < 0.51f) {
            vehicle.accelerateForward();
            vehicle.update(0.1f);
        }

        vehicle.turn(0.1f);

        REQUIRE(vehicle.getRotation() != initialRotation);
    }

    SECTION("Cannot turn when stationary") {
        // Try to turn without velocity
        float initialRotation = vehicle.getRotation();
        vehicle.turn(1.0f);
        REQUIRE(vehicle.getRotation() == Approx(initialRotation));
    }
}

TEST_CASE("Vehicle reset", "[vehicle]") {
    Vehicle vehicle(5.0f, 0.0f, 10.0f);

    // Move and accelerate
    for (int i = 0; i < 10; i = i + 1) {
        vehicle.accelerateForward();
        vehicle.update(0.1f);
    }

    // Reset
    vehicle.reset();

    SECTION("Position returns to initial") {
        auto position = vehicle.getPosition();
        REQUIRE(position[0] == 5.0f);
        REQUIRE(position[1] == 0.0f);
        REQUIRE(position[2] == 10.0f);
    }

    SECTION("Velocity returns to zero") {
        REQUIRE(vehicle.getVelocity() == 0.0f);
    }

    SECTION("Rotation returns to zero") {
        REQUIRE(vehicle.getRotation() == 0.0f);
    }
}

TEST_CASE("Vehicle movement", "[vehicle]") {
    Vehicle vehicle(0.0f, 0.0f, 0.0f);

    SECTION("Forward movement changes position") {
        auto initialPos = vehicle.getPosition();

        for (int i = 0; i < 10; i = i + 1) {
            vehicle.accelerateForward();
            vehicle.update(0.1f);
        }

        auto finalPos = vehicle.getPosition();

        // Position should have changed (exact values depend on rotation)
        bool positionChanged = (initialPos[0] != finalPos[0]) ||
                              (initialPos[2] != finalPos[2]);
        REQUIRE(positionChanged == true);
    }
}
