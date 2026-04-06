// Phase 5: Thread Safety Smoke Tests
// Run with TSan (-fsanitize=thread) for race detection.

#include "TestFixtures.h"
#include <thread>
#include <atomic>

TEST_CASE("SPSC queue works correctly with one producer one consumer", "[thread][queue]") {
    boost::lockfree::spsc_queue<int, boost::lockfree::capacity<100>> queue;

    constexpr int N = 1000;
    std::atomic<int> consumed{0};

    std::thread producer([&] {
        for (int i = 0; i < N; ++i) {
            while (!queue.push(i)) {
                std::this_thread::yield();
            }
        }
    });

    std::thread consumer([&] {
        int expected = 0;
        while (expected < N) {
            int val;
            if (queue.pop(val)) {
                REQUIRE(val == expected);
                ++expected;
                consumed.store(expected);
            } else {
                std::this_thread::yield();
            }
        }
    });

    producer.join();
    consumer.join();

    CHECK(consumed.load() == N);
}

TEST_CASE("Layer queue producer-consumer with shared_ptr", "[thread][queue]") {
    auto ctx = makeTestContext();
    constexpr int N = 20;
    std::atomic<int> consumed{0};

    std::thread producer([&] {
        for (int i = 0; i < N; ++i) {
            auto l = std::make_shared<Layer>(1, 1000);
            while (!ctx->xchange->layerQueue->push(l)) {
                std::this_thread::yield();
            }
        }
    });

    std::thread consumer([&] {
        int count = 0;
        while (count < N) {
            std::shared_ptr<Layer> l;
            if (ctx->xchange->layerQueue->pop(l)) {
                REQUIRE(l != nullptr);
                REQUIRE(l->Buffer != nullptr);
                ++count;
                consumed.store(count);
            } else {
                std::this_thread::yield();
            }
        }
    });

    producer.join();
    consumer.join();

    CHECK(consumed.load() == N);
}

TEST_CASE("Concurrent Track construction and destruction", "[thread][track]") {
    // Verify that multiple Tracks can be created and destroyed
    // from different threads without crashing.
    MinimalProcessorFixture fixture;
    std::atomic<bool> done{false};

    std::thread t1([&] {
        for (int i = 0; i < 5 && !done.load(); ++i) {
            auto track = fixture.makeTrack(0, false);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });

    std::thread t2([&] {
        for (int i = 0; i < 5 && !done.load(); ++i) {
            auto ctx = makeTestContext();
            Loop loop(i);
            if (!ctx->xchange->layerQueue->empty()) {
                loop.AddLayer(true, ctx);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });

    t1.join();
    t2.join();

    SUCCEED("Concurrent construction/destruction completed without crash");
}
