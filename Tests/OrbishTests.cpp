#define CATCH_CONFIG_RUNNER
#include <catch_amalgamated.hpp>
#include "TestFixtures.h"

int main(int argc, char* argv[]) {
    JUCEInitialiser juceInit;
    int result = Catch::Session().run(argc, argv);
    return result;
}
