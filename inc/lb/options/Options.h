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

#ifndef LIB_LB_OPTIONS_OPTIONS_H
#define LIB_LB_OPTIONS_OPTIONS_H

#include <set>

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <optional>

#include <lb/options/KeyedOptionDefinition.h>
#include <lb/options/ParsedOptions.h>


namespace lb
{


namespace options
{


/** \brief Takes option definitions and parses an {argc.argv} set against them.

    The constructor takes a list of keyed option definitions. Once constructed
    use the \a parse method to decode a set of {argc, argv} against the stored
    definitions. This may be done more than once if required. The result of that
    is a ParsedOptions structure that allows you to search for options by your
    chosen key and/or work through the original order.
 */
template< class Key, class Hash = std::hash<Key> >
class Options
{
public:
  struct Configuration
  {
    bool allowTrailingValues{ true };
  };

  /** \brief Construct an Options instance from a list of option definitions.
      \throw std::runtime_error on construction failure (see description)

      Construction errors occur when sanity checking fails on the definitions.
      The following conditions are considered errors:
      - neither a short nor a long flag is set for an option
      - duplicated flag (either short or long)
      - duplicated key
      - both min and max num default values set and min > max
      - either too few or too many default values supplied for an option that
        takes a specific number of values
   */
  Options( std::initializer_list< KeyedOptionDefinition<Key> >
         , Configuration = {} );

  /** \brief Parse the given options into a ParsedOptions instance.
      \throw std::runtime_error on parse failure (see decsription)

      Parse errors cause std::runtime_error to be thrown. The following
      conditions are considered parse errors:
      - unknown option flag (either short or long)
      - insuffucient number of arguments based on option definition
      - excess number of arguments based on option definition unless they could
        be interpreted as trailing arguments
   */
  ParsedOptions<Key, Hash> parse( int argc, char** argv ) const;

  /** \brief Look up the definition for the option given by \a key. */
        OptionDefinition& getDefinition( Key key );
  const OptionDefinition& getDefinition( Key key ) const;

private:
  const Configuration config;

  using AvailableOptions = std::vector< KeyedOptionDefinition<Key> >;
  const AvailableOptions availableOptions;

  std::unordered_map< Key        , const KeyedOptionDefinition<Key>*, Hash > byKey;
  std::unordered_map< char       , const KeyedOptionDefinition<Key>*       > byShort;
  std::unordered_map< std::string, const KeyedOptionDefinition<Key>*       > byLong;
  std::vector< typename AvailableOptions::const_iterator > haveDefaults;
};


template< class Key, class Hash >
Options<Key, Hash>::Options( std::initializer_list<KeyedOptionDefinition<Key>> init
                           , Configuration c )
  : config{ c }
  , availableOptions{ init }
{
  // Keep track of all short and long options and make sure there are no
  // duplicates being registered.
  std::unordered_set<char> shortOptions;
  std::unordered_set<std::string> longOptions;

  // Keep track of all keys and make sure there are no duplicates
  std::unordered_set<Key, Hash> keys;

  // Set up the byKey, byShort, and byLong auxiliary maps but do sanity checks first.
  for ( auto A = availableOptions.cbegin(); A != availableOptions.cend(); ++A )
  {
    const auto& a{ *A };

    if ( a.option.s == '\0' && a.option.l.empty() )
    {
      throw std::runtime_error( "Misconfigured option, neither short not long flag specified." );
    }

    if ( a.option.s != '\0' )
    {
      if ( shortOptions.find( a.option.s ) != shortOptions.end() )
      {
        throw std::runtime_error(
          std::string{ "Misconfigured option, short option " } + a.option.s + " defined twice." );
      }
      else
      {
        shortOptions.insert( a.option.s );
      }
    }

    if ( !a.option.l.empty() )
    {
      if ( longOptions.find( a.option.l ) != longOptions.end() )
      {
        throw std::runtime_error(
          std::string{ "Misconfigured option, long option " } + a.option.l + " defined twice." );
      }
      else
      {
        longOptions.insert( a.option.l );
      }
    }

    const auto K{ keys.find( a.key ) };
    if ( K != keys.end() )
    {
      throw std::runtime_error(
        std::string{ "Misconfigured option, key already defined for " }
                   + ( a.option.s == '\0' ? a.option.l : std::string{ a.option.s } ) );
    }
    keys.emplace( a.key );

    if ( ( a.option.minNumValues > -1 )
      && ( a.option.maxNumValues > -1 )
      && ( a.option.minNumValues > a.option.maxNumValues ) )
    {
      throw std::runtime_error(
        std::string{ "Misconfigured option, min > max for " }
                   + ( a.option.s == '\0' ? a.option.l : std::string{ a.option.s } ) );
    }

    if ( !a.option.defaultValues.empty() )
    {
      if ( ( a.option.minNumValues > -1 ) && ( a.option.defaultValues.size() < a.option.minNumValues ) )
      {
        throw std::runtime_error(
          std::string{ "Misconfigured option, too few default values for " }
                     + ( a.option.s == '\0' ? a.option.l : std::string{ a.option.s } ) );
      }
      if ( ( a.option.maxNumValues > -1 ) && ( a.option.defaultValues.size() > a.option.maxNumValues ) )
      {
        throw std::runtime_error(
          std::string{ "Misconfigured option, too many default values for " }
                     + ( a.option.s == '\0' ? a.option.l : std::string{ a.option.s } ) );
      }

      haveDefaults.emplace_back( A );
    }

    byKey  [ a.key      ] = &a;
    byShort[ a.option.s ] = &a;
    byLong [ a.option.l ] = &a;
  }
}


template< class Key >
struct Parsing
{
  Parsing( const KeyedOptionDefinition<Key>& o
         , const std::string& invocationFlag
         , ParsedOption& p )
    : option{ o }, invocationFlag{ invocationFlag }, parsedOption{ p } {}

  const KeyedOptionDefinition<Key>& option;
  const std::string invocationFlag;
  ParsedOption& parsedOption;
};


template< class Key, class Hash >
ParsedOptions<Key, Hash> Options<Key, Hash>::parse( int argc, char** argv ) const
{
  ParsedOptions<Key, Hash> parsed{ argv[0] };

  parsed.optionsByKey.reserve( availableOptions.size() );
  parsed.optionsByArgvPosition.reserve( argc );

  // Note that we don't yet support option values that start with a dash. We
  // possibly could in cases where there are an exact number of expected
  // arguments but that's for future if it is ever required.
  std::optional<Parsing<Key>> currentlyParsing;

  std::vector< std::string > trailingValues;

  for ( int i = 1; i < argc; ++i )
  {
    std::string s{ argv[i] }; // non-const as it will be moved if it's not a flag

    if ( s[0] == '-' )
    {
      // Got a flag, is it short or long?
      if ( ( s.size() > 1 ) && ( s[1] == '-' ) )
      {
        // Long flag
        const auto L{ byLong.find( s.substr( 2 ) ) };
        if ( L == byLong.end() )
        {
          throw std::runtime_error{ std::string{ "Unknown long option " } + s.substr( 2 ) };
        }
        // Close off the flag we are currently parsing, if any
        if ( currentlyParsing )
        {
          if ( currentlyParsing->parsedOption.occurrences.back().values.size()
               < currentlyParsing->option.option.minNumValues )
          {
            throw std::runtime_error{ std::string{ "Too few values for option " }
                                    + currentlyParsing->invocationFlag };
          }
          if ( !trailingValues.empty() )
          {
            throw std::runtime_error{ std::string{ "Too many values for option " }
                                    + currentlyParsing->invocationFlag };
          }
        }
        const KeyedOptionDefinition<Key>& option{ *L->second };
        // Add or reuse parsed map entry as required
        currentlyParsing.emplace( option, s.substr( 2 ), parsed.optionsByKey[ option.key ] );
        parsed.optionsByArgvPosition.emplace_back( i, option.key, currentlyParsing->parsedOption.occurrences.size() );
        currentlyParsing->parsedOption.occurrences.emplace_back();
      }
      else // short flag, could be multiple short options all together
      {
        for ( std::string::size_type j = 1; j < s.size(); ++j )
        {
          const auto S{ byShort.find( s[j] ) };
          if ( S == byShort.end() )
          {
            throw std::runtime_error{ std::string{ "Unknown short option " } + s[j] };
          }
          // Close off the flag we are currently parsing, if any
          if ( currentlyParsing )
          {
            if ( currentlyParsing->parsedOption.occurrences.back().values.size()
                 < currentlyParsing->option.option.minNumValues )
            {
              throw std::runtime_error{ std::string{ "Too few values for option " }
                                      + currentlyParsing->invocationFlag };
            }
            if ( !trailingValues.empty() )
            {
              throw std::runtime_error{ std::string{ "Too many values for option " }
                                      + currentlyParsing->invocationFlag };
            }
          }
          const KeyedOptionDefinition<Key>& option{ *S->second };
          // Add or reuse parsed map entry as required
          currentlyParsing.emplace( option, &s[j], parsed.optionsByKey[ option.key ] );
          parsed.optionsByArgvPosition.emplace_back( i, option.key, currentlyParsing->parsedOption.occurrences.size() );
          currentlyParsing->parsedOption.occurrences.emplace_back();
        }
      }

      trailingValues.clear();
    }
    else // not a flag
    {
      if ( currentlyParsing )
      {
        auto& occurrence{ currentlyParsing->parsedOption.occurrences.back() };

        // Current policy is to treat excess values as an error unless they are
        // trailing but we don't know if they are trailing values until we've
        // finished looking for flags. So keep a note of them and if we hit
        // another flag then we throw and if not we file them under the trailing
        // values enumeration.
        if ( occurrence.values.size() == currentlyParsing->option.option.maxNumValues )
        {
          trailingValues.emplace_back( std::move( s ) );
        }
        else
        {
          occurrence.values.emplace_back( std::move( s ) );
        }
      }
      else
      {
        trailingValues.emplace_back( std::move( s ) );
      }
    }
  }

  // Close off the flag we are currently parsing, if any
  if ( currentlyParsing )
  {
    if ( currentlyParsing->parsedOption.occurrences.back().values.size()
         < currentlyParsing->option.option.minNumValues )
    {
      throw std::runtime_error{ std::string{ "Too few values for option " }
                              + currentlyParsing->invocationFlag };
    }
    // Only check for excess values here if we are not accepting trailing values.
    if ( !config.allowTrailingValues && !trailingValues.empty() )
    {
      throw std::runtime_error{ std::string{ "Too many values for option " }
                              + currentlyParsing->invocationFlag };
    }
  }

  if ( config.allowTrailingValues && !trailingValues.empty() )
  {
    parsed.trailingValues = std::move( trailingValues );
  }

  // Add in missing options that have defaults
  for ( auto A : haveDefaults )
  {
    if ( parsed.optionsByKey.find( A->key ) == parsed.optionsByKey.end() )
    {
      parsed.optionsByKey[ A->key ].occurrences.emplace_back().values = A->option.defaultValues;
    }
  }

  return parsed;
}

template< class Key, class Hash >
OptionDefinition& Options<Key, Hash>::getDefinition( Key key )
{
  const auto I{ byKey.find( key ) };
  if ( I != byKey.end() )
  {
    return I->second->option;
  }
  throw std::runtime_error( "Option key not found" );
}

template< class Key, class Hash >
const OptionDefinition& Options<Key, Hash>::getDefinition( Key key ) const
{
  const auto I{ byKey.find( key ) };
  if ( I != byKey.cend() )
  {
    return I->second->option;
  }
  throw std::runtime_error( "Option key not found" );
}


} // End of namespace options


} // End of namespace lb


#endif // LIB_LB_OPTIONS_OPTIONS_H
