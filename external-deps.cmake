include(FetchContent)

FetchContent_Declare(j7s-blinkt
    GIT_REPOSITORY "https://github.com/jpace121/j7s-blinkt.git"
    GIT_TAG "cpp-only"
    GIT_SHALLOW "True"
)
FetchContent_MakeAvailable(j7s-blinkt)

FetchContent_Declare(argparse
  GIT_REPOSITORY "https://github.com/p-ranav/argparse.git"
  GIT_TAG "v2.2"
  GIT_SHALLOW "True"
)
FetchContent_MakeAvailable(argparse)

FetchContent_Declare(json
    GIT_REPOSITORY "https://github.com/nlohmann/json.git"
    GIT_TAG "develop"
    GIT_SHALLOW "True"
)
FetchContent_MakeAvailable(json)

FetchContent_Declare(json
    GIT_REPOSITORY "https://github.com/nlohmann/json.git"
    GIT_TAG "develop"
    GIT_SHALLOW "True"
)
FetchContent_MakeAvailable(json)

FetchContent_MakeAvailable(j7s-blinkt argparse json)