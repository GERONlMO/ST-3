// Copyright 2024 Novostroev Ivan

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <chrono> // NOLINT [build/c++11]
#include <thread> // NOLINT [build/c++11]

#include "TimedDoor.h"

class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
    TimedDoor door;
    MockTimerClient mockClient;
    Timer timer;

    TimedDoorTest() : door(1), timer() {}

    void SetUp() override {
        timer.tregister(door.getTimeOut(), &mockClient);
    }

    void TearDown() override {
        testing::Mock::VerifyAndClearExpectations(&mockClient);
    }
};

TEST_F(TimedDoorTest, UNLOCK_DOOR) {
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, LOCK_DOOR) {
    door.unlock();
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, INIT_LOCK_DOOR) {
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, UNLOCK_OPENED_DOOR_AGAIN) {
    door.unlock();
    EXPECT_THROW(door.unlock(), std::logic_error);
}

TEST_F(TimedDoorTest, LOCK_CLOSED_DOOR_AGAIN) {
    EXPECT_THROW(door.lock(), std::logic_error);
}

TEST_F(TimedDoorTest, CLOSED_BEFORE_TIMEOUT_NO_THROW) {
    EXPECT_NO_THROW(door.throwState());
}

TEST_F(TimedDoorTest, OPENED_EXCEPTION_THROW) {
    door.unlock();
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, LOCK_DOOR_THEN_UNLOCKED_AFTER_TIMEOUT_NO_EXCEPTION) {
    std::this_thread::sleep_for(std::chrono::seconds(door.getTimeOut() + 1));
    EXPECT_NO_THROW(door.throwState());
}

TEST_F(TimedDoorTest, LOCK_BEFORE_TIMEOUT_THEN_OPENED_AFTER_TIMEOUT_EX_THROW) {
    std::this_thread::sleep_for(std::chrono::seconds(door.getTimeOut() + 1));
    door.unlock();
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, UNLOCK_DOOR_THEN_TIMEOUT_EXCEPTION_THROWN) {
    door.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(door.getTimeOut()));
    EXPECT_THROW(door.throwState(), std::runtime_error);
}
