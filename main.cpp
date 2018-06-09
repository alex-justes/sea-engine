#include <iostream>
#include <filesystem>

#include "core/Engine.h"

#include "core/BasicBehaviors.hpp"
#include "core/CollisionDetectors.hpp"
#include "Log.h"
#include "helpers/Configuration.h"

int main(int argc, char **argv)
{
    namespace fs = std::filesystem;
    fs::path config_path = "engine.conf";
    if (argc < 2)
    {
        LOG_W("No configuration specified. Default \"%s\" will be used.", config_path.c_str())
    } else if (argc > 1)
    {
        config_path = fs::path(argv[1]);
    }

    LOG_S("Using \"%s\" for engine configuration.", config_path.c_str())

    if (!fs::exists(config_path))
    {
        LOG_E("Configuration \"%s\" not found.", config_path.c_str())
        LOG_S("Finished.")
        return 1;
    }

    core::Engine engine(config_path);
    if (!engine.initialize_sdl())
    {
        return 1;
    }

    engine.main_loop();

    return 0;
};