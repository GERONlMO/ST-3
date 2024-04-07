// Copyright 2024 Novostroev Ivan

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include "TimedDoor.h"

class MockTimerClient : public TimerClient {
public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class MockDoor : public Door {
public:
    MOCK_METHOD(void, lock, (), (override));
    MOCK_METHOD(void, unlock, (), (override));
    MOCK_METHOD(bool, isDoorOpened, (), (override));
};

TEST(TimedDoor, LockUnlock) {
    TimedDoor door(5);
    EXPECT_FALSE(door.isDoorOpened());
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST(TimedDoor, ThrowState) {
    TimedDoor door(5);
    EXPECT_NO_THROW(door.throwState());
    door.unlock();
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST(DoorTimerAdapter, Timeout) {
    TimedDoor door(5);
    DoorTimerAdapter adapter(door);
    EXPECT_NO_THROW(adapter.Timeout());
    door.unlock();
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(TimedDoor, TimeOut) {
    TimedDoor door(5);
    EXPECT_EQ(door.getTimeOut(), 5);
    door.unlock();
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST(TimedDoor, NoTimeOut) {
    TimedDoor door(5);
    EXPECT_EQ(door.getTimeOut(), 5);
    door.lock();
    EXPECT_NO_THROW(door.throwState());
}

TEST(DoorTimerAdapter, NoTimeOut) {
    TimedDoor door(5);
    DoorTimerAdapter adapter(door);
    door.lock();
    EXPECT_NO_THROW(adapter.Timeout());
}

TEST(DoorTimerAdapter, TimeOut) {
    TimedDoor door(5);
    DoorTimerAdapter adapter(door);
    door.unlock();
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(Timer, Register) {
    Timer timer;
    MockTimerClient client;
    EXPECT_CALL(client, Timeout()).Times(1);
    timer.tregister(5, &client);
}

TEST(Timer, NoRegister) {
    Timer timer;
    MockTimerClient client;
    EXPECT_CALL(client, Timeout()).Times(0);
    timer.tregister(0, &client);
}

TEST(Timer, MultipleRegister) {
    Timer timer;
    MockTimerClient client;
    EXPECT_CALL(client, Timeout()).Times(2);
    timer.tregister(5, &client);
    timer.tregister(5, &client);
}
