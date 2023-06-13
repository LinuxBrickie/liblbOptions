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

#include <lb/options/Options.h>


#ifdef DEBUG
template< class Key, class Hash = std::hash<Key> >
void printParsedOptions( const lb::options::ParsedOptions<Key, Hash>& parsed )
{
  std::cout << "Exe:     " << parsed.executable << std::endl;
  std::cout << "Options:\n";
  for ( auto&[ key, value ] : parsed.optionsByKey )
  {
    std::cout << toString(key) << ':' << std::endl;
    for ( const auto& occurrence : value.occurrences )
    {
      std::cout << "  Occurrence\n";
      for ( const auto& v : occurrence.values )
      {
        std::cout << "    " << v << "  " << std::endl;
      }
    }
  }
  std::cout << "Trailing:\n";
  for ( const auto& v : parsed.trailingValues )
  {
    std::cout << "  " << v << "  " << std::endl;
  }
}
#endif

// Most tests use this simple enumeration as the option key
enum class OptionEnum
{
  eShortA,
  eShortB,
  eShortC,
  eShortD,
  eShortE,
  eLongA,
  eLongB,
  eLongC,
  eLongD,
  eLongE,
};

std::string toString( OptionEnum e )
{
  switch( e )
  {
  case OptionEnum::eShortA:
    return "eShortA";
  case OptionEnum::eShortB:
    return "eShortB";
  case OptionEnum::eShortC:
    return "eShortC";
  case OptionEnum::eShortD:
    return "eShortD";
  case OptionEnum::eShortE:
    return "eShortE";
  case OptionEnum::eLongA:
    return "eLongA";
  case OptionEnum::eLongB:
    return "eLongB";
  case OptionEnum::eLongC:
    return "eLongC";
  case OptionEnum::eLongD:
    return "eLongD";
  case OptionEnum::eLongE:
    return "eLongE";
  }
  return "Unknown";
}

// All other tests use this custom option key just to prove that works too
struct CustomKey
{
  std::string type;
  std::string option;
  bool operator==( const CustomKey& rhs ) const
  {
    return ( type == rhs.type ) && ( option == rhs.option );
  }
};

struct CustomHash
{
  std::size_t operator()( const CustomKey& key ) const
  {
    std::size_t h1 = std::hash<std::string>{}( key.type );
    std::size_t h2 = std::hash<std::string>{}( key.option );
    return h1 ^ ( h2 << 1 );
  }
};

std::string toString( const CustomKey& key )
{
  return key.type + key.option;
}


void testMisconfiguredOptions1()
{
  // Test that setting neither short nor long flag is a misconfiguration.
  const lb::options::KeyedOptionDefinition<OptionEnum> optionDefs
  {
    OptionEnum::eShortA, '\0' , {}, -1, -1, "A misconfigured short option with no flags."
  };
  EXPECT_THROW(
    const lb::options::Options<OptionEnum> misconfiguredOptions1
    {
      optionDefs
    },
    std::runtime_error
  );
}

void testMisconfiguredOptions2s()
{
  // Test that setting a duplicated short flag is a misconfiguration.
  const std::initializer_list<lb::options::KeyedOptionDefinition<OptionEnum>> optionDefs
  {
    { OptionEnum::eShortA, 'a', {}, -1, -1, "A valid short option." },
    { OptionEnum::eShortB, 'a', {}, -1, -1, "A misconfigured short option." }
  };

  ASSERT_THROW(
    const lb::options::Options<OptionEnum> misconfiguredOptions2s
    {
      optionDefs
    },
    std::runtime_error
  );
}

void testMisconfiguredOptions2l()
{
  // Test that setting a duplicated long flag is a misconfiguration.
  const std::initializer_list<lb::options::KeyedOptionDefinition<OptionEnum>> optionDefs
  {
    { OptionEnum::eLongA, '\0', "aaa", -1, -1, "A valid long option." },
    { OptionEnum::eLongB, '\0', "aaa", -1, -1, "A misconfigured long option." }
  };

  ASSERT_THROW(
    const lb::options::Options<OptionEnum> misconfiguredOptions2l
    {
      optionDefs
    },
    std::runtime_error
  );
}

void testMisconfiguredOptions2sc()
{
  // Custom key variation of testMisconfiguredOptions2s
  const std::initializer_list<lb::options::KeyedOptionDefinition<CustomKey>> optionDefs
  {
    { { "short", "a" }, 'a', {}, -1, -1, "A valid short option." },
    { { "short", "b" }, 'a', {}, -1, -1, "A misconfigured short option." }
  };

  using Options = lb::options::Options<CustomKey, CustomHash>;
  ASSERT_THROW(
    const Options misconfiguredOptions2s{ optionDefs },
    std::runtime_error
  );
}

void testMisconfiguredOptions2lc()
{
  // Test that setting a duplicated long flag is a misconfiguration.
  const std::initializer_list<lb::options::KeyedOptionDefinition<CustomKey>> optionDefs
  {
    { { "long", "a" }, '\0', "aaa", -1, -1, "A valid long option." },
    { { "long", "b" }, '\0', "aaa", -1, -1, "A misconfigured long option." }
  };

  using Options = lb::options::Options<CustomKey, CustomHash>;
  ASSERT_THROW(
    const Options misconfiguredOptions2l{ optionDefs },
    std::runtime_error
  );
}

void testMisconfiguredOptions3s()
{
  // Test that setting min > max is a misconfiguration.
  const lb::options::KeyedOptionDefinition<OptionEnum> optionDefs
  {
    OptionEnum::eShortA, 'a' , {}, 6, 1, "A misconfigured short option with min > max."
  };
  ASSERT_THROW(
    const lb::options::Options<OptionEnum> misconfiguredOptions3s
    {
      optionDefs
    },
    std::runtime_error
  );
}

void testMisconfiguredOptions3l()
{
  // Test that setting min > max is a misconfiguration.
  const lb::options::KeyedOptionDefinition<OptionEnum> optionDefs
  {
    OptionEnum::eLongA, '\0' , "aaa", 7, 6, "A misconfigured long option with min > max."
  };
  ASSERT_THROW(
    const lb::options::Options<OptionEnum> misconfiguredOptions3l
    {
      optionDefs
    },
    std::runtime_error
  );
}

void testMisconfiguredOptions4s()
{
  // Test that setting too many default values is a misconfiguration.
  const std::initializer_list<lb::options::KeyedOptionDefinition<OptionEnum>> optionDefs
  {
    { OptionEnum::eShortA, 'a', {}, 2, 3, "A misconfigured short option.", { { "1.23" }, { "ghj" }, { "[][]"}, { "oops" } } },
  };

  ASSERT_THROW(
    const lb::options::Options<OptionEnum> misconfiguredOptions4s
    {
      optionDefs
    },
    std::runtime_error
  );
}

void testMisconfiguredOptions4l()
{
  // Test that setting too many default values is a misconfiguration.
  const std::initializer_list<lb::options::KeyedOptionDefinition<OptionEnum>> optionDefs
  {
    { OptionEnum::eShortA, '\0', "aaa", 2, 3, "A misconfigured long option.", { { "1.23" }, { "ghj" }, { "[][]"}, { "oops" } } },
  };

  ASSERT_THROW(
    const lb::options::Options<OptionEnum> misconfiguredOptions4l
    {
      optionDefs
    },
    std::runtime_error
  );
}

void testMisconfiguredOptions5()
{
  // Test that setting duplicate keys is a misconfiguration.
  const std::initializer_list<lb::options::KeyedOptionDefinition<OptionEnum>> optionDefs
  {
    { OptionEnum::eShortA, 'a' , {}   , 2, 3, "A valid short option." },
    { OptionEnum::eShortA, '\0', "aaa", 1, 3, "A misconfigured long option." },
  };

  ASSERT_THROW(
    const lb::options::Options<OptionEnum> misconfiguredOptions5
    {
      optionDefs
    },
    std::runtime_error
  );
}

void testMisconfiguredOptions5c()
{
  // Custom key variation of testMisconfiguredOptions5.
  const std::initializer_list<lb::options::KeyedOptionDefinition<CustomKey>> optionDefs
  {
    { { "long", "a" }, 'a' , {}   , 2, 3, "A valid short option." },
    { { "long", "a" }, '\0', "aaa", 1, 3, "A misconfigured long option." },
  };

  using Options = lb::options::Options<CustomKey, CustomHash>;
  ASSERT_THROW(
    const Options misconfiguredOptions5{ optionDefs },
    std::runtime_error
  );
}

void testUnknownOptions1s()
{
  // Test that an unknown short option throws an error.
  const lb::options::Options<OptionEnum> unknownOptions1s
  {
  };

  const char* argv[2]
  {
    { "exe" },
    { "-a" }
  };

  ASSERT_THROW( unknownOptions1s.parse( sizeof(argv)/sizeof(argv[0]), const_cast<char**>( argv ) ), std::runtime_error );
}

void testUnknownOptions1l()
{
  // Test that an unknown long option throws an error.
  const lb::options::Options<OptionEnum> unknownOptions1l
  {
  };

  const char* argv[2]
  {
    { "exe" },
    { "--aaa" }
  };

  ASSERT_THROW( unknownOptions1l.parse( sizeof(argv)/sizeof(argv[0]), const_cast<char**>( argv ) ), std::runtime_error );
}

void testBadOptions1s()
{
  // Test that an excess argument to a short option that cannot be interpreted
  // as a trailing argument throws an error.
  const lb::options::Options<OptionEnum> badOptions1s
  {
    { OptionEnum::eShortA, 'a' , {}, 0, 0, "A short option that requires no arguments." },
    { OptionEnum::eShortB, 'b' , {}, 0, 0, "A short option that requires no arguments." },
  };

  const char* argv[4]
  {
    { "exe" },
    { "-a" }, { "oops" },
    { "-b" }
  };

  ASSERT_THROW( badOptions1s.parse( sizeof(argv)/sizeof(argv[0]), const_cast<char**>( argv ) ), std::runtime_error );
}

void testBadOptions1l()
{
  // Test that an excess argument to a long option that cannot be interpreted
  // as a trailing argument throws an error.
  const lb::options::Options<OptionEnum> badOptions1l
  {
    { OptionEnum::eLongA , '\0', "aaa", 0, 0, "A long option that requires no arguments." },
    { OptionEnum::eShortB, 'b' , {}   , 0, 0, "A short option that requires no arguments." },
  };

  const char* argv[4]
  {
    { "exe" },
    { "--aaa" }, { "oops" },
    { "-b" }
  };

  ASSERT_THROW( badOptions1l.parse( sizeof(argv)/sizeof(argv[0]), const_cast<char**>( argv ) ), std::runtime_error );
}

void testBadOptions2s()
{
  // Test that missing arguments to a short option throws an error.
  const lb::options::Options<OptionEnum> badOptions2s
  {
    { OptionEnum::eShortA, 'a' , {}, 3, 3, "A short option that expects 3 arguments." },
    { OptionEnum::eShortB, 'b' , {}, 0, 0, "A short option that requires no arguments." },
  };

  const char* argv[3]
  {
    { "exe" },
    { "-a" },
    { "-b" }
  };

  ASSERT_THROW( badOptions2s.parse( sizeof(argv)/sizeof(argv[0]), const_cast<char**>( argv ) ), std::runtime_error );
}

void testBadOptions2l()
{
  // Test that missing arguments to a long option throws an error.
  const lb::options::Options<OptionEnum> badOptions2l
  {
    { OptionEnum::eLongA , 'a' , {}, 3, 3, "A long option that expects 3 arguments." },
    { OptionEnum::eShortB, 'b' , {}, 0, 0, "A short option that requires no arguments." },
  };

  const char* argv[3]
  {
    { "exe" },
    { "--aaa" },
    { "-b" }
  };

  ASSERT_THROW( badOptions2l.parse( sizeof(argv)/sizeof(argv[0]), const_cast<char**>( argv ) ), std::runtime_error );
}

void testBadOptions3s()
{
  // Test that one missing argument to a short option throws an error.
  const lb::options::Options<OptionEnum> badOptions3s
  {
    { OptionEnum::eShortA, 'a' , {}, 3, 3, "A short option that expects 3 arguments." },
    { OptionEnum::eShortB, 'b' , {}, 0, 0, "A short option that requires no arguments." },
  };

  const char* argv[5]
  {
    { "exe" },
    { "-a" }, { "bob" }, { "sue" },
    { "-b" }
  };

  ASSERT_THROW( badOptions3s.parse( sizeof(argv)/sizeof(argv[0]), const_cast<char**>( argv ) ), std::runtime_error );
}

void testBadOptions3l()
{
  // Test that one missing argument to a short option throws an error.
  const lb::options::Options<OptionEnum> badOptions3l
  {
    { OptionEnum::eLongA, 'a' , {}, 3, 3, "A long option that expects 3 arguments." },
    { OptionEnum::eShortB, 'b' , {}, 0, 0, "A short option that requires no arguments." },
  };

  const char* argv[5]
  {
    { "exe" },
    { "-aaa" }, { "bob" }, { "sue" },
    { "-b" }
  };

  ASSERT_THROW( badOptions3l.parse( sizeof(argv)/sizeof(argv[0]), const_cast<char**>( argv ) ), std::runtime_error );
}

void testBadOptions4s()
{
  // Test that an excess argument to a short option expecting argumenst that
  // cannot be interpreted as a trailing argument throws an error.
  const lb::options::Options<OptionEnum> badOptions4s
  {
    { OptionEnum::eShortA, 'a' , {}, 2, 4, "A short option that requires at most 4 arguments." },
    { OptionEnum::eShortB, 'b' , {}, 0, 0, "A short option that requires no arguments." },
  };

  const char* argv[8]
  {
    { "exe" },
    { "-a" }, { "oops" }, { "I" }, { "did" }, { "it" }, { "again" },
    { "-b" }
  };

  ASSERT_THROW( badOptions4s.parse( sizeof(argv)/sizeof(argv[0]), const_cast<char**>( argv ) ), std::runtime_error );
}

void testBadOptions4l()
{
  // Test that an excess argument to a long option expecting arguments that
  // cannot be interpreted as a trailing argument throws an error.
  const lb::options::Options<OptionEnum> badOptions4l
  {
    { OptionEnum::eLongA , '\0', "aaa", 1, 4, "A long option that requires at most 4 arguments." },
    { OptionEnum::eShortB, 'b' , {}   , 0, 0, "A short option that requires no arguments." },
  };

  const char* argv[8]
  {
    { "exe" },
    { "--aaa" }, { "oops" }, { "I" }, { "did" }, { "it" }, { "again" },
    { "-b" }
  };

  ASSERT_THROW( badOptions4l.parse( sizeof(argv)/sizeof(argv[0]), const_cast<char**>( argv ) ), std::runtime_error );
}

void testBadOptions5s()
{
  // Test that missing arguments to a short option that expetcs at least 3
  // arguments throws an error.
  const lb::options::Options<OptionEnum> badOptions5s
  {
    { OptionEnum::eShortA, 'a' , {}, 3, 4, "A short option that expects at least 3 arguments." },
    { OptionEnum::eShortB, 'b' , {}, 0, 0, "A short option that requires no arguments." },
  };

  const char* argv[5]
  {
    { "exe" },
    { "-a" }, { "one" }, { "short" },
    { "-b" }
  };

  ASSERT_THROW( badOptions5s.parse( sizeof(argv)/sizeof(argv[0]), const_cast<char**>( argv ) ), std::runtime_error );
}

void testBadOptions5l()
{
  // Test that missing arguments to a long option that expetcs at least 3
  // arguments throws an error.
  const lb::options::Options<OptionEnum> badOptions5l
  {
    { OptionEnum::eLongA, '\0' , "aaa", 3, 7, "A long option that expects 3 arguments." },
    { OptionEnum::eLongB, '\0' , "bbb", 0, 0, "A slong option that requires no arguments." },
  };

  const char* argv[5]
  {
    { "exe" },
    { "-aaa" }, { "and" }, { "again" },
    { "-bbb" }
  };

  ASSERT_THROW( badOptions5l.parse( sizeof(argv)/sizeof(argv[0]), const_cast<char**>( argv ) ), std::runtime_error );
}

void testBadOptions6s()
{
  // Test that an excess argument to a short option that is not allowed to be
  // interpreted as a trailing argument throws an error.
  const lb::options::Options<OptionEnum> badOptions6s
  {
    {
      { OptionEnum::eShortA, 'a' , {}, 0, 0, "A short option that requires no arguments." },
      { OptionEnum::eShortB, 'b' , {}, 0, 0, "A short option that requires no arguments." },
    },
    { false } // don't allow trailing arguments
  };

  const char* argv[3]
  {
    { "exe" },
    { "-a" }, { "oops" },
  };

  ASSERT_THROW( badOptions6s.parse( sizeof(argv)/sizeof(argv[0]), const_cast<char**>( argv ) ), std::runtime_error );
}

void testBadOptions6l()
{
  // Test that missing arguments to a long option that expetcs at most 1
  // arguments throws an error.
  const lb::options::Options<OptionEnum> badOptions6l
  {
    {
      { OptionEnum::eLongA, '\0', "aaa", 0, 1, "A long option that expects at most 1 argument." },
      { OptionEnum::eLongB, '\0', "bbb", 0, 0, "A long option that requires no arguments." },
    },
    { false } // don't allow trailing arguments
  };

  const char* argv[4]
  {
    { "exe" },
    { "-a" }, { "and" }, { "again" },
  };

  ASSERT_THROW( badOptions6l.parse( sizeof(argv)/sizeof(argv[0]), const_cast<char**>( argv ) ), std::runtime_error );
}

template< class Key, class Hash = std::hash<Key> >
struct TestParsedOption
{
  TestParsedOption( const lb::options::ParsedOptions<Key, Hash>& parsed
                  , const std::string& testContext )
    : parsed{ parsed }
    , testContext{ testContext }
  {
  }

  void operator()( const Key& key
                 , const size_t expectedNumOccurences
                 , const std::vector< std::vector< std::string > >& expectedValuesPerOccurrence )
  {
    const std::string context{ " in " + testContext + " for " + toString( key ) };
    const auto I{ parsed.optionsByKey.find( key ) };
    EXPECT_NE( I, parsed.optionsByKey.end() ) << context;
    ASSERT_EQ( I->second.occurrences.size(), expectedNumOccurences ) << context;
    for ( size_t i = 0; i < expectedValuesPerOccurrence.size(); ++i )
    {
      const auto& expectedValues{ expectedValuesPerOccurrence[i] };
      const auto& actualValues{ I->second.occurrences[i].values };
      EXPECT_EQ( actualValues.size(), expectedValues.size() ) << context << " where i=" << i;
      for ( size_t j = 0; j < expectedValues.size(); ++j )
      {
        EXPECT_EQ( actualValues[j], expectedValues[j] ) << context << " where i=" << i << " and j=" << j;
      }
    }
  }

  void operator() ( const std::vector<std::tuple<size_t, Key, size_t> >& byPosition )
  {
    if ( byPosition.size() != parsed.optionsByArgvPosition.size() )
    {
      FAIL() << "By Position vector size mismatch for " << testContext;
    }
    auto P{ parsed.optionsByArgvPosition.begin() };
    for ( const auto& tuple : byPosition )
    {
      EXPECT_EQ( P->positionIndex  , std::get<0>( tuple ) ) << testContext << " position index " << P->positionIndex;
      EXPECT_EQ( P->key            , std::get<1>( tuple ) ) << testContext << " position index " << P->positionIndex;
      EXPECT_EQ( P->occurrenceIndex, std::get<2>( tuple ) ) << testContext << " position index " << P->positionIndex;
      ++P;
    }
  }

  const lb::options::ParsedOptions<Key, Hash>& parsed;
  const std::string testContext;
};

void testDefaultedOptions1s()
{
  // Test that default values do not override given values.
  const lb::options::Options<OptionEnum> defaultedOptions1s
  {
    { OptionEnum::eShortA, 'a' , {}, 1, 1, "A short option that expects 1 argument.", { "default-a-1" } },
    { OptionEnum::eShortB, 'b' , {}, 0, 1, "A short option that expects at most 1 argument.", { "default-b-1" } },
    { OptionEnum::eShortC, 'c' , {}, 0, 1, "Another short option that expects at most 1 argument.", { "default-c-1" } },
    { OptionEnum::eShortD, 'd' , {}, 3, 3, "A short option that expects 3 arguments.", { "default-d-1" , "default-d-2", "default-d-3" } },
  };

  const char* argv[10]
  {
    { "exe" },
    { "-a" }, { "override-a" },
    { "-b" },
    { "-c" }, { "override-c" },
    { "-d" }, { "override-d-1" }, { "override-d-2" }, { "override-d-3" }
  };

  lb::options::ParsedOptions<OptionEnum> parsed;
  ASSERT_NO_THROW( parsed = defaultedOptions1s.parse( sizeof(argv)/sizeof(argv[0]), const_cast<char**>( argv ) ) );

  TestParsedOption<OptionEnum> testParsedOption{ parsed, "testDefaultedOptions1s" };

  // Test by Key
  testParsedOption( OptionEnum::eShortA, 1, { { "override-a" } } );
  testParsedOption( OptionEnum::eShortB, 1, { } );
  testParsedOption( OptionEnum::eShortC, 1, { { "override-c" } } );
  testParsedOption( OptionEnum::eShortD, 1, { { "override-d-1", "override-d-2", "override-d-3" } } );

  // Test by position index
  testParsedOption( { { 1, OptionEnum::eShortA, 0 }
                    , { 3, OptionEnum::eShortB, 0 }
                    , { 4, OptionEnum::eShortC, 0 }
                    , { 6, OptionEnum::eShortD, 0 } } );
}

void testDefaultedOptions1l()
{
  // Test that default values do not override given values.
  const lb::options::Options<OptionEnum> defaultedOptions1l
  {
    { OptionEnum::eLongA, 'a' , {}, 1, 1, "A long option that expects 1 argument.", { "default-a-1" } },
    { OptionEnum::eLongB, 'b' , {}, 0, 1, "A long option that expects at most 1 argument.", { "default-b-1" } },
    { OptionEnum::eLongC, 'c' , {}, 0, 1, "Another long option that expects at most 1 argument.", { "default-c-1" } },
    { OptionEnum::eLongD, 'd' , {}, 3, 3, "A long option that expects 3 arguments.", { "default-d-1" , "default-d-2", "default-d-3" } },
  };

  const char* argv[10]
  {
    { "exe" },
    { "-a" }, { "override-a" },
    { "-b" },
    { "-c" }, { "override-c" },
    { "-d" }, { "override-d-1" }, { "override-d-2" }, { "override-d-3" }
  };

  lb::options::ParsedOptions<OptionEnum> parsed;
  ASSERT_NO_THROW( parsed = defaultedOptions1l.parse( sizeof(argv)/sizeof(argv[0]), const_cast<char**>( argv ) ) );

  TestParsedOption<OptionEnum> testParsedOption{ parsed, "testDefaultedOptions1l" };

  // Test by Key
  testParsedOption( OptionEnum::eLongA, 1, { { "override-a" } } );
  testParsedOption( OptionEnum::eLongB, 1, { } );
  testParsedOption( OptionEnum::eLongC, 1, { { "override-c" } } );
  testParsedOption( OptionEnum::eLongD, 1, { { "override-d-1", "override-d-2", "override-d-3" } } );

  // Test by position index
  testParsedOption( { { 1, OptionEnum::eLongA, 0 }
                    , { 3, OptionEnum::eLongB, 0 }
                    , { 4, OptionEnum::eLongC, 0 }
                    , { 6, OptionEnum::eLongD, 0 } } );
}

void testDefaultedOptions2s()
{
  // Test that default values do not override given values.
  const lb::options::Options<OptionEnum> defaultedOptions2s
  {
    { OptionEnum::eShortA, 'a' , {}, 0, 2, "A short option that expects at most 2 arguments.", { "default-a-1", "default-a-2" } },
    { OptionEnum::eShortB, 'b' , {}, 0, 2, "A short option that expects at most 2 arguments.", { "default-b-1", "default-b-2" } },
    { OptionEnum::eShortC, 'c' , {}, 0, 2, "A short option that expects at most 2 arguments.", { "default-c-1", "default-c-2" } },
    { OptionEnum::eShortD, 'd' , {}, 0, 2, "A short option that expects at most 2 arguments.", { "default-d-1" } },
    { OptionEnum::eShortE, 'e' , {}, 0, 2, "A short option that expects at most 2 arguments.", { "default-e-1" } },
  };

  const char* argv[12]
  {
    { "exe" },
    { "-a" },
    { "-b" }, { "override-b-1" },
    { "-c" }, { "override-c-1" }, { "override-c-2" },
    { "-d" }, { "override-d-1" },
    { "-e" }, { "override-e-1" }, { "override-e-2" },
  };

  lb::options::ParsedOptions<OptionEnum> parsed;
  ASSERT_NO_THROW( parsed = defaultedOptions2s.parse( sizeof(argv)/sizeof(argv[0]), const_cast<char**>( argv ) ) );

  TestParsedOption<OptionEnum> testParsedOption{ parsed, "testDefaultedOptions2s" };

  // Test by Key
  testParsedOption( OptionEnum::eShortA, 1, { { } } );
  testParsedOption( OptionEnum::eShortB, 1, { { "override-b-1" } } );
  testParsedOption( OptionEnum::eShortC, 1, { { "override-c-1", "override-c-2" } } );
  testParsedOption( OptionEnum::eShortD, 1, { { "override-d-1" } } );
  testParsedOption( OptionEnum::eShortE, 1, { { "override-e-1", "override-e-2" } } );

  // Test by position index
  testParsedOption( { { 1, OptionEnum::eShortA, 0 }
                    , { 2, OptionEnum::eShortB, 0 }
                    , { 4, OptionEnum::eShortC, 0 }
                    , { 7, OptionEnum::eShortD, 0 }
                    , { 9, OptionEnum::eShortE, 0 } } );
}

void testDefaultedOptions2l()
{
  // Test that default values do not override given values.
  const lb::options::Options<OptionEnum> defaultedOptions2l
  {
    { OptionEnum::eLongA, '\0' , "aaa", 0, 2, "A long option that expects at most 2 arguments.", { "default-a-1", "default-a-2" } },
    { OptionEnum::eLongB, '\0' , "bbb", 0, 2, "A long option that expects at most 2 arguments.", { "default-b-1", "default-b-2" } },
    { OptionEnum::eLongC, '\0' , "ccc", 0, 2, "A long option that expects at most 2 arguments.", { "default-c-1", "default-c-2" } },
    { OptionEnum::eLongD, '\0' , "ddd", 0, 2, "A long option that expects at most 2 arguments.", { "default-d-1" } },
    { OptionEnum::eLongE, '\0' , "eee", 0, 2, "A long option that expects at most 2 arguments.", { "default-e-1" } },
  };

  const char* argv[12]
  {
    { "exe" },
    { "--aaa" },
    { "--bbb" }, { "override-b-1" },
    { "--ccc" }, { "override-c-1" }, { "override-c-2" },
    { "--ddd" }, { "override-d-1" },
    { "--eee" }, { "override-e-1" }, { "override-e-2" },
  };

  lb::options::ParsedOptions<OptionEnum> parsed;
  ASSERT_NO_THROW( parsed = defaultedOptions2l.parse( sizeof(argv)/sizeof(argv[0]), const_cast<char**>( argv ) ) );

  TestParsedOption<OptionEnum> testParsedOption{ parsed, "testDefaultedOptions2s" };

  // Test by Key
  testParsedOption( OptionEnum::eLongA, 1, { { } } );
  testParsedOption( OptionEnum::eLongB, 1, { { "override-b-1" } } );
  testParsedOption( OptionEnum::eLongC, 1, { { "override-c-1", "override-c-2" } } );
  testParsedOption( OptionEnum::eLongD, 1, { { "override-d-1" } } );
  testParsedOption( OptionEnum::eLongE, 1, { { "override-e-1", "override-e-2" } } );

  // Test by position index
  testParsedOption( { { 1, OptionEnum::eLongA, 0 }
                    , { 2, OptionEnum::eLongB, 0 }
                    , { 4, OptionEnum::eLongC, 0 }
                    , { 7, OptionEnum::eLongD, 0 }
                    , { 9, OptionEnum::eLongE, 0 } } );
}

void testDefaultedOptions3s()
{
  // Test that default values do appear when there are no overriding values.
  const lb::options::Options<OptionEnum> defaultedOptions3s
  {
    { OptionEnum::eShortA, 'a' , {}, 1, 1, "A short option that expects 1 argument.", { "default-a-1" } },
    { OptionEnum::eShortB, 'b' , {}, 0, 1, "A short option that expects at most 1 arguments", { "default-b-1" } },
    { OptionEnum::eShortC, 'c' , {}, 3, 3, "A short option that expects 3 arguments.", { "default-c-1", "default-c-2", "default-c-3" } },
    { OptionEnum::eShortD, 'd' , {}, 0, 2, "A short option that expects at most 2 arguments.", { "default-d-1" } },
    { OptionEnum::eShortE, 'e' , {}, 0, 2, "A short option that expects at most 2 arguments.", { "default-e-1", "default-e-2" } },
  };

  const char* argv[1]
  {
    { "exe" },
  };

  lb::options::ParsedOptions<OptionEnum> parsed;
  ASSERT_NO_THROW( parsed = defaultedOptions3s.parse( sizeof(argv)/sizeof(argv[0]), const_cast<char**>( argv ) ) );

  TestParsedOption<OptionEnum> testParsedOption{ parsed, "testDefaultedOptions3s" };

  // Test by Key
  testParsedOption( OptionEnum::eShortA, 1, { { "default-a-1"} } );
  testParsedOption( OptionEnum::eShortB, 1, { { "default-b-1" } } );
  testParsedOption( OptionEnum::eShortC, 1, { { "default-c-1", "default-c-2", "default-c-3" } } );
  testParsedOption( OptionEnum::eShortD, 1, { { "default-d-1" } } );
  testParsedOption( OptionEnum::eShortE, 1, { { "default-e-1", "default-e-2" } } );

  // Test by position index
  testParsedOption( { } );
}

void testDefaultedOptions3l()
{
  // Test that default values do appear when there are no overriding values.
  const lb::options::Options<OptionEnum> defaultedOptions3l
  {
    { OptionEnum::eLongA, '\0' , "aaa", 1, 1, "A long option that expects 1 argument.", { "default-a-1" } },
    { OptionEnum::eLongB, '\0' , "bbb", 0, 1, "A long option that expects at most 1 arguments", { "default-b-1" } },
    { OptionEnum::eLongC, '\0' , "ccc", 3, 3, "A long option that expects 3 arguments.", { "default-c-1", "default-c-2", "default-c-3" } },
    { OptionEnum::eLongD, '\0' , "ddd", 0, 2, "A long option that expects at most 2 arguments.", { "default-d-1" } },
    { OptionEnum::eLongE, '\0' , "eee", 0, 2, "A long option that expects at most 2 arguments.", { "default-e-1", "default-e-2" } },
  };

  const char* argv[1]
  {
    { "exe" },
  };

  lb::options::ParsedOptions<OptionEnum> parsed;
  ASSERT_NO_THROW( parsed = defaultedOptions3l.parse( sizeof(argv)/sizeof(argv[0]), const_cast<char**>( argv ) ) );

  TestParsedOption<OptionEnum> testParsedOption{ parsed, "testDefaultedOptions3s" };

  // Test by Key
  testParsedOption( OptionEnum::eLongA, 1, { { "default-a-1"} } );
  testParsedOption( OptionEnum::eLongB, 1, { { "default-b-1" } } );
  testParsedOption( OptionEnum::eLongC, 1, { { "default-c-1", "default-c-2", "default-c-3" } } );
  testParsedOption( OptionEnum::eLongD, 1, { { "default-d-1" } } );
  testParsedOption( OptionEnum::eLongE, 1, { { "default-e-1", "default-e-2" } } );

  // Test by position index
  testParsedOption( { } );
}

void testGoodOptions1()
{
  const lb::options::Options<OptionEnum> goodOptions1
  {
    { OptionEnum::eShortA, 'a' , {}              , 0, 0, "A short option that requires no arguments." },
    { OptionEnum::eShortB, 'b' , {}              , 1, 1, "A short option that requires a single argument without a default.", },
    { OptionEnum::eShortC, 'c' , {}              , 1, 1, "A short option that requires a single argument with a default.", { "1.234" } },
    { OptionEnum::eShortD, 'd' , {}              , 3, 3, "A short option that requires three arguments without defaults.", },
    { OptionEnum::eShortE, 'e' , {}              , 3, 3, "A short option that requires three arguments with defaults.", { "1.234", "abc", "???" } },
    { OptionEnum::eLongA , '\0' , "long-option-a", 0, 0, "A long option that requires no argument." },
    { OptionEnum::eLongB , '\0' , "long-option-b", 1, 1, "A long option that requires a single argument without a default.", },
    { OptionEnum::eLongC , '\0' , "long-option-c", 1, 1, "A long option that requires a single argument with a default.", { "bob" } },
    { OptionEnum::eLongD , '\0' , "long-option-d", 3, 3, "A long option that requires three arguments without defaults." },
    { OptionEnum::eLongE , '\0' , "long-option-e", 3, 3, "A long option that requires three arguments with defaults.", { "bob", "=-0", "1.e-16" } },
  };

  // Note that this also tests that en excess argument that *can* be interpreted
  // as a trailing argument is done so correctly.
  const char* argv[28]
  {
    { "exe" },
    { "-a" },
    { "-b" }, { "bbb" },
    { "-c" }, { "cCcC" },
    { "-d" }, { "9.87" }, { "iou" }, { ";'#" },
    { "-e" }, { "3.21" }, { "fed" }, { "!\"£" },
    { "--long-option-a" },
    { "--long-option-b" }, { "sue" },
    { "--long-option-c" }, { "paul" },
    { "--long-option-d" }, { "benny" }, { ",/." }, { "9.99f32" },
    { "--long-option-e" }, { "emmet" }, { "<>?" }, { ".123e-1" },
    { "foo" }
  };

  lb::options::ParsedOptions<OptionEnum> parsed;
  ASSERT_NO_THROW( parsed = goodOptions1.parse( sizeof(argv)/sizeof(argv[0]), const_cast<char**>( argv ) ) );
  EXPECT_EQ( parsed.executable, argv[0] );

  TestParsedOption<OptionEnum> testParsedOption{ parsed, "testGoodOptions1" };

  // Test by Key
  testParsedOption( OptionEnum::eShortA, 1, {} );
  testParsedOption( OptionEnum::eShortB, 1, { { "bbb" } } );
  testParsedOption( OptionEnum::eShortC, 1, { { "cCcC" } } );
  testParsedOption( OptionEnum::eShortD, 1, { { "9.87", "iou", ";'#" } } );
  testParsedOption( OptionEnum::eShortE, 1, { { "3.21", "fed", "!\"£" } } );
  testParsedOption( OptionEnum::eLongA, 1, {} );
  testParsedOption( OptionEnum::eLongB, 1, { { "sue" } } );
  testParsedOption( OptionEnum::eLongC, 1, { { "paul" } } );
  testParsedOption( OptionEnum::eLongD, 1, { { "benny", ",/.", "9.99f32" } } );
  testParsedOption( OptionEnum::eLongE, 1, { { "emmet", "<>?", ".123e-1" } } );
  ASSERT_EQ( parsed.trailingValues.size(), 1 );
  EXPECT_EQ( parsed.trailingValues.front(), argv[27] );

  // Test by position index
  testParsedOption( { {  1, OptionEnum::eShortA, 0 }
                    , {  2, OptionEnum::eShortB, 0 }
                    , {  4, OptionEnum::eShortC, 0 }
                    , {  6, OptionEnum::eShortD, 0 }
                    , { 10, OptionEnum::eShortE, 0 }
                    , { 14, OptionEnum::eLongA, 0 }
                    , { 15, OptionEnum::eLongB, 0 }
                    , { 17, OptionEnum::eLongC, 0 }
                    , { 19, OptionEnum::eLongD, 0 }
                    , { 23, OptionEnum::eLongE, 0 } } );
#ifdef DEBUG
  printParsedOptions<OptionEnum>( parsed );
#endif
}

void testGoodOptions2()
{
  const lb::options::Options<OptionEnum> goodOptions2
  {
    { OptionEnum::eShortA, 'a' , {}              , 0, 0, "A short option that requires no arguments." },
    { OptionEnum::eShortB, 'b' , {}              , 0, 1, "A short option that requires at most 1 argument without a default.", },
    { OptionEnum::eShortC, 'c' , {}              , 1, 2, "A short option that requires either 1 or 2 arguments with 1 default.", { "1.234" } },
    { OptionEnum::eShortD, 'd' , {}              , 0, 3, "A short option that requires at most 3 arguments with 3 defaults.", { "1.234", "abc", "???" } },
    { OptionEnum::eShortE, 'e' , {}              , 2, 3, "A short option that requires 2 or 3 arguments without defaults.",  },
    { OptionEnum::eLongA , '\0' , "long-option-a", 0, 0, "A long option that requires no argument." },
    { OptionEnum::eLongB , '\0' , "long-option-b", 0, 1, "A long option that requires at most 1 argument without a default.", },
    { OptionEnum::eLongC , '\0' , "long-option-c", 1, 2, "A long option that requires either 1 or 2 arguments with 1 default.", { "bob" } },
    { OptionEnum::eLongD , '\0' , "long-option-d", 0, 3, "A long option that requires at most 3 arguments with 3 defaults.", { "bob", "=-0", "1.e-16" } },
    { OptionEnum::eLongE , '\0' , "long-option-e", 2, 3, "A long option that requires 2 or 3 arguments without defaults." },
  };

  const char* argv[23]
  {
    { "/path/to/exe" },
    { "--long-option-c" }, { "paul" },
    { "-b" }, { "bbb" },
    { "-e" }, { "3.21" }, { "fed" }, { "!\"£" },
    { "--long-option-b" }, { "sue" },
    { "--long-option-e" }, { "emmet" }, { "<>?" }, { ".123e-1" },
    { "-d" }, { "9.87" }, { "iou" }, { ";'#" },
    { "--long-option-a" },
    { "-a" },
    { "foo" },
    { "bar" }
  };

  lb::options::ParsedOptions<OptionEnum> parsed;
  ASSERT_NO_THROW( parsed = goodOptions2.parse( sizeof(argv)/sizeof(argv[0]), const_cast<char**>( argv ) ) );
  EXPECT_EQ( parsed.executable, argv[0] );

  TestParsedOption<OptionEnum> testParsedOption{ parsed, "testGoodOptions2" };

  // Test by Key
  testParsedOption( OptionEnum::eShortA, 1, {} );
  testParsedOption( OptionEnum::eShortB, 1, { { "bbb" } } );
  testParsedOption( OptionEnum::eShortC, 1, { { "1.234" } } );
  testParsedOption( OptionEnum::eShortD, 1, { { "9.87", "iou", ";'#" } } );
  testParsedOption( OptionEnum::eShortE, 1, { { "3.21", "fed", "!\"£" } } );
  testParsedOption( OptionEnum::eLongA, 1, {} );
  testParsedOption( OptionEnum::eLongB, 1, { { "sue" } } );
  testParsedOption( OptionEnum::eLongC, 1, { { "paul" } } );
  testParsedOption( OptionEnum::eLongD, 1, { { "bob", "=-0", "1.e-16" } } );
  testParsedOption( OptionEnum::eLongE, 1, { { "emmet", "<>?", ".123e-1" } } );
  ASSERT_EQ( parsed.trailingValues.size(), 2 );
  EXPECT_EQ( parsed.trailingValues.front(), argv[21] );
  EXPECT_EQ( parsed.trailingValues.back() , argv[22] );

  // Test by position index
  testParsedOption( { {  1, OptionEnum::eLongC, 0 }
                    , {  3, OptionEnum::eShortB, 0 }
                    , {  5, OptionEnum::eShortE, 0 }
                    , {  9, OptionEnum::eLongB, 0 }
                    , { 11, OptionEnum::eLongE, 0 }
                    , { 15, OptionEnum::eShortD, 0 }
                    , { 19, OptionEnum::eLongA, 0 }
                    , { 20, OptionEnum::eShortA, 0 } } );
#ifdef DEBUG
  printParsedOptions<OptionEnum>( parsed );
#endif
}

void testGoodOptions3()
{
  // Test multiple occurences
  const lb::options::Options<OptionEnum> goodOptions3
  {
    { OptionEnum::eShortA, 'a' , {}              , 0, 0, "A short option that requires no arguments." },
    { OptionEnum::eShortB, 'b' , {}              , 1, 1, "A short option that requires a single argument.", },
    { OptionEnum::eShortC, 'c' , {}              , 2, 4, "A short option that requires btween 2 and 4 arguments." },
    { OptionEnum::eLongA , '\0' , "long-option-a", 0, 0, "A long option that requires no argument." },
    { OptionEnum::eLongB , '\0' , "long-option-b", 1, 1, "A long option that requires a single argument.", },
    { OptionEnum::eLongC , '\0' , "long-option-c", 1, 2, "A long option that requires either 1 or 2 arguments." },
  };

  const char* argv[28]
  {
    { "exe" },
    { "-a" },
    { "-b" }, { "bbb" },
    { "-c" }, { "cCcC" }, { "tt" }, { "9876" },
    { "-a" },
    { "-b" }, { "3.21" },
    { "-c" }, { "cCcCcC" }, { "345.21" },
    { "-a" },
    { "--long-option-a" },
    { "--long-option-b" }, { "sue" },
    { "--long-option-c" }, { "paul" },
    { "--long-option-c" }, { "benny" }, { ",/." },
    { "--long-option-b" }, { "emmet" },
    { "--long-option-a" },
    { "--long-option-c" }, { "lucy" },
  };

  lb::options::ParsedOptions<OptionEnum> parsed;
  ASSERT_NO_THROW( parsed = goodOptions3.parse( sizeof(argv)/sizeof(argv[0]), const_cast<char**>( argv ) ) );
  EXPECT_EQ( parsed.executable, argv[0] );

  TestParsedOption<OptionEnum> testParsedOption{ parsed, "testGoodOptions3" };

  // Test by Key
  testParsedOption( OptionEnum::eShortA, 3, {} );
  testParsedOption( OptionEnum::eShortB, 2, { { "bbb" }, { "3.21" } } );
  testParsedOption( OptionEnum::eShortC, 2, { { "cCcC", "tt" , "9876" }, { "cCcCcC", "345.21" } } );
  testParsedOption( OptionEnum::eLongA, 2, {} );
  testParsedOption( OptionEnum::eLongB, 2, { { "sue" }, { "emmet" } } );
  testParsedOption( OptionEnum::eLongC, 3, { { "paul" }, { "benny", ",/." }, { "lucy" } } );
  ASSERT_EQ( parsed.trailingValues.size(), 0 );

  // Test by position index
  testParsedOption( { {  1, OptionEnum::eShortA, 0 }
                    , {  2, OptionEnum::eShortB, 0 }
                    , {  4, OptionEnum::eShortC, 0 }
                    , {  8, OptionEnum::eShortA, 1 }
                    , {  9, OptionEnum::eShortB, 1 }
                    , { 11, OptionEnum::eShortC, 1 }
                    , { 14, OptionEnum::eShortA, 2 }
                    , { 15, OptionEnum::eLongA, 0 }
                    , { 16, OptionEnum::eLongB, 0 }
                    , { 18, OptionEnum::eLongC, 0 }
                    , { 20, OptionEnum::eLongC, 1 }
                    , { 23, OptionEnum::eLongB, 1 }
                    , { 25, OptionEnum::eLongA, 1 }
                    , { 26, OptionEnum::eLongC, 2 } } );
#ifdef DEBUG
  printParsedOptions<OptionEnum>( parsed );
#endif
}

void testGoodOptions4()
{
  // Redo good options 2 test but with a different key and custom hash.
  const lb::options::Options<CustomKey, CustomHash> goodOptions4
  {
    { { "short", "a" }, 'a' , {}              , 0, 0, "A short option that requires no arguments." },
    { { "short", "b" }, 'b' , {}              , 0, 1, "A short option that requires at most 1 argument without a default.", },
    { { "short", "c" }, 'c' , {}              , 1, 2, "A short option that requires either 1 or 2 arguments with 1 default.", { "1.234" } },
    { { "short", "d" }, 'd' , {}              , 0, 3, "A short option that requires at most 3 arguments with 3 defaults.", { "1.234", "abc", "???" } },
    { { "short", "e" }, 'e' , {}              , 2, 3, "A short option that requires 2 or 3 arguments without defaults.",  },
    { { "long", "a" } , '\0' , "long-option-a", 0, 0, "A long option that requires no argument." },
    { { "long", "b" } , '\0' , "long-option-b", 0, 1, "A long option that requires at most 1 argument without a default.", },
    { { "long", "c" } , '\0' , "long-option-c", 1, 2, "A long option that requires either 1 or 2 arguments with 1 default.", { "bob" } },
    { { "long", "d" } , '\0' , "long-option-d", 0, 3, "A long option that requires at most 3 arguments with 3 defaults.", { "bob", "=-0", "1.e-16" } },
    { { "long", "e" } , '\0' , "long-option-e", 2, 3, "A long option that requires 2 or 3 arguments without defaults." },
  };

  const char* argv[23]
  {
    { "/path/to/exe" },
    { "--long-option-c" }, { "paul" },
    { "-b" }, { "bbb" },
    { "-e" }, { "3.21" }, { "fed" }, { "!\"£" },
    { "--long-option-b" }, { "sue" },
    { "--long-option-e" }, { "emmet" }, { "<>?" }, { ".123e-1" },
    { "-d" }, { "9.87" }, { "iou" }, { ";'#" },
    { "--long-option-a" },
    { "-a" },
    { "foo" },
    { "bar" }
  };

  lb::options::ParsedOptions<CustomKey, CustomHash> parsed;
  ASSERT_NO_THROW( parsed = goodOptions4.parse( sizeof(argv)/sizeof(argv[0]), const_cast<char**>( argv ) ) );
  EXPECT_EQ( parsed.executable, argv[0] );

  TestParsedOption<CustomKey, CustomHash> testParsedOption{ parsed, "testGoodOptions4" };

  // Test by Key
  testParsedOption( { "short", "a" }, 1, {} );
  testParsedOption( { "short", "b" }, 1, { { "bbb" } } );
  testParsedOption( { "short", "c" }, 1, { { "1.234" } } );
  testParsedOption( { "short", "d" }, 1, { { "9.87", "iou", ";'#" } } );
  testParsedOption( { "short", "e" }, 1, { { "3.21", "fed", "!\"£" } } );
  testParsedOption( { "long", "a" }, 1, {} );
  testParsedOption( { "long", "b" }, 1, { { "sue" } } );
  testParsedOption( { "long", "c" }, 1, { { "paul" } } );
  testParsedOption( { "long", "d" }, 1, { { "bob", "=-0", "1.e-16" } } );
  testParsedOption( { "long", "e" }, 1, { { "emmet", "<>?", ".123e-1" } } );
  ASSERT_EQ( parsed.trailingValues.size(), 2 );
  EXPECT_EQ( parsed.trailingValues.front(), argv[21] );
  EXPECT_EQ( parsed.trailingValues.back() , argv[22] );

  // Test by position index
  testParsedOption( { {  1, { "long" , "c" }, 0 }
                    , {  3, { "short", "b" }, 0 }
                    , {  5, { "short", "e" }, 0 }
                    , {  9, { "long" , "b" }, 0 }
                    , { 11, { "long" , "e" }, 0 }
                    , { 15, { "short", "d" }, 0 }
                    , { 19, { "long" , "a" }, 0 }
                    , { 20, { "short", "a" }, 0 } } );
#ifdef DEBUG
  printParsedOptions<CustomKey, CustomHash>( parsed );
#endif
}

void testGoodOptions5()
{
  // Repeat test 1 but don't allow trailing values
  const lb::options::Options<OptionEnum> goodOptions5
  {
    {
      { OptionEnum::eShortA, 'a' , {}              , 0, 0, "A short option that requires no arguments." },
      { OptionEnum::eShortB, 'b' , {}              , 1, 1, "A short option that requires a single argument without a default.", },
      { OptionEnum::eShortC, 'c' , {}              , 1, 1, "A short option that requires a single argument with a default.", { "1.234" } },
      { OptionEnum::eShortD, 'd' , {}              , 3, 3, "A short option that requires three arguments without defaults.", },
      { OptionEnum::eShortE, 'e' , {}              , 3, 3, "A short option that requires three arguments with defaults.", { "1.234", "abc", "???" } },
      { OptionEnum::eLongA , '\0' , "long-option-a", 0, 0, "A long option that requires no argument." },
      { OptionEnum::eLongB , '\0' , "long-option-b", 1, 1, "A long option that requires a single argument without a default.", },
      { OptionEnum::eLongC , '\0' , "long-option-c", 1, 1, "A long option that requires a single argument with a default.", { "bob" } },
      { OptionEnum::eLongD , '\0' , "long-option-d", 3, 3, "A long option that requires three arguments without defaults." },
      { OptionEnum::eLongE , '\0' , "long-option-e", 3, 3, "A long option that requires three arguments with defaults.", { "bob", "=-0", "1.e-16" } },
    },
    { false } // don't allow trailing values
  };

  const char* argv[27]
  {
    { "exe" },
    { "-a" },
    { "-b" }, { "bbb" },
    { "-c" }, { "cCcC" },
    { "-d" }, { "9.87" }, { "iou" }, { ";'#" },
    { "-e" }, { "3.21" }, { "fed" }, { "!\"£" },
    { "--long-option-a" },
    { "--long-option-b" }, { "sue" },
    { "--long-option-c" }, { "paul" },
    { "--long-option-d" }, { "benny" }, { ",/." }, { "9.99f32" },
    { "--long-option-e" }, { "emmet" }, { "<>?" }, { ".123e-1" }
  };

  lb::options::ParsedOptions<OptionEnum> parsed;
  ASSERT_NO_THROW( parsed = goodOptions5.parse( sizeof(argv)/sizeof(argv[0]), const_cast<char**>( argv ) ) );
  EXPECT_EQ( parsed.executable, argv[0] );

  TestParsedOption<OptionEnum> testParsedOption{ parsed, "testGoodOptions1" };

  // Test by Key
  testParsedOption( OptionEnum::eShortA, 1, {} );
  testParsedOption( OptionEnum::eShortB, 1, { { "bbb" } } );
  testParsedOption( OptionEnum::eShortC, 1, { { "cCcC" } } );
  testParsedOption( OptionEnum::eShortD, 1, { { "9.87", "iou", ";'#" } } );
  testParsedOption( OptionEnum::eShortE, 1, { { "3.21", "fed", "!\"£" } } );
  testParsedOption( OptionEnum::eLongA, 1, {} );
  testParsedOption( OptionEnum::eLongB, 1, { { "sue" } } );
  testParsedOption( OptionEnum::eLongC, 1, { { "paul" } } );
  testParsedOption( OptionEnum::eLongD, 1, { { "benny", ",/.", "9.99f32" } } );
  testParsedOption( OptionEnum::eLongE, 1, { { "emmet", "<>?", ".123e-1" } } );
  ASSERT_EQ( parsed.trailingValues.size(), 0 );

  // Test by position index
  testParsedOption( { {  1, OptionEnum::eShortA, 0 }
                    , {  2, OptionEnum::eShortB, 0 }
                    , {  4, OptionEnum::eShortC, 0 }
                    , {  6, OptionEnum::eShortD, 0 }
                    , { 10, OptionEnum::eShortE, 0 }
                    , { 14, OptionEnum::eLongA, 0 }
                    , { 15, OptionEnum::eLongB, 0 }
                    , { 17, OptionEnum::eLongC, 0 }
                    , { 19, OptionEnum::eLongD, 0 }
                    , { 23, OptionEnum::eLongE, 0 } } );
#ifdef DEBUG
  printParsedOptions<OptionEnum>( parsed );
#endif
}

bool isSame( const lb::options::OptionDefinition& lhs
           , const lb::options::OptionDefinition& rhs )
{
  return ( lhs.s == rhs.s )
      && ( lhs.l == rhs.l )
      && ( lhs.maxNumValues == rhs.maxNumValues )
      && ( lhs.minNumValues == rhs.minNumValues )
      && ( lhs.description == rhs.description )
      && ( lhs.defaultValues == rhs.defaultValues );
}

void testGetDefinition()
{
  using OptionDef = lb::options::OptionDefinition;
  const OptionDef optionDefShortA{ 'a' , {}              , 0, 0, "A short option that requires no arguments." };
  const OptionDef optionDefShortB{ 'b' , {}              , 1, 1, "A short option that requires a single argument without a default." };
  const OptionDef optionDefShortC{ 'c' , {}              , 1, 1, "A short option that requires a single argument with a default.", { "1.234" } };
  const OptionDef optionDefShortD{ 'd' , {}              , 3, 3, "A short option that requires three arguments without defaults.", };
  const OptionDef optionDefLongA{ '\0' , "long-option-a", 0, 0, "A long option that requires no argument." };
  const OptionDef optionDefLongC{ '\0' , "long-option-c", 1, 1, "A long option that requires a single argument with a default.", { "bob" } };
  const OptionDef optionDefLongD{ '\0' , "long-option-d", 3, 3, "A long option that requires three arguments without defaults." };
  const OptionDef optionDefLongE{ '\0' , "long-option-e", 3, 3, "A long option that requires three arguments with defaults.", { "bob", "=-0", "1.e-16" } };

  const lb::options::Options<OptionEnum> options
  {
    { OptionEnum::eShortA, 'a' , {}              , 0, 0, "A short option that requires no arguments." },
    { OptionEnum::eShortB, 'b' , {}              , 1, 1, "A short option that requires a single argument without a default.", },
    { OptionEnum::eShortC, 'c' , {}              , 1, 1, "A short option that requires a single argument with a default.", { "1.234" } },
    { OptionEnum::eShortD, 'd' , {}              , 3, 3, "A short option that requires three arguments without defaults.", },
    { OptionEnum::eLongA , '\0' , "long-option-a", 0, 0, "A long option that requires no argument." },
    { OptionEnum::eLongC , '\0' , "long-option-c", 1, 1, "A long option that requires a single argument with a default.", { "bob" } },
    { OptionEnum::eLongD , '\0' , "long-option-d", 3, 3, "A long option that requires three arguments without defaults." },
    { OptionEnum::eLongE , '\0' , "long-option-e", 3, 3, "A long option that requires three arguments with defaults.", { "bob", "=-0", "1.e-16" } },
  };

  ASSERT_NO_THROW( options.getDefinition( OptionEnum::eShortA ) );
  ASSERT_NO_THROW( options.getDefinition( OptionEnum::eShortB ) );
  ASSERT_NO_THROW( options.getDefinition( OptionEnum::eShortC ) );
  ASSERT_NO_THROW( options.getDefinition( OptionEnum::eShortD ) );
  ASSERT_THROW   ( options.getDefinition( OptionEnum::eShortE ), std::runtime_error );
  ASSERT_NO_THROW( options.getDefinition( OptionEnum::eLongA ) );
  ASSERT_THROW   ( options.getDefinition( OptionEnum::eLongB ), std::runtime_error );
  ASSERT_NO_THROW( options.getDefinition( OptionEnum::eLongC ) );
  ASSERT_NO_THROW( options.getDefinition( OptionEnum::eLongD ) );
  ASSERT_NO_THROW( options.getDefinition( OptionEnum::eLongE ) );

  EXPECT_TRUE( isSame( options.getDefinition( OptionEnum::eShortA ), optionDefShortA ) );
  EXPECT_TRUE( isSame( options.getDefinition( OptionEnum::eShortB ), optionDefShortB ) );
  EXPECT_TRUE( isSame( options.getDefinition( OptionEnum::eShortC ), optionDefShortC ) );
  EXPECT_TRUE( isSame( options.getDefinition( OptionEnum::eShortD ), optionDefShortD ) );
  EXPECT_TRUE( isSame( options.getDefinition( OptionEnum::eLongA ), optionDefLongA ) );
  EXPECT_TRUE( isSame( options.getDefinition( OptionEnum::eLongC ), optionDefLongC ) );
  EXPECT_TRUE( isSame( options.getDefinition( OptionEnum::eLongD ), optionDefLongD ) );
  EXPECT_TRUE( isSame( options.getDefinition( OptionEnum::eLongE ), optionDefLongE ) );
//  ASSERT_NO_THROW( options.getDefinition( OptionEnum::eShortB ) );
//  ASSERT_NO_THROW( options.getDefinition( OptionEnum::eShortC ) );
//  ASSERT_NO_THROW( options.getDefinition( OptionEnum::eShortD ) );
//  ASSERT_THROW   ( options.getDefinition( OptionEnum::eShortE ), std::runtime_error );
//  ASSERT_NO_THROW( options.getDefinition( OptionEnum::eLongA ) );
//  ASSERT_THROW   ( options.getDefinition( OptionEnum::eLongB ), std::runtime_error );
//  ASSERT_NO_THROW( options.getDefinition( OptionEnum::eLongC ) );
//  ASSERT_NO_THROW( options.getDefinition( OptionEnum::eLongD ) );
//  ASSERT_NO_THROW( options.getDefinition( OptionEnum::eLongE ) );
}

void testIsPresent()
{
  const lb::options::Options<OptionEnum> isPresentOptions
  {
    { OptionEnum::eShortA, 'a' , {}              , 0, 0, "A short option that requires no arguments." },
    { OptionEnum::eShortB, 'b' , {}              , 1, 1, "A short option that requires a single argument without a default.", },
    { OptionEnum::eShortC, 'c' , {}              , 1, 1, "A short option that requires a single argument with a default.", { "1.234" } },
    { OptionEnum::eShortD, 'd' , {}              , 3, 3, "A short option that requires three arguments without defaults.", },
    { OptionEnum::eShortE, 'e' , {}              , 3, 3, "A short option that requires three arguments with defaults.", { "1.234", "abc", "???" } },
    { OptionEnum::eLongA , '\0' , "long-option-a", 0, 0, "A long option that requires no argument." },
    { OptionEnum::eLongB , '\0' , "long-option-b", 1, 1, "A long option that requires a single argument without a default.", },
    { OptionEnum::eLongC , '\0' , "long-option-c", 1, 1, "A long option that requires a single argument with a default.", { "bob" } },
    { OptionEnum::eLongD , '\0' , "long-option-d", 3, 3, "A long option that requires three arguments without defaults." },
    { OptionEnum::eLongE , '\0' , "long-option-e", 3, 3, "A long option that requires three arguments with defaults.", { "bob", "=-0", "1.e-16" } },
  };

  const char* argv[18]
  {
    { "exe" },
    { "-a" },
    { "-b" }, { "bbb" },
    { "-d" }, { "9.87" }, { "iou" }, { ";'#" },
    { "--long-option-a" },
    { "-b" }, { "bbb2" },
    { "--long-option-b" }, { "sue" },
    { "--long-option-d" }, { "benny" }, { ",/." }, { "9.99f32" },
    { "--long-option-a" },
  };

  lb::options::ParsedOptions<OptionEnum> parsed;
  ASSERT_NO_THROW( parsed = isPresentOptions.parse( sizeof(argv)/sizeof(argv[0]), const_cast<char**>( argv ) ) );
  EXPECT_EQ( parsed.executable, argv[0] );
  EXPECT_TRUE( parsed.isPresent( OptionEnum::eShortA ) );
  EXPECT_TRUE( parsed.isPresent( OptionEnum::eShortB ) );
  EXPECT_TRUE( parsed.isPresent( OptionEnum::eShortC ) );
  EXPECT_TRUE( parsed.isPresent( OptionEnum::eShortD ) );
  EXPECT_TRUE( parsed.isPresent( OptionEnum::eShortE ) );
  EXPECT_TRUE( parsed.isPresent( OptionEnum::eLongA ) );
  EXPECT_TRUE( parsed.isPresent( OptionEnum::eLongB ) );
  EXPECT_TRUE( parsed.isPresent( OptionEnum::eLongC ) );
  EXPECT_TRUE( parsed.isPresent( OptionEnum::eLongD ) );
  EXPECT_TRUE( parsed.isPresent( OptionEnum::eLongE ) );
}

void testIsNotPresent()
{
  const lb::options::Options<OptionEnum> isNotPresentOptions
  {
  };

  const char* argv[1]
  {
    { "exe" },
  };

  lb::options::ParsedOptions<OptionEnum> parsed;
  ASSERT_NO_THROW( parsed = isNotPresentOptions.parse( sizeof(argv)/sizeof(argv[0]), const_cast<char**>( argv ) ) );
  EXPECT_EQ( parsed.executable, argv[0] );
  EXPECT_FALSE( parsed.isPresent( OptionEnum::eShortA ) );
  EXPECT_FALSE( parsed.isPresent( OptionEnum::eShortB ) );
  EXPECT_FALSE( parsed.isPresent( OptionEnum::eShortC ) );
  EXPECT_FALSE( parsed.isPresent( OptionEnum::eShortD ) );
  EXPECT_FALSE( parsed.isPresent( OptionEnum::eShortE ) );
  EXPECT_FALSE( parsed.isPresent( OptionEnum::eLongA ) );
  EXPECT_FALSE( parsed.isPresent( OptionEnum::eLongB ) );
  EXPECT_FALSE( parsed.isPresent( OptionEnum::eLongC ) );
  EXPECT_FALSE( parsed.isPresent( OptionEnum::eLongD ) );
  EXPECT_FALSE( parsed.isPresent( OptionEnum::eLongE ) );
}

void testGetLatestValue()
{
  const lb::options::Options<OptionEnum> isPresentOptions
  {
    { OptionEnum::eShortA, 'a' , {}              , 0, 0, "A short option that requires no arguments." },
    { OptionEnum::eShortB, 'b' , {}              , 1, 1, "A short option that requires a single argument without a default.", },
    { OptionEnum::eShortC, 'c' , {}              , 1, 1, "A short option that requires a single argument with a default.", { "1.234" } },
    { OptionEnum::eShortD, 'd' , {}              , 3, 3, "A short option that requires three arguments without defaults.", },
    { OptionEnum::eShortE, 'e' , {}              , 0, 3, "A short option that requires at most three arguments with defaults.", { "1.234", "abc", "???" } },
    { OptionEnum::eLongA , '\0' , "long-option-a", 0, 0, "A long option that requires no argument." },
    { OptionEnum::eLongB , '\0' , "long-option-b", 1, 1, "A long option that requires a single argument without a default.", },
    { OptionEnum::eLongC , '\0' , "long-option-c", 1, 1, "A long option that requires a single argument with a default.", { "bob" } },
    { OptionEnum::eLongD , '\0' , "long-option-d", 3, 3, "A long option that requires three arguments without defaults." },
    { OptionEnum::eLongE , '\0' , "long-option-e", 3, 3, "A long option that requires three arguments with defaults.", { "bob", "=-0", "1.e-16" } },
  };

  const char* argv[23]
  {
    { "exe" },
    { "-a" },
    { "-b" }, { "bbb" },
    { "-d" }, { "9.87" }, { "iou" }, { ";'#" },
    { "--long-option-a" },
    { "-e" },
    { "--long-option-a" },
    { "--long-option-b" }, { "sue" },
    { "-e" }, { "1.01" }, { "xyz" }, { "\\|`" },
    { "--long-option-d" }, { "benny" }, { ",/." }, { "9.99f32" },
    { "-b" }, { "bbb2" },
  };

  lb::options::ParsedOptions<OptionEnum> parsed;
  ASSERT_NO_THROW( parsed = isPresentOptions.parse( sizeof(argv)/sizeof(argv[0]), const_cast<char**>( argv ) ) );
  EXPECT_EQ( parsed.executable, argv[0] );
  EXPECT_EQ( parsed.getLatestValue( OptionEnum::eShortA ), "" );
  EXPECT_EQ( parsed.getLatestValue( OptionEnum::eShortB ), "bbb2" );
  EXPECT_EQ( parsed.getLatestValue( OptionEnum::eShortC ), "1.234" );
  EXPECT_EQ( parsed.getLatestValue( OptionEnum::eShortD ), ";'#" );
  EXPECT_EQ( parsed.getLatestValue( OptionEnum::eShortE ), "\\|`" );
  EXPECT_EQ( parsed.getLatestValue( OptionEnum::eLongA ), "" );
  EXPECT_EQ( parsed.getLatestValue( OptionEnum::eLongB ), "sue" );
  EXPECT_EQ( parsed.getLatestValue( OptionEnum::eLongC ), "bob" );
  EXPECT_EQ( parsed.getLatestValue( OptionEnum::eLongD ), "9.99f32" );
  EXPECT_EQ( parsed.getLatestValue( OptionEnum::eLongE ), "1.e-16" );
}

TEST(Options, EnumClassKey)
{
  testMisconfiguredOptions1();
  testMisconfiguredOptions2s();
  testMisconfiguredOptions2l();
  testMisconfiguredOptions2sc();
  testMisconfiguredOptions2lc();
  testMisconfiguredOptions3s();
  testMisconfiguredOptions3l();
  testMisconfiguredOptions4s();
  testMisconfiguredOptions4l();
  testMisconfiguredOptions5();
  testMisconfiguredOptions5c();

  testUnknownOptions1s();
  testUnknownOptions1l();

  testBadOptions1s();
  testBadOptions1l();
  testBadOptions2s();
  testBadOptions2l();
  testBadOptions3s();
  testBadOptions3l();
  testBadOptions4s();
  testBadOptions4l();
  testBadOptions5s();
  testBadOptions5l();
  testBadOptions6s();
  testBadOptions6l();

  testDefaultedOptions1s();
  testDefaultedOptions1l();
  testDefaultedOptions2s();
  testDefaultedOptions2l();
  testDefaultedOptions3s();
  testDefaultedOptions3l();

  testGoodOptions1();
  testGoodOptions2();
  testGoodOptions3();
  testGoodOptions4();
  testGoodOptions5();

  testGetDefinition();
  testIsPresent();
  testIsNotPresent();
  testGetLatestValue();
}
