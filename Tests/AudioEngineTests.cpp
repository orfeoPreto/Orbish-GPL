// Phase 4: Audio Engine Regression Tests

#include "TestFixtures.h"

TEST_CASE("safeLayer returns nullptr for empty or out-of-bounds access", "[engine][track]") {
    MinimalProcessorFixture fixture;
    auto track = fixture.makeTrack(0, true);

    REQUIRE(track->Layers != nullptr);

    CHECK(track->safeLayer(-1) == nullptr);
    CHECK(track->safeLayer(0) == nullptr);
    CHECK(track->safeLayer(100) == nullptr);
    CHECK(track->safeCurrentTop() == -1);
}

TEST_CASE("Loop add and remove layer lifecycle", "[engine][loop]") {
    auto ctx = makeTestContext();

    // Pre-populate the layer queue
    for (int i = 0; i < 3; ++i) {
        auto layer = std::make_shared<Layer>(2, 44100 * 10);
        ctx->xchange->layerQueue->push(layer);
    }

    Loop loop(0);

    loop.AddLayer(true, ctx);
    REQUIRE(loop.Layers->size() == 1);
    CHECK(loop.CurrentTop == 0);

    loop.AddLayer(true, ctx);
    REQUIRE(loop.Layers->size() == 2);
    CHECK(loop.CurrentTop == 1);

    loop.AddLayer(false, ctx);
    REQUIRE(loop.Layers->size() == 3);
    CHECK(loop.CurrentTop == 1); // incrementTop was false

    // Queue is now empty, AddLayer should be a no-op
    loop.AddLayer(true, ctx);
    CHECK(loop.Layers->size() == 3);
}

TEST_CASE("Loop AddLayer is no-op when queue is empty", "[engine][loop]") {
    auto ctx = makeTestContext();
    Loop loop(0);

    // Queue is empty from the start
    loop.AddLayer(true, ctx);
    CHECK(loop.Layers->size() == 0);
    CHECK(loop.CurrentTop == -1);
}

TEST_CASE("Layer constructor allocates buffer with correct dimensions", "[engine][layer]") {
    Layer l(2, 44100 * 10);

    REQUIRE(l.Buffer != nullptr);
    CHECK(l.Buffer->getNumChannels() == 2);
    CHECK(l.Buffer->getNumSamples() == 44100 * 10);
    CHECK(l.Checkpoint == -1);
    CHECK(l.dirty == false);
    CHECK(l.index == 0);
}

TEST_CASE("Layer default constructor creates null buffer", "[engine][layer]") {
    Layer l;
    CHECK(l.Buffer == nullptr);
}

TEST_CASE("Context sample conversion math", "[engine][context]") {
    auto ctx = makeTestContext();

    SECTION("quartersToSamples") {
        double samples = ctx->quartersToSamples(1.0);
        CHECK(samples == Catch::Approx(22050.0).margin(1.0));
    }

    SECTION("samplesToQuarters") {
        double quarters = ctx->samplesToQuarters(22050.0);
        CHECK(quarters == Catch::Approx(1.0).margin(0.01));
    }

    SECTION("beatsToSamples with 4/4 time") {
        double samples = ctx->beatsToSamples(1.0);
        CHECK(samples == Catch::Approx(22050.0).margin(1.0));
    }

    SECTION("differenceFromClosestBeatInSamples") {
        double diff = ctx->differenceFromClosestBeatInSamples(22050);
        CHECK(diff == Catch::Approx(0.0));

        diff = ctx->differenceFromClosestBeatInSamples(22150);
        CHECK(diff == Catch::Approx(100.0));
    }

    SECTION("Edge case: zero BPM returns zero") {
        ctx->bpm = 0;
        CHECK(ctx->quartersToSamples(1.0) == 0.0);
    }

    SECTION("quartersPerBar in 4/4") {
        double qpb = ctx->quartersPerBar();
        CHECK(qpb == Catch::Approx(4.0).margin(0.01));
    }
}

TEST_CASE("Realignment construction and basic state", "[engine][realignment]") {
    Realignment r(512, 44100);

    CHECK(r.getTotalOffset() == 0);
    CHECK(r.isRealigned() == false);
    CHECK(r.isSyncInProgress() == false);
    CHECK(r.isFadeInProgress() == false);
}
