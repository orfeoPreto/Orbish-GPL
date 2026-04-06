// Phase 2: Leak/Cleanup Regression Tests
// Run with ASAN enabled (-fsanitize=address) for full leak detection.

#include "TestFixtures.h"

TEST_CASE("DataExchange destructor frees all queue allocations", "[leak][dataexchange]") {
    {
        DataExchange dx;
        REQUIRE(dx.writeVisualisationBufferQueue != nullptr);
        REQUIRE(dx.readVisualisationBufferQueue != nullptr);
        REQUIRE(dx.logWriteMessageQueue != nullptr);
        REQUIRE(dx.logReadMessageQueue != nullptr);
        REQUIRE(dx.readBufferQueue != nullptr);
        REQUIRE(dx.writeGainModifierQueue != nullptr);
        REQUIRE(dx.readGainModifierQueue != nullptr);
        REQUIRE(dx.writeMeasureBufferQueue != nullptr);
        REQUIRE(dx.readMeasureBufferQueue != nullptr);
        REQUIRE(dx.layerQueue != nullptr);
    }
    SUCCEED("DataExchange destroyed without leak");
}

TEST_CASE("Track destructor frees Realignment allocation", "[leak][track]") {
    MinimalProcessorFixture fixture;
    {
        auto track = fixture.makeTrack(0, true);
        REQUIRE(track->realignment != nullptr);
    }
    SUCCEED("Track destroyed, Realignment freed");
}

TEST_CASE("OrbishContext destructor frees DataExchange", "[leak][context]") {
    {
        auto ctx = makeTestContext();
        REQUIRE(ctx->xchange != nullptr);
    }
    SUCCEED("Context destroyed, DataExchange freed");
}

TEST_CASE("Log queue strings are freed even if queue is not fully drained", "[leak][context]") {
    {
        auto ctx = makeTestContext();
        for (int i = 0; i < 5; ++i) {
            auto* s = new std::string("test message " + std::to_string(i));
            s->reserve(200);
            ctx->xchange->logWriteMessageQueue->push(s);
        }
        for (int i = 0; i < 3; ++i) {
            auto* s = new std::string("read message " + std::to_string(i));
            ctx->xchange->logReadMessageQueue->push(s);
        }
    }
    SUCCEED("Context destroyed with undrained log queues, no leak");
}

TEST_CASE("Track ValueTree state is freed on destruction", "[leak][track]") {
    MinimalProcessorFixture fixture;
    {
        auto track = fixture.makeTrack(0, false);
        REQUIRE(track->state != nullptr);
    }
    SUCCEED("Track destroyed, ValueTree state deleted");
}

TEST_CASE("TrackGroup pointers are properly freed", "[leak][groups]") {
    {
        std::vector<TrackGroup*> groups;
        for (int i = 0; i < 10; ++i) {
            groups.push_back(new TrackGroup(i));
        }
        for (auto* g : groups) delete g;
        groups.clear();
    }
    SUCCEED("TrackGroup pointers freed without leak");
}
