#ifndef LIB_LB_OPTIONS_PARSEDOPTIONS_H
#define LIB_LB_OPTIONS_PARSEDOPTIONS_H

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

#include <lb/options/ParsedOption.h>

#include <string>
#include <unordered_map>
#include <vector>


namespace lb
{


namespace options
{


template< class Key, class Hash = std::hash<Key> >
struct ParsedOptions
{
  std::string executable;
  std::unordered_map< Key, ParsedOption, Hash > optionsByKey;
  std::vector< std::string > trailingValues;

  /** \brief Gives the position index withing argv of each {Key, occurrence} pair.

      Useful if you need to know the order of each Key occurence across Keys
      (the optionsByKey information only provides ordering for a Key). You can
      look up the ParsedOption by key.

      Option values are not listed here.

      Note that options absent from argv that have default values and which are
      therefore added to optinosByKey will *not* appear here.
  */
  struct ArgvEntry
  {
    ArgvEntry( size_t p, const Key& k, size_t i )
      : positionIndex{ p }, key{ k }, occurrenceIndex{ i } {};

    size_t positionIndex;   //!< The position index within argv. Must lie between 0 and argc-1.
    Key key;                //!< The key of the option at this index.
    size_t occurrenceIndex; //!< The occurrence index of \a key at this position index.
  };
  std::vector<ArgvEntry> optionsByArgvPosition;

  /** \brief Helper to check if a \a key is present or not.
      \return True if there is at least one occurrence of the \a key.
   */
  bool isPresent( const Key& key ) const
  {
    return optionsByKey.count( key ) > 0;
  }

  /** \brief Helper to get the last value of the last occurernce of \a key.
      \return The latest value set for \a key or an empty string if either the
              option is not present.
      \note This is likely not what you want if the option can take multiple
            arguments. Really intended for single argument options where they
            appear more than once and the last one wins.
   */
  std::string getLatestValue( const Key& key ) const
  {
    std::string latest;
    const auto I{ optionsByKey.find( key ) };
    if ( I != optionsByKey.end() )
    {
      const auto& values{ I->second.occurrences.back().values };
      if ( !values.empty() )
      {
        latest = values.back();
      }
    }
    return latest;
  }
};


} // End of namespace options


} // End of namespace lb


#endif // LIB_LB_OPTIONS_PARSEDOPTIONS_H
