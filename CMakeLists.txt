#Open Ephys GUI main build file
cmake_minimum_required(VERSION 3.15)

set(GUI_VERSION 1.0.0)

string(REGEX MATCHALL "[0-9]+" VERSION_LIST ${GUI_VERSION})
list(LENGTH VERSION_LIST num_version_components)

if(num_version_components LESS 3)
	message(FATAL_ERROR "Version number must follow semantic versioning format: MAJOR.MINOR.PATCH[-PRERELEASE]")
endif()

list(GET VERSION_LIST 0 version_major)
list(GET VERSION_LIST 1 version_minor)
list(GET VERSION_LIST 2 version_patch)


math(EXPR hex "(${version_major} << 16) + (${version_minor} << 8) + ${version_patch}" OUTPUT_FORMAT HEXADECIMAL)
set(GUI_VERSION_HEX "${hex}")

list(JOIN VERSION_LIST "," GUI_VERSION_COMMA)

set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version" FORCE)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "Build architecture for Mac OS X" FORCE)
set(CMAKE_OSX_SYSROOT "")
if ((NOT ${CMAKE_CURRENT_BINARY_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR}/Build ) AND (NOT OE_DONT_CHECK_BUILD_PATH))
message(FATAL_ERROR "Please run cmake inside the Build folder to build the binaries in the predefined paths.\nSet the OE_DONT_CHECK_BUILD_PATH cmake variable to TRUE to disable this error")
endif()

project(open-ephys-GUI)
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set(LINUX 1)
	if(NOT CMAKE_BUILD_TYPE)
		set(CMAKE_BUILD_TYPE Debug)
	endif()
endif()
include(HelperFunctions.cmake)

set(BASE_BUILD_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Build)
set(JUCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/JuceLibraryCode)
set(RESOURCES_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Resources)
set(PLUGINS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Plugins)
set(SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Source)
set(TEST_HELPERS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Tests/TestHelpers)

configure_file(${JUCE_DIRECTORY}/JuceHeader.h.in ${JUCE_DIRECTORY}/JuceHeader.h)
configure_file(${RESOURCES_DIRECTORY}/Build-files/resources.rc.in ${RESOURCES_DIRECTORY}/Build-files/resources.rc)

#global properties
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS
	$<$<PLATFORM_ID:Windows>:_CRT_SECURE_NO_WARNINGS>
	$<$<PLATFORM_ID:Windows>:_CONSOLE>
	$<$<PLATFORM_ID:Windows>:NOMINMAX>
	$<$<CONFIG:Debug>:DEBUG=1>
	$<$<CONFIG:Debug>:_DEBUG=1>
	$<$<CONFIG:Release>:NDEBUG=1>
	JUCE_APP_VERSION=${GUI_VERSION}
	JUCE_APP_VERSION_HEX=${GUI_VERSION_HEX}
	)

if (APPLE)
	set(JUCE_FILES_EXTENSION mm)
else()
	set(JUCE_FILES_EXTENSION cpp)
endif()


set(JUCE_SOURCE
    ${JUCE_DIRECTORY}/AppConfig.h
    ${JUCE_DIRECTORY}/JuceHeader.h
    ${JUCE_DIRECTORY}/BinaryData.h
    ${JUCE_DIRECTORY}/BinaryData.cpp
    ${JUCE_DIRECTORY}/include_juce_audio_basics.${JUCE_FILES_EXTENSION}
    ${JUCE_DIRECTORY}/include_juce_audio_devices.${JUCE_FILES_EXTENSION}
    ${JUCE_DIRECTORY}/include_juce_audio_formats.${JUCE_FILES_EXTENSION}
    ${JUCE_DIRECTORY}/include_juce_audio_processors.${JUCE_FILES_EXTENSION}
    ${JUCE_DIRECTORY}/include_juce_audio_utils.${JUCE_FILES_EXTENSION}
    ${JUCE_DIRECTORY}/include_juce_core.${JUCE_FILES_EXTENSION}
    ${JUCE_DIRECTORY}/include_juce_cryptography.${JUCE_FILES_EXTENSION}
    ${JUCE_DIRECTORY}/include_juce_data_structures.${JUCE_FILES_EXTENSION}
    ${JUCE_DIRECTORY}/include_juce_events.${JUCE_FILES_EXTENSION}
    ${JUCE_DIRECTORY}/include_juce_graphics.${JUCE_FILES_EXTENSION}
	${JUCE_DIRECTORY}/include_juce_graphics_Harfbuzz.cpp
	${JUCE_DIRECTORY}/include_juce_graphics_Sheenbidi.c
    ${JUCE_DIRECTORY}/include_juce_gui_basics.${JUCE_FILES_EXTENSION}
    ${JUCE_DIRECTORY}/include_juce_gui_extra.${JUCE_FILES_EXTENSION}
    ${JUCE_DIRECTORY}/include_juce_opengl.${JUCE_FILES_EXTENSION}
    ${JUCE_DIRECTORY}/include_juce_osc.cpp
    ${JUCE_DIRECTORY}/include_juce_video.${JUCE_FILES_EXTENSION}
)

#create executable and add JUCE components
add_executable(open-ephys ${JUCE_SOURCE})
 if(MSVC)
	target_sources(open-ephys PRIVATE ${RESOURCES_DIRECTORY}/Build-files/resources.rc)
 endif()



target_include_directories(open-ephys PRIVATE ${JUCE_DIRECTORY} ${JUCE_DIRECTORY}/modules)
target_compile_features(open-ephys PUBLIC cxx_auto_type cxx_generalized_initializers cxx_std_17)

if(BUILD_TESTS)
	enable_testing()
	add_definitions(-DBUILD_TESTS)
	add_library(gui_testable_source SHARED ${JUCE_SOURCE})
	if(MSVC)
		set_property(TARGET gui_testable_source APPEND PROPERTY COMPILE_DEFINITIONS "JUCE_API=__declspec(dllexport)")
		target_compile_options(gui_testable_source PRIVATE /sdl- /nologo /MP /W0 /bigobj)
	endif()
	target_include_directories(gui_testable_source PRIVATE ${JUCE_DIRECTORY} ${JUCE_DIRECTORY}/modules)
	target_compile_features(gui_testable_source PUBLIC cxx_auto_type cxx_generalized_initializers cxx_std_17)

	set(BIN_TESTS_DIR ${CMAKE_BINARY_DIR}/TestBin)

	set_target_properties( gui_testable_source PROPERTIES RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/TestBin/common )
	set_target_properties( gui_testable_source PROPERTIES LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/TestBin/common )

	add_custom_command(TARGET gui_testable_source POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory "${BIN_TESTS_DIR}"
		)
endif()

file(GLOB _bitfiles "${RESOURCES_DIRECTORY}/Bitfiles/*.bit")
file(GLOB _xmlfiles "${RESOURCES_DIRECTORY}/Configs/*.xml")
#output folders and specific options
if (MSVC OR LINUX)
	set(BIN_PLUGIN_DIR $<TARGET_FILE_DIR:open-ephys>/plugins)
	set(BIN_SHARED_DIR $<TARGET_FILE_DIR:open-ephys>/shared)
	add_custom_command(TARGET open-ephys POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E make_directory "${BIN_SHARED_DIR}"
			COMMAND ${CMAKE_COMMAND} -E make_directory "${BIN_PLUGIN_DIR}"
			)

	#copy bitfiles
	add_custom_command(TARGET open-ephys POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy ${_bitfiles} "${BIN_SHARED_DIR}"
			)

	# copy default configs and their icons
	add_custom_command(TARGET open-ephys POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:open-ephys>/configs
				COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:open-ephys>/configs/icons
				COMMAND ${CMAKE_COMMAND} -E copy ${_xmlfiles} $<TARGET_FILE_DIR:open-ephys>/configs
				COMMAND ${CMAKE_COMMAND} -E copy_directory ${RESOURCES_DIRECTORY}/Configs/Icons $<TARGET_FILE_DIR:open-ephys>/configs/icons
				)

	#copy FileReader resources to output directory
	add_custom_command(TARGET open-ephys POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_directory ${RESOURCES_DIRECTORY}/FileReader/resources $<TARGET_FILE_DIR:open-ephys>/resources
				)

	#copy GUI icon to output directory
	add_custom_command(TARGET open-ephys POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy ${RESOURCES_DIRECTORY}/Icons/icon-small.png $<TARGET_FILE_DIR:open-ephys>/
			)
elseif(APPLE)
	set(BIN_PLUGIN_DIR $<TARGET_FILE_DIR:open-ephys>/../PlugIns)
	set(BIN_SHARED_DIR $<TARGET_FILE_DIR:open-ephys>/../Resources/shared)
	add_custom_command(TARGET open-ephys POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E make_directory "${BIN_PLUGIN_DIR}"
			COMMAND ${CMAKE_COMMAND} -E make_directory "${BIN_SHARED_DIR}"
			)
	
	#copy bitfiles
	add_custom_command(TARGET open-ephys POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy ${_bitfiles} "${BIN_SHARED_DIR}"
			)

	# copy default configs and their icons
	add_custom_command(TARGET open-ephys POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:open-ephys>/../Resources/configs
				COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:open-ephys>/../Resources/configs/icons
				COMMAND ${CMAKE_COMMAND} -E copy ${_xmlfiles} $<TARGET_FILE_DIR:open-ephys>/../Resources/configs
				COMMAND ${CMAKE_COMMAND} -E copy_directory ${RESOURCES_DIRECTORY}/Configs/Icons $<TARGET_FILE_DIR:open-ephys>/../Resources/configs/icons
				)
	
	#copy FileReader resources to output directory
	add_custom_command(TARGET open-ephys POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_directory ${RESOURCES_DIRECTORY}/FileReader/resources $<TARGET_FILE_DIR:open-ephys>/../Resources/resources
				)
	
	#copy GUI icon to output directory
	add_custom_command(TARGET open-ephys POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy ${RESOURCES_DIRECTORY}/Icons/icon-small.png $<TARGET_FILE_DIR:open-ephys>/../
			)
endif()

if(MSVC)
	set_property(TARGET open-ephys APPEND PROPERTY COMPILE_DEFINITIONS "JUCE_API=__declspec(dllexport)")
	target_compile_options(open-ephys PRIVATE /sdl- /nologo /MP /W0 /bigobj)
	target_link_libraries(open-ephys setupapi.lib opengl32.lib glu32.lib)
	set_property(TARGET open-ephys APPEND PROPERTY VS_DEBUGGER_COMMAND "Debug\\open-ephys.exe")
	set_property(TARGET open-ephys APPEND_STRING PROPERTY LINK_FLAGS_DEBUG " /NODEFAULTLIB:\"libcmt.lib\" /NODEFAULTLIB:\"msvcrt.lib\"")
	if (CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(DLLARCH "Win64")
	else()
		set(DLLARCH "Win32")
	endif()
	set(DLLDIR ${RESOURCES_DIRECTORY}/DLLs/${DLLARCH})
	file(GLOB _dllfiles "${DLLDIR}/*.dll")
	add_custom_command(TARGET open-ephys POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${_dllfiles} ${BIN_SHARED_DIR}
		)

elseif(LINUX)
	if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		target_compile_options(open-ephys PRIVATE -O0) #enable optimization for linux debug
	elseif (${CMAKE_BUILD_TYPE} STREQUAL "Release")
		target_compile_options(open-ephys PRIVATE -O3) #enable optimization for linux release
	else()
		message(FATAL_ERROR "Invalid build type")
	endif()
	find_package(CURL REQUIRED)
	find_package(PkgConfig REQUIRED)
	find_package(Fontconfig REQUIRED)

	# pkg_check_modules(GTK REQUIRED gtk+-3.0)

	target_include_directories(open-ephys PRIVATE
		/usr/include
		/usr/include/freetype2
		${CURL_INCLUDE_DIRS})

	target_link_libraries(open-ephys GL X11 Xext Xinerama asound dl freetype pthread rt ${Fontconfig_LIBRARIES} ${CURL_LIBRARIES})
	set_property(TARGET open-ephys APPEND_STRING PROPERTY LINK_FLAGS
		"-fvisibility=hidden -fPIC -rdynamic -Wl,-rpath,'$ORIGIN/shared'")

	target_compile_options(open-ephys PRIVATE -fPIC -rdynamic -fvisibility=hidden)
	set_property(TARGET open-ephys PROPERTY RUNTIME_OUTPUT_DIRECTORY ${BASE_BUILD_DIRECTORY}/${CMAKE_BUILD_TYPE})
    if(BUILD_TESTS)
            target_include_directories(gui_testable_source PRIVATE
                /usr/include
                /usr/include/freetype2
                ${CURL_INCLUDE_DIRS})
            target_link_libraries(gui_testable_source GL X11 Xext Xinerama asound dl freetype pthread rt ${Fontconfig_LIBRARIES} ${CURL_LIBRARIES})
            set_property(TARGET gui_testable_source APPEND_STRING PROPERTY LINK_FLAGS
                "-fvisibility=hidden -fPIC -rdynamic -Wl,-rpath,'$ORIGIN/shared'")

            target_compile_options(gui_testable_source PRIVATE -fPIC -rdynamic -fvisibility=hidden)
    endif()
elseif(APPLE)
	set_target_properties(open-ephys PROPERTIES
		MACOSX_BUNDLE TRUE
		OUTPUT_NAME "Open Ephys GUI"
		MACOSX_BUNDLE_VERSION ${GUI_VERSION}
		MACOSX_BUNDLE_ICON_FILE "Icon.icns"
		MACOSX_BUNDLE_INFO_PLIST "${RESOURCES_DIRECTORY}/Build-files/Info.plist.in"
		)
	target_compile_options(open-ephys PRIVATE -fPIC -Wpartial-availability -Wno-inconsistent-missing-override)
	target_link_options(open-ephys PRIVATE -rdynamic)
	target_link_libraries(open-ephys dl)
	target_link_libraries(open-ephys
		"-framework Accelerate"
		"-framework AudioToolbox"
		"-framework AVKit"
		"-framework AVFoundation"
		"-framework Carbon"
		"-framework Cocoa"
		"-framework CoreAudio"
		"-framework CoreAudioKit"
		"-framework CoreMedia"
		"-framework CoreMIDI"
		"-framework DiscRecording"
		"-framework Foundation"
		"-framework IOKit"
		"-framework Metal"
		"-framework MetalKit"
		"-framework OpenGL"
		"-framework QuartzCore"
		"-framework Security"
		"-framework WebKit"
	)
if(BUILD_TESTS)
    target_link_libraries(gui_testable_source
        "-framework Accelerate"
        "-framework AudioToolbox"
        "-framework AVKit"
        "-framework AVFoundation"
        "-framework Carbon"
        "-framework Cocoa"
        "-framework CoreAudio"
        "-framework CoreAudioKit"
        "-framework CoreMedia"
        "-framework CoreMIDI"
        "-framework DiscRecording"
        "-framework Foundation"
        "-framework IOKit"
        "-framework Metal"
        "-framework MetalKit"
        "-framework OpenGL"
        "-framework QuartzCore"
        "-framework Security"
        "-framework WebKit"
    )
endif()
	set_target_properties(open-ephys PROPERTIES
		XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
		XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED NO
		XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT dwarf
		XCODE_ATTRIBUTE_GCC_INLINES_ARE_PRIVATE_EXTERN YES
		XCODE_ATTRIBUTE_CLANG_LINK_OBJC_RUNTIME NO
		XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "org.open-ephys.gui"
		XCODE_ATTRIBUTE_EXECUTABLE_NAME "open-ephys"
		XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS "${CMAKE_CURRENT_SOURCE_DIR}/Resources/Build-files/entitlements.plist"
		)

		set(MAC_RESOURCE_FILES
			${RESOURCES_DIRECTORY}/Build-files/Icon.icns
			${RESOURCES_DIRECTORY}/Build-files/dmg_background.png
			${RESOURCES_DIRECTORY}/Build-files/RecentFilesMenuTemplate.nib
			${RESOURCES_DIRECTORY}/Fonts/unibody-8.otf
		)
		set_source_files_properties(${MAC_RESOURCE_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
		target_sources(open-ephys PRIVATE ${MAC_RESOURCE_FILES})


else()
	message( FATAL_ERROR "Unsupported OS")
endif()

#start recursive directory add
add_subdirectory(Source)

#create filters for vs and xcode
get_target_property(SRC_FILES open-ephys SOURCES)

foreach( src_file IN ITEMS ${SRC_FILES})
	get_filename_component(src_path "${src_file}" PATH)
	file(RELATIVE_PATH src_path_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${src_path}")
	string(REPLACE "/" "\\" group_name "${src_path_rel}")
	source_group("${group_name}" FILES "${src_file}")
endforeach()

#remove Main.cpp from open_ephys sources and add to API library
list(REMOVE_ITEM SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/Main.cpp)


if(BUILD_TESTS)
target_sources(gui_testable_source PRIVATE ${SRC_FILES})
endif()

#Add plugin build files
add_subdirectory(Plugins)


if(BUILD_TESTS)
add_subdirectory(Tests)
endif()
