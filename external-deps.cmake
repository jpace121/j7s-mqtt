include(FetchContent)

FetchContent_Declare(j7s-blinkt
    GIT_REPOSITORY "https://github.com/jpace121/j7s-blinkt.git"
    GIT_TAG "cpp-only"
    GIT_SHALLOW "True"
)

FetchContent_Declare(argparse
  GIT_REPOSITORY "https://github.com/p-ranav/argparse.git"
  GIT_TAG "v2.2"
  GIT_SHALLOW "True"
)

FetchContent_Declare(json
    GIT_REPOSITORY "https://github.com/nlohmann/json.git"
    GIT_TAG "develop"
    GIT_SHALLOW "True"
)

FetchContent_Declare(cpr
    GIT_REPOSITORY "https://github.com/libcpr/cpr.git"
    GIT_TAG "1.8.1"
    GIT_SHALLOW "True"
)

FetchContent_Declare(fmt
    GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
    GIT_TAG "8.1.1"
    GIT_SHALLOW "True"
)

FetchContent_MakeAvailable(j7s-blinkt argparse json cpr fmt)