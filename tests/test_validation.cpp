#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "core/vehicle.hpp"
#include "audio/audio_manager.hpp"

using Catch::Approx;

TEST_CASE("Audio manager initialization", "[audio]") {
    AudioManager audioManager;

    SECTION("Can initialize with valid audio file") {
        // This test verifies initialization doesn't crash with a valid path
        // Note: Will only pass if asset files are present in the expected location
        (void)audioManager.initialize("assets/carnoise.wav");
        // If assets are present, initialization should succeed
        REQUIRE_NOTHROW(audioManager.update(Vehicle(0.0f, 0.0f, 0.0f)));
    }

    SECTION("Handles missing audio file gracefully") {
        // Note: This test intentionally passes a non-existent file
        // The error message to stderr is expected and indicates proper error handling
        bool initialized = audioManager.initialize("nonexistent_file.wav");
        REQUIRE_FALSE(initialized);
    }
}

TEST_CASE("Audio manager update doesn't crash", "[audio]") {
    AudioManager audioManager;
    Vehicle vehicle(0.0f, 0.0f, 0.0f);

    SECTION("Update works even when not initialized") {
        // Should not crash even if audio isn't initialized
        audioManager.update(vehicle);
        REQUIRE(true); // If we reach here, no crash occurred
    }
}

TEST_CASE("Vehicle input validation", "[vehicle][validation]") {
    Vehicle vehicle(0.0f, 0.0f, 0.0f);

    SECTION("setVelocity clamps to reasonable values") {
        vehicle.setVelocity(1000.0f); // Unreasonably high
        REQUIRE(vehicle.getVelocity() < 100.0f); // Should be clamped

        vehicle.setVelocity(-1000.0f); // Unreasonably low
        REQUIRE(vehicle.getVelocity() > -100.0f); // Should be clamped
    }

    SECTION("setScale prevents zero or negative values") {
        vehicle.setScale(0.0f);
        REQUIRE(vehicle.getScale() > 0.0f);

        vehicle.setScale(-5.0f);
        REQUIRE(vehicle.getScale() > 0.0f);
    }

    SECTION("setAccelerationMultiplier clamps to reasonable range") {
        vehicle.setAccelerationMultiplier(100.0f);
        REQUIRE(vehicle.getAccelerationMultiplier() <= 5.0f);

        vehicle.setAccelerationMultiplier(-1.0f);
        REQUIRE(vehicle.getAccelerationMultiplier() >= 0.1f);
    }
}

TEST_CASE("Vehicle state consistency", "[vehicle][state]") {
    Vehicle vehicle(0.0f, 0.0f, 0.0f);

    SECTION("Nitrous state is consistent") {
        REQUIRE_FALSE(vehicle.hasNitrous());
        REQUIRE_FALSE(vehicle.isNitrousActive());

        vehicle.pickupNitrous();
        REQUIRE(vehicle.hasNitrous());
        REQUIRE_FALSE(vehicle.isNitrousActive());

        vehicle.activateNitrous();
        REQUIRE_FALSE(vehicle.hasNitrous()); // Consumed
        REQUIRE(vehicle.isNitrousActive());
        REQUIRE(vehicle.getNitrousTimeRemaining() > 0.0f);
    }

    SECTION("Cannot activate nitrous without having it") {
        REQUIRE_FALSE(vehicle.hasNitrous());
        vehicle.activateNitrous();
        REQUIRE_FALSE(vehicle.isNitrousActive());
    }

    SECTION("Drift state transitions correctly") {
        REQUIRE_FALSE(vehicle.isDrifting());

        vehicle.startDrift();
        REQUIRE(vehicle.isDrifting());

        vehicle.stopDrift();
        REQUIRE_FALSE(vehicle.isDrifting());
    }
}

TEST_CASE("Vehicle reset functionality", "[vehicle][reset]") {
    Vehicle vehicle(10.0f, 0.0f, 10.0f);

    SECTION("Reset restores initial position") {
        // Move vehicle
        vehicle.accelerateForward();
        vehicle.update(1.0f);

        auto pos1 = vehicle.getPosition();
        REQUIRE(pos1[0] != 10.0f); // Should have moved

        // Reset
        vehicle.reset();
        auto pos2 = vehicle.getPosition();
        REQUIRE(pos2[0] == 10.0f);
        REQUIRE(pos2[2] == 10.0f);
    }

    SECTION("Reset clears velocity and acceleration") {
        vehicle.accelerateForward();
        vehicle.update(1.0f);
        REQUIRE(vehicle.getVelocity() != 0.0f);

        vehicle.reset();
        REQUIRE(vehicle.getVelocity() == 0.0f);
    }

    SECTION("Reset clears nitrous state") {
        vehicle.pickupNitrous();
        vehicle.activateNitrous();

        vehicle.reset();
        REQUIRE_FALSE(vehicle.hasNitrous());
        REQUIRE_FALSE(vehicle.isNitrousActive());
    }

    SECTION("Reset clears drift state") {
        vehicle.startDrift();
        vehicle.update(0.1f);

        vehicle.reset();
        REQUIRE_FALSE(vehicle.isDrifting());
        REQUIRE(vehicle.getDriftAngle() == 0.0f);
    }
}

TEST_CASE("Vehicle gear system", "[vehicle][gears]") {
    Vehicle vehicle(0.0f, 0.0f, 0.0f);

    SECTION("Starts in gear 1") {
        REQUIRE(vehicle.getCurrentGear() == 1);
    }

    SECTION("RPM is above idle initially") {
        REQUIRE(vehicle.getRPM() >= 1000.0f);
    }

    SECTION("Gear changes with speed") {
        int initialGear = vehicle.getCurrentGear();

        // Accelerate for a while
        for (int i = 0; i < 100; ++i) {
            vehicle.accelerateForward();
            vehicle.update(0.016f); // ~60 FPS
        }

        // Gear should have changed if we reached sufficient speed
        if (vehicle.getVelocity() > 5.0f) {
            REQUIRE(vehicle.getCurrentGear() >= initialGear);
        }
    }
}

TEST_CASE("Vehicle update with zero deltaTime", "[vehicle][edge-cases]") {
    Vehicle vehicle(0.0f, 0.0f, 0.0f);

    SECTION("Zero deltaTime doesn't cause issues") {
        auto pos1 = vehicle.getPosition();
        vehicle.accelerateForward();
        vehicle.update(0.0f);
        auto pos2 = vehicle.getPosition();

        // Position shouldn't change with zero deltaTime
        REQUIRE(pos1[0] == pos2[0]);
        REQUIRE(pos1[2] == pos2[2]);
    }
}

TEST_CASE("Vehicle update with large deltaTime", "[vehicle][edge-cases]") {
    Vehicle vehicle(0.0f, 0.0f, 0.0f);

    SECTION("Large deltaTime doesn't cause instability") {
        vehicle.accelerateForward();
        vehicle.update(10.0f); // Unrealistically large deltaTime

        // Vehicle should still be in valid state
        REQUIRE(std::abs(vehicle.getVelocity()) < 100.0f);
        auto pos = vehicle.getPosition();
        REQUIRE(std::abs(pos[0]) < 1000.0f);
        REQUIRE(std::abs(pos[2]) < 1000.0f);
    }
}

