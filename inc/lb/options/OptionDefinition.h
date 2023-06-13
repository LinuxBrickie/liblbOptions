#ifndef LIB_LB_OPTIONS_OPTION_H
#define LIB_LB_OPTIONS_OPTION_H

/*
    This is free and unencumbered software released into the public domain.

    Anyone is free to copy, modify, publish, use, compile, sell, or
    distribute this software, either in source code form or as a compiled
    binary, for any purpose, commercial or non-commercial, and by any
    means.

    In jurisdictions that recognize copyright laws, the author or authors
    of this software dedicate any and all copyright interest in the
    software to the public domain. We make this dedication for the benefit
    of the public at large and to the detriment of our heirs and
    successors. We intend this dedication to be an overt act of
    relinquishment in perpetuity of all present and future rights to this
    software under copyright law.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
    OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.

    For more information, please refer to <https://unlicense.org>
*/

#include <string>
#include <vector>


namespace lb
{


namespace options
{


/** \brief Description of a command line option.

    Specifies
    - short and/or long flags, at least one must be present
    - minimum and/or maximum expected number of arguments
    - a description of the option for the help
    - a default value (or values)

    The description will be formatted for you when the help is printed out by
    lb::options::print().

    The default values are used in two ways. They are printed in the help for
    the option but are also used when an option is not explciitly present i.e.
    the option will be added if missing with the given default values. Note that
    they will *not* be used if the option is present but missing the required
    number of arguments, that is an error.

    A negative value for either \a minNumValues or \a maxNumValues indiciates
    that there is no limit.

    If you set both \a minNumValues and \a maxNumValues to non-negative values
    and the minimum is greater than the maximum then this will be pciked up as
    a parse error by the Options constructor. This is just a dumb structure so
    no sanity check is made here.
*/
struct OptionDefinition
{
  char        s{ '\0' }; //!< short version
  std::string l;       //!< long version

  int minNumValues{ -1 }; //!< The minimum number of required arguments, if any
  int maxNumValues{ -1 }; //!< The maximum number of required arguments, if any

  std::string description; //!< Description for help output.

  std::vector< std::string > defaultValues; //! Optional default values.
};


} // End of namespace options


} // End of namespace lb


#endif // LIB_LB_OPTIONS_OPTION_H
