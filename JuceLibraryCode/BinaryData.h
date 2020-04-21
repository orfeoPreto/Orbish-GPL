/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   Loop_h_bak;
    const int            Loop_h_bakSize = 1227;

    extern const char*   MenuManager_h_bak;
    const int            MenuManager_h_bakSize = 229;

    extern const char*   PluginEditor_cpp_bak;
    const int            PluginEditor_cpp_bakSize = 80403;

    extern const char*   PluginEditor_h_bak;
    const int            PluginEditor_h_bakSize = 11821;

    extern const char*   PluginProcessor_cpp_bak;
    const int            PluginProcessor_cpp_bakSize = 118887;

    extern const char*   PluginProcessor_h_bak;
    const int            PluginProcessor_h_bakSize = 6678;

    extern const char*   Track_cpp_bak;
    const int            Track_cpp_bakSize = 28363;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 7;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
