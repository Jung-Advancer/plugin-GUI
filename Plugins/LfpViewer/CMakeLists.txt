# plugin build file
cmake_minimum_required(VERSION 3.15)

# include common rules
include(../PluginRules.cmake)

# add sources
add_sources(
  ${PLUGIN_NAME}
  OpenEphysLib.cpp
  LfpDisplayNode.cpp
  LfpDisplayNode.h
  LfpDisplayEditor.cpp
  LfpDisplayEditor.h
  DisplayBuffer.cpp
  DisplayBuffer.h
  EventDisplayInterface.cpp
  EventDisplayInterface.h
  LfpBitmapPlotter.h
  LfpBitmapPlotterInfo.h
  LfpChannelDisplay.cpp
  LfpChannelDisplay.h
  LfpChannelDisplayInfo.cpp
  LfpChannelDisplayInfo.h
  LfpDisplay.cpp
  LfpDisplay.h
  LfpDisplayClasses.h
  LfpDisplayCanvas.cpp
  LfpDisplayCanvas.h
  LfpDisplayOptions.cpp
  LfpDisplayOptions.h
  LfpTimescale.cpp
  LfpTimescale.h
  LfpViewport.cpp
  LfpViewport.h
  PerPixelBitmapPlotter.cpp
  PerPixelBitmapPlotter.h
  ShowHideOptionsButton.cpp
  ShowHideOptionsButton.h
  SupersampledBitmapPlotter.cpp
  SupersampledBitmapPlotter.h
  ColourSchemes/ChannelColourScheme.cpp
  ColourSchemes/ChannelColourScheme.h
  ColourSchemes/DefaultColourScheme.cpp
  ColourSchemes/DefaultColourScheme.h
  ColourSchemes/MonochromeGrayColourScheme.cpp
  ColourSchemes/MonochromeGrayColourScheme.h
  ColourSchemes/MonochromeYellowColourScheme.cpp
  ColourSchemes/MonochromeYellowColourScheme.h
  ColourSchemes/MonochromeGreenColourScheme.cpp
  ColourSchemes/MonochromeGreenColourScheme.h
  ColourSchemes/MonochromePurpleColourScheme.cpp
  ColourSchemes/MonochromePurpleColourScheme.h
  ColourSchemes/OELogoColourScheme.cpp
  ColourSchemes/OELogoColourScheme.h
  ColourSchemes/TropicalColourScheme.cpp
  ColourSchemes/TropicalColourScheme.h
  ColourSchemes/LightBackgroundColourScheme.cpp
  ColourSchemes/LightBackgroundColourScheme.h)

if(APPLE)
  set_target_properties(
    ${PLUGIN_NAME} PROPERTIES XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER
                              "org.open-ephys.plugin.LfpViewer")
endif()

if(BUILD_TESTS)
  add_subdirectory(Tests)
endif()
