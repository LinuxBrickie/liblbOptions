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

#include <gtest/gtest.h>

#include <sstream>

#include <lb/options/print.h>
#include <lb/options/OptionDefinition.h>


void testPrintOptions1()
{
  //std::ostream& oss = std::cout;
  std::ostringstream oss;

  std::vector<lb::options::OptionDefinition> optionDefs
  {
    { 'a' , {}   , 0, 0
      , "A short option with a short description." },
    { '\0', "aaa", 0, 0
      , "A long option expecting no arguments and with no default values." },
    { 'a' , "aaa", 0, 0
      , "A short/long option expecting no arguments and with no default values." },
    { 'b' , "long-option-b",
      0, 0, "A short/long option expecting no arguments and with no default values." },
    { 'c' , "long-option-ccc", 1, 1
      , "A short/long option expecting one argument and with no default values. "
        "A lot more blurb to make it across several lines. Blah, blah, blah, blah!" },
    { 'd' , "really-long-option-ddd", 0, 0
      , "A short/long option expecting no arguments and with no default values.\n"
         "A new paragraph with more to say." },
    { 'e' , "reeeally-long-option-eee", 0, -1
      , "A short/long option expecting an unlimited number of arguments and with no default values.\n"
         "A new paragraph with more to say. This one much longer than the previous "
         "example." },
    { 'f' , "fff", 0, 0
      , "A short/long option expecting no arguments and with no default values.\n"
         "A new paragraph with more to say.\n"
         "Another paragraph, this one much longer than the previous so that it spans multipe lines.\n" },
    { 'g' , {}   , 1, 1
      , "A short option with a default.", { "default-g" } },
    { '\0', "hhh", 2, 2
      , "A long option expecting two arguments and with default values."
      , { "default-h-1", "default-h-2" } },
    { 'i' , "iii", 1, 2
      , "A short/long option expecting either 1 or 2 arguments with 1 default value."
      , { "default-i-1" } },
    { 'j' , "long-option-j", 1, 2
      , "A short/long option expecting either 1 or 2 arguments with 2 default values."
      , { "default-j-1", "default-j-2" } },
    { 'k' , "long-option-k", 3, 3
      , "A short/long option expecting 3 arguments and with 2 default values. "
        "A lot more blurb to make it across several lines. Blah, blah, blah, blah!"
      , { "default-k-1", "default-k-2" } },
    { 'l' , "really-long-option-lll", 3, 3
      , "A short/long option expecting 3 arguments and with 3 default values.\n"
         "A new paragraph with more to say."
      , { "default-l-1", "default-l-2" , "default-l-3" } },
    { 'm' , "really-long-option-mmm-mmmmm", 0, 5
      , "A short/long option expecting no more than 5 arguments and with 5 default values.\n"
         "A new paragraph with more to say. This one much longer than the previous "
         "example."
      , { "default-m-1", "default-m-2", "default-m-3", "default-m-4", "default-m-5" } },
    { 'n' , "nnn", 9, 9
      , "A short/long option expecting 9 arguments and with 9 default values.\n"
         "A new paragraph with more to say.\n"
         "Another paragraph, this one much longer than the previous so that it spans multipe lines."
      , { "default-n-1", "default-n-2", "default-n-3", "default-n-4", "default-n-5"
        , "default-n-6", "default-n-7", "default-n-8", "default-n-9" } }
  };

  const unsigned int Indent{ 4 };
  for ( const auto& optionDef : optionDefs )
  {
    lb::options::print( oss, optionDef, Indent );
  }

  const std::string expectedOutput
  {
    "    -a\n"
    "        A short option with a short description.\n"
    "    --aaa\n"
    "        A long option expecting no arguments and with no default\n"
    "        values.\n"
    "    -a, --aaa\n"
    "        A short/long option expecting no arguments and with no default\n"
    "        values.\n"
    "    -b, --long-option-b\n"
    "        A short/long option expecting no arguments and with no default\n"
    "        values.\n"
    "    -c, --long-option-ccc\n"
    "        A short/long option expecting one argument and with no default\n"
    "        values. A lot more blurb to make it across several lines. Blah,\n"
    "        blah, blah, blah!\n"
    "    -d, --really-long-option-ddd\n"
    "        A short/long option expecting no arguments and with no default\n"
    "        values.\n"
    "\n"
    "        A new paragraph with more to say.\n"
    "    -e, --reeeally-long-option-eee\n"
    "        A short/long option expecting an unlimited number of arguments\n"
    "        and with no default values.\n"
    "\n"
    "        A new paragraph with more to say. This one much longer than the\n"
    "        previous example.\n"
    "    -f, --fff\n"
    "        A short/long option expecting no arguments and with no default\n"
    "        values.\n"
    "\n"
    "        A new paragraph with more to say.\n"
    "\n"
    "        Another paragraph, this one much longer than the previous so\n"
    "        that it spans multipe lines.\n"
    "\n"
    "    -g\n"
    "        A short option with a default.\n"
    "\n"
    "        Defaults: default-g \n"
    "    --hhh\n"
    "        A long option expecting two arguments and with default values.\n"
    "\n"
    "        Defaults: default-h-1 default-h-2 \n"
    "    -i, --iii\n"
    "        A short/long option expecting either 1 or 2 arguments with 1\n"
    "        default value.\n"
    "\n"
    "        Defaults: default-i-1 \n"
    "    -j, --long-option-j\n"
    "        A short/long option expecting either 1 or 2 arguments with 2\n"
    "        default values.\n"
    "\n"
    "        Defaults: default-j-1 default-j-2 \n"
    "    -k, --long-option-k\n"
    "        A short/long option expecting 3 arguments and with 2 default\n"
    "        values. A lot more blurb to make it across several lines. Blah,\n"
    "        blah, blah, blah!\n"
    "\n"
    "        Defaults: default-k-1 default-k-2 \n"
    "    -l, --really-long-option-lll\n"
    "        A short/long option expecting 3 arguments and with 3 default\n"
    "        values.\n"
    "\n"
    "        A new paragraph with more to say.\n"
    "\n"
    "        Defaults: default-l-1 default-l-2 default-l-3 \n"
    "    -m, --really-long-option-mmm-mmmmm\n"
    "        A short/long option expecting no more than 5 arguments and with\n"
    "        5 default values.\n"
    "\n"
    "        A new paragraph with more to say. This one much longer than the\n"
    "        previous example.\n"
    "\n"
    "        Defaults: default-m-1 default-m-2 default-m-3 default-m-4\n"
    "                  default-m-5 \n"
    "    -n, --nnn\n"
    "        A short/long option expecting 9 arguments and with 9 default\n"
    "        values.\n"
    "\n"
    "        A new paragraph with more to say.\n"
    "\n"
    "        Another paragraph, this one much longer than the previous so\n"
    "        that it spans multipe lines.\n"
    "\n"
    "        Defaults: default-n-1 default-n-2 default-n-3 default-n-4\n"
    "                  default-n-5 default-n-6 default-n-7 default-n-8\n"
    "                  default-n-9 \n"
  };
  EXPECT_EQ( oss.str(), expectedOutput );
}


TEST(Options, Print)
{
  testPrintOptions1();
}
