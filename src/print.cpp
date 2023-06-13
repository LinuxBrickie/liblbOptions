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

#include <lb/options/print.h>

#include <iostream>

#include <lb/options/OptionDefinition.h>


namespace lb
{


namespace options
{


void split( std::ostream& os
          , const std::string& str
          , const std::string& indent
          , bool skipInitialIndent
          , std::string::size_type width )
{
  std::string::size_type i = 0;
  while ( i < str.size() )
  {
    // The line to print out. Might be less if we go back to the last space or
    // the first newline.
    auto s{ str.substr( i, width ) };

    // First look for a newline cahracter to see if we should start a new
    // paragraph.
    const auto firstNewline{ s.find_first_of( '\n' ) };
    const bool newParagraph{ firstNewline != std::string::npos };
    if ( newParagraph )
    {
      s = s.substr( 0, firstNewline );
      ++i; // Skip the newline for the next line
    }
    else if ( s.size() >= width )
    {
      // Not the last line (unless there are zero characters after this). Split
      // the line at a sensible place.
      const auto lastSpace{ s.find_last_of( ' ' ) };
      if ( lastSpace != std::string::npos )
      {
        s = s.substr( 0, lastSpace );
        ++i; // Skip the space for the next line.
      }
    }

    if ( skipInitialIndent )
    {
      skipInitialIndent = false;
    }
    else
    {
      os << indent;
    }
    os << s << '\n';
    if ( newParagraph )
    {
      os << '\n';
    }
    i += s.size();
  }
}

void print( std::ostream& os, const OptionDefinition& option, unsigned int indentation )
{
  const std::string indent1( indentation, ' ' );
  const std::string indent2( indent1 + "    " );
  os << indent1;
  if ( option.s != '\0' )
  {
    os << '-' << option.s;
  }
  if ( !option.l.empty() )
  {
    if ( option.s != '\0' )
    {
      os << ", ";
    }
    os << "--" << option.l;
  }
  os << '\n';

  const std::string::size_type width{ 64 };
  split( os, option.description, indent2, false, width );

  if ( !option.defaultValues.empty() )
  {
    if ( !option.description.empty() )
    {
      os << '\n';
    }
    const std::string defaults{ "Default: " };
    os << indent2 << defaults;
    const std::string indent3{ indent2 + std::string( defaults.size(), ' ' ) };
    std::string str;
    for ( const auto& dv : option.defaultValues )
    {
      str += dv + ' ';
    }
    split( os, str, indent3, true, width - defaults.size() );
  }
}


} // End of namespace options


} // End of namespace lb
