// Phase 3: Initialization Regression Tests

#include "TestFixtures.h"

TEST_CASE("Track CurrentPlayingIndex points to valid memory after construction", "[init][track]") {
    MinimalProcessorFixture fixture;
    auto track = fixture.makeTrack(0, true);

    REQUIRE(track->CurrentPlayingIndex != nullptr);
    REQUIRE(track->ActiveLoop != nullptr);
    // After RegisterLoop, CurrentPlayingIndex should point to the Loop's member
    CHECK(track->CurrentPlayingIndex == &(track->ActiveLoop->CurrentPlayingIndex));
    CHECK(*track->CurrentPlayingIndex == 0);
}

TEST_CASE("InternalSynchronizer currentPos valid after Track init", "[init][sync]") {
    MinimalProcessorFixture fixture;
    auto track = fixture.makeTrack(0, true);

    // getNextSynchronizationPoint dereferences currentPos internally.
    // This must not crash (would crash if pointing to dead stack).
    // Returns -1 when Track is neither recording nor playing.
    int result = track->getNextSynchronizationPoint(kSnapNone);
    CHECK(result == -1);
}

TEST_CASE("InternalSynchronizer handles nullptr currentPos gracefully", "[init][sync]") {
    auto ctx = makeTestContext();
    InternalSynchronizer sync(ctx, nullptr);

    // Should return 0 instead of crashing on nullptr dereference
    int result = sync.getNextSynchronizationPoint(kSnapNone);
    CHECK(result == 0);

    result = sync.getNextSynchronizationPoint(kSnapMeasure);
    CHECK(result == 0);
}

TEST_CASE("Track private members are initialized after construction", "[init][track]") {
    MinimalProcessorFixture fixture;
    auto track = fixture.makeTrack(0, true);

    CHECK(track->isMuteArmed() == false);
    CHECK(track->isSoloArmed() == false);
    CHECK(track->isReverseArmed() == false);
    CHECK(track->isRecordingArmed() == false);
    CHECK(track->isPlayArmed() == false);
    CHECK(track->isMonitoring() == true);
    CHECK(track->isAutoTrigger() == false);
    CHECK(track->getSnapMode() == kSnapMeasure);
    CHECK(track->getRecordMode() == kRecLoopOver);

    // Verify isStopArmed doesn't return garbage
    CHECK_NOTHROW(track->isStopArmed());
}

TEST_CASE("Track RegisterLoop wires all pointer members correctly", "[init][track]") {
    MinimalProcessorFixture fixture;
    auto track = fixture.makeTrack(0, true);

    REQUIRE(track->ActiveLoop != nullptr);
    REQUIRE(track->ActiveLoop == track->loops[0]);
    CHECK(track->CurrentPlayingIndex == &(track->ActiveLoop->CurrentPlayingIndex));
    CHECK(track->LoopDuration == &(track->ActiveLoop->LoopDuration));
    CHECK(track->CurrentTop == &(track->ActiveLoop->CurrentTop));
    CHECK(track->Progress == &(track->ActiveLoop->Progress));
    CHECK(track->Layers == track->ActiveLoop->Layers);
}

TEST_CASE("Track AddLoop and RegisterLoop second loop", "[init][track]") {
    MinimalProcessorFixture fixture;
    auto track = fixture.makeTrack(0, true);

    track->AddLoop();
    REQUIRE(track->loops.size() == 2);

    track->RegisterLoop(1);
    CHECK(track->ActiveLoop == track->loops[1]);
    CHECK(track->CurrentPlayingIndex == &(track->loops[1]->CurrentPlayingIndex));
    CHECK(track->LoopDuration == &(track->loops[1]->LoopDuration));
}
