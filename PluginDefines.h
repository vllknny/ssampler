#pragma once

//==============================================================================
/**
    This function must be implemented by a user of the JUCE library and should
    return the name of the plugin that is being built.

    @see JUCEPluginManifest
*/
const juce::String getPluginDescription();

//==============================================================================
/**
    If you're using the Projucer to build your plugin, it will create the main
   .cpp compilation unit for you with the correct plugin manifests and meta-data
    and you shouldn't need to edit this file.

    If you're creating a project without the Projucer, you'll need to copy the
    appropriate plugin manifest files from JUCE's extras/PluginManifests
    directory to your own project, and add this file to your build target.

    @see JUCEPluginManifest
*/
const char* getPluginManifest();

//==============================================================================
/**
    @internal

    This function is called by JUCEApplicationBase:: initialise() and should
    create and return the instance of the AudioProcessor subclass that this
    plugin is based on.

    It's safe to call this method multiple times, because each call will
    return a newly created object rather than re-using an existing one.
*/
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter();