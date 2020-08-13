/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   buttonbaseactive_png;
    const int            buttonbaseactive_pngSize = 4582;

    extern const char*   buttonbaseclicked_png;
    const int            buttonbaseclicked_pngSize = 4571;

    extern const char*   buttonbasehover_png;
    const int            buttonbasehover_pngSize = 3029;

    extern const char*   buttonbase_png;
    const int            buttonbase_pngSize = 1191;

    extern const char*   squarebuttonbaseactive_png;
    const int            squarebuttonbaseactive_pngSize = 5990;

    extern const char*   squarebuttonbaseclicked_png;
    const int            squarebuttonbaseclicked_pngSize = 7022;

    extern const char*   squarebuttonbasehover_png;
    const int            squarebuttonbasehover_pngSize = 3673;

    extern const char*   squarebuttonbase_png;
    const int            squarebuttonbase_pngSize = 2503;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 8;

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
