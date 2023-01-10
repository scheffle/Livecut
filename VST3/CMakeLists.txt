cmake_minimum_required(VERSION 3.14.0)

set(CMAKE_OSX_DEPLOYMENT_TARGET 10.14 CACHE STRING "")

set(vst3sdk_SOURCE_DIR CACHE PATH ../../vst3sdk)
if(NOT vst3sdk_SOURCE_DIR)
    message(FATAL_ERROR "Path to VST3 SDK is empty!")
endif()

project(Livecut VERSION 1.0.0)

set(SMTG_ADD_VSTGUI ON)

set(SMTG_VSTGUI_ROOT "${vst3sdk_SOURCE_DIR}")

add_subdirectory(${vst3sdk_SOURCE_DIR} ${PROJECT_BINARY_DIR}/vst3sdk)
smtg_enable_vst3_sdk()

add_library(lcdsp
	../lib/BBCutter.cpp
	../lib/BBCutter.h
	../lib/BitCrusher.cpp
	../lib/BitCrusher.h
	../lib/Comb.cpp
	../lib/Comb.h
	../lib/DelayLine.cpp
	../lib/DelayLine.h
	../lib/FirstOrderLowpass.cpp
	../lib/FirstOrderLowpass.h
	../lib/float_cast.h
	../lib/Functor.h
	../lib/FirstOrderLowpass.cpp
	../lib/SQPAmp.cpp
	../lib/SQPAmp.h
)

smtg_add_vst3plugin(Livecut
    source/version.h
    source/cids.h
    source/pids.h
    source/normplain.h
    source/paramdesc.h
    source/dspkernel.h
    source/parameter.cpp
    source/parameter.h
    source/processor.h
    source/processor.cpp
    source/controller.h
    source/controller.cpp
    source/editordelegate.cpp
    source/editordelegate.h
    source/entry.cpp
)

target_compile_features(Livecut
    PUBLIC
        cxx_std_17
)

#- VSTGUI Wanted ----
if(SMTG_ADD_VSTGUI)
	target_compile_definitions(Livecut
		PUBLIC
			LIVECUT_VSTGUI_SUPPORT
	)
    target_sources(Livecut
        PRIVATE
            resource/editor.uidesc
    )
    target_link_libraries(Livecut
        PRIVATE
            vstgui_support
    )
    smtg_target_add_plugin_resources(Livecut
        RESOURCES
            "resource/editor.uidesc"
    )
endif(SMTG_ADD_VSTGUI)
# -------------------

smtg_target_add_plugin_snapshots (Livecut
    RESOURCES
        resource/A5CBCDB2B9745B88A69760B164E92582_snapshot.png
        resource/A5CBCDB2B9745B88A69760B164E92582_snapshot_2.0x.png
)

smtg_target_add_plugin_resources (Livecut
	OUTPUT_SUBDIRECTORY
		fonts
    RESOURCES
        resource/Symbola.ttf
)

target_link_libraries(Livecut
    PRIVATE
        sdk
        lcdsp
)

if(SMTG_MAC)
	smtg_target_setup_universal_binary(lcdsp)
	smtg_target_setup_universal_binary(Livecut)

    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.14)
    smtg_target_set_bundle(Livecut
        BUNDLE_IDENTIFIER com.smartelectronix.mdsp.Livecut
        INFOPLIST "${CMAKE_CURRENT_LIST_DIR}/resource/Info.plist" PREPROCESS
    )
    smtg_target_set_debug_executable(Livecut
        "/Applications/VST3PluginTestHost.app"
        "--pluginfolder;$(BUILT_PRODUCTS_DIR)"
    )
elseif(SMTG_WIN)
    target_sources(Livecut PRIVATE 
        resource/win32resource.rc
    )
    if(MSVC)
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Livecut)

        smtg_target_set_debug_executable(Livecut
            "$(ProgramW6432)/Steinberg/VST3PluginTestHost/VST3PluginTestHost.exe"
            "--pluginfolder \"$(OutDir)/\""
        )
    endif()
endif(SMTG_MAC)

include(CheckFunctionExists)
check_function_exists(lrint HAVE_LRINT)
if(HAVE_LRINT)
  target_compile_definitions(lcdsp PRIVATE HAVE_LRINT=1)
  target_compile_definitions(Livecut PRIVATE HAVE_LRINT=1)
endif()

check_function_exists(lrintf HAVE_LRINTF)
if(HAVE_LRINTF)
  target_compile_definitions(lcdsp PRIVATE HAVE_LRINTF=1)
  target_compile_definitions(Livecut PRIVATE HAVE_LRINTF=1)
endif()

