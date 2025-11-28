find_package(OConfigurator QUIET)

if (NOT OConfigurator_FOUND AND NOT TARGET OConfigurator)
	include(FetchContent)

	if(DEFINED BUILD_TESTS)
		set(_orig_BUILD_TESTS "${BUILD_TESTS}")
	else()
		set(_orig_BUILD_TESTS ON)
	endif()

	if(DEFINED BUILD_DOC)
		set(_orig_BUILD_DOC "${BUILD_DOC}")
	else()
		set(_orig_BUILD_DOC ON)
	endif()

	set(BUILD_TESTS OFF CACHE BOOL "Temporarily disabled for FetchContent" FORCE)
	set(BUILD_DOC   OFF CACHE BOOL "Temporarily disabled for FetchContent" FORCE)

	FetchContent_Declare(
		OConfigurator
		GIT_REPOSITORY https://github.com/leocity207/OConfigurator.git
		GIT_TAG V0.0.3
	)
	FetchContent_MakeAvailable(OConfigurator)

	set(BUILD_TESTS "${_orig_BUILD_TESTS}" CACHE BOOL "Restored" FORCE)
	set(BUILD_DOC   "${_orig_BUILD_DOC}"   CACHE BOOL "Restored" FORCE)

	unset(_orig_BUILD_TESTS)
	unset(_orig_BUILD_DOC)

endif()