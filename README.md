# liblbOptions

A public domain C++ library for parsing command line options.

lb is short for LinuxBrickie, my online handle.

## Dependencies

The main library has no dependencies (beyond STL).

The gtest binary dependencies are
- googletest( licensed under BSD 3-Clause)

## Usage

Choose a suitable option key type (an enum class is perfect) and create
an Options instance with that key type (and custom hash if required). The
constructor accepts a list of option defintions. Use the parse method to
parse a set of {argc, argv} against those definitions.

Multiple occurrences of options are supported. They will be returned in
the ParsedOptions structure as a vector ordered by their ordering in argv.
Options can be looked up either by key or by the original position in argv.

Default values are supported. Any option that has default values and that
is not present in the argv list is automatically added in to the ParsedOptions
structure as a single occurrence with those default values.

## Notes

Built and tested on Fedora 37.

If you don't need the print functionality then this is effectively a header
only library as everything else is templated with source in the headers.

I know that boost has program_options but I feel it is overengineered. Option
parsing should be simple and not trying to do too much. This library does not
attempt to introduce types to each option nor does it try to enforce anything
beyond the required number of arguments. Instead it simply gather together
the occurrences of each option and their values which can then be looked up
by whatever key you chose leaving the application logic to you.

