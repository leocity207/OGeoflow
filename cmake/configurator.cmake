include(FetchContent)

set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_DOC OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    OConfigurator
    GIT_REPOSITORY https://github.com/leocity207/OConfigurator.git
    GIT_TAG V0.0.2
)

FetchContent_MakeAvailable(OConfigurator)

set(BUILD_TESTS ON CACHE BOOL "" FORCE)
set(BUILD_DOC ON CACHE BOOL "" FORCE)