/* ============================================================================
 * Copyright (c) 2009-2019 BlueQuartz Software, LLC
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
 * contributors may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace UnitTestSupport
{
static std::string CurrentMethod("");
static int numTestsPass = 0;
static int numTestFailed = 0;
static int numTests = 0;
constexpr size_t textWidth = 120;

static const std::string Passed("PASSED");
static const std::string Failed("FAILED");
} // namespace UnitTestSupport

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class TestException : public std::exception
{
public:
  /**
   * @brief
   * @param what
   * @param file
   * @param lineNumber
   */
  TestException(const std::string& what, const std::string& filename, int lineNumber)
  : m_Message(what)
  , m_FileName(filename)
  , m_LineNumber(lineNumber)
  {
    updateWhat();
  }

  /**
   * @brief Copy Constructor
   */
  TestException(const TestException& te)
  {
    m_Message = te.m_Message;
    m_FileName = te.m_FileName;
    m_LineNumber = te.m_LineNumber;
    updateWhat();
  }

  virtual ~TestException() noexcept
  {
  }

  void operator=(const TestException&) = delete; // Move assignment Not Implemented

  /**
   * @brief Over ride from base class
   */
  virtual const char* what() const noexcept
  {
    return m_What.c_str();
  }

  void setMessage(const std::string& m)
  {
    m_Message = m;
    updateWhat();
  }
  std::string getMessage()
  {
    return m_Message;
  }
  std::string getMessage() const
  {
    return m_Message;
  }

  void setFileName(const std::string& fn)
  {
    m_FileName = fn;
    updateWhat();
  }
  std::string getFileName()
  {
    return m_FileName;
  }
  std::string getFileName() const
  {
    return m_FileName;
  }

  void setLineNumber(int ln)
  {
    m_LineNumber = ln;
    updateWhat();
  }
  int getLineNumber()
  {
    return m_LineNumber;
  }
  int getLineNumber() const
  {
    return m_LineNumber;
  }

protected:
  TestException()
  {
    updateWhat();
  }

  void updateWhat()
  {
    std::stringstream ss;
    ss << "    Reason: " << m_Message << std::endl;
    ss << "    File:   " << m_FileName << std::endl;
    ss << "    Line:   " << m_LineNumber;

    m_What = ss.str();
  }

private:
  std::string m_Message;
  std::string m_FileName;
  int m_LineNumber;

  std::string m_What;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestPassed(const std::string& test)
{
  std::cout << test << std::setw(UnitTestSupport::textWidth - test.size()) << UnitTestSupport::Passed << std::endl;
  UnitTestSupport::numTestsPass++;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestFailed(const std::string& test)
{
  std::cout << test << std::setw(UnitTestSupport::textWidth - test.size()) << UnitTestSupport::Failed << std::endl;
  UnitTestSupport::numTestFailed++;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
#define INFINITYCHECK 1
#define SIGNCHECK 1

bool AlmostEqualUlpsFinal(float A, float B, int maxUlps)
{
  // There are several optional checks that you can do, depending
  // on what behavior you want from your floating point comparisons.
  // These checks should not be necessary and they are included
  // mainly for completeness.

#ifdef INFINITYCHECK
  // If A or B are infinity (positive or negative) then
  // only return true if they are exactly equal to each other -
  // that is, if they are both infinities of the same sign.
  // This check is only needed if you will be generating
  // infinities and you don't want them 'close' to numbers
  // near FLT_MAX.
  if(std::isinf(A) || std::isinf(B))
  {
    return A == B;
  }
#endif

#ifdef NANCHECK
  // If A or B are a NAN, return false. NANs are equal to nothing,
  // not even themselves.
  // This check is only needed if you will be generating NANs
  // and you use a maxUlps greater than 4 million or you want to
  // ensure that a NAN does not equal itself.
  if(std::isnan(A) || std::isnan(B))
  {
    return false;
  }
#endif

#ifdef SIGNCHECK
  // After adjusting floats so their representations are lexicographically
  // ordered as twos-complement integers a very small positive number
  // will compare as 'close' to a very small negative number. If this is
  // not desireable, and if you are on a platform that supports
  // subnormals (which is the only place the problem can show up) then
  // you need this check.
  // The check for A == B is because zero and negative zero have different
  // signs but are equal to each other.
  if(std::signbit(A) != std::signbit(B))
  {
    return A == B;
  }
#endif

  int aInt = reinterpret_cast<const int&>(A);
  // Make aInt lexicographically ordered as a twos-complement int
  if(aInt < 0)
  {
    aInt = 0x80000000 - aInt;
  }
  // Make bInt lexicographically ordered as a twos-complement int
  int bInt = reinterpret_cast<const int&>(B);
  if(bInt < 0)
  {
    bInt = 0x80000000 - bInt;
  }

  // Now we can compare aInt and bInt to find out how far apart A and B
  // are.
  int intDiff = std::abs(aInt - bInt);
  if(intDiff <= maxUlps)
  {
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
// Developer Used Macros
// -----------------------------------------------------------------------------
#define H5SUPPORT_TEST_FAILED(P)                                                                                                                                                                       \
  {                                                                                                                                                                                                    \
    H5SUPPORT_TEST_THROW_EXCEPTION(s)                                                                                                                                                                  \
  }

#define H5SUPPORT_REQUIRE(P)                                                                                                                                                                           \
  {                                                                                                                                                                                                    \
    bool b = (P);                                                                                                                                                                                      \
    if((b) == (false))                                                                                                                                                                                 \
    {                                                                                                                                                                                                  \
      std::string s("Your test required the following\n            '");                                                                                                                                \
      s = s.append(#P).append("'\n             but this condition was not met.");                                                                                                                      \
      H5SUPPORT_TEST_THROW_EXCEPTION(s)                                                                                                                                                                \
    }                                                                                                                                                                                                  \
  }

#define H5SUPPORT_REQUIRED(L, Q, R)                                                                                                                                                                    \
  {                                                                                                                                                                                                    \
    std::stringstream ss;                                                                                                                                                                              \
    bool b = (L Q R);                                                                                                                                                                                  \
    if((b) == (false))                                                                                                                                                                                 \
    {                                                                                                                                                                                                  \
      ss << "Your test required the following\n            '";                                                                                                                                         \
      ss << #L << " " << #Q << " " << #R << "' but this condition was not met.\n";                                                                                                                     \
      ss << "            " << #L << " = " << L << "\n";                                                                                                                                                \
      ss << "            " << #R << " = " << R << "\n";                                                                                                                                                \
      H5SUPPORT_TEST_THROW_EXCEPTION(ss.str())                                                                                                                                                         \
    }                                                                                                                                                                                                  \
  }

#define H5SUPPORT_REQUIRED_PTR(L, Q, P)                                                                                                                                                                \
  {                                                                                                                                                                                                    \
    std::stringstream ss;                                                                                                                                                                              \
    bool b = (L Q P);                                                                                                                                                                                  \
    if((b) == (false))                                                                                                                                                                                 \
    {                                                                                                                                                                                                  \
      ss << "Your test required the following\n            '";                                                                                                                                         \
      ss << #L << " " << #Q << " " << #P << "' but this condition was not met.\n";                                                                                                                     \
      ss << "            " << #L << " = " << L << "\n";                                                                                                                                                \
      ss << "            " << #P << " = \n";                                                                                                                                                           \
      H5SUPPORT_TEST_THROW_EXCEPTION(ss.str())                                                                                                                                                         \
    }                                                                                                                                                                                                  \
  }

#define H5SUPPORT_REQUIRE_NE(L, R)                                                                                                                                                                     \
  if((L) == (R))                                                                                                                                                                                       \
  {                                                                                                                                                                                                    \
    std::stringstream ss;                                                                                                                                                                              \
    ss << "Your test required the following\n            '";                                                                                                                                           \
    ss << #L << " != " << #R << "'\n             but this condition was not met.\n";                                                                                                                   \
    ss << "             " << L << "==" << R;                                                                                                                                                           \
    H5SUPPORT_TEST_THROW_EXCEPTION(ss.str())                                                                                                                                                           \
  }

#define H5SUPPORT_REQUIRE_EQUAL(L, R)                                                                                                                                                                  \
  if((L) != (R))                                                                                                                                                                                       \
  {                                                                                                                                                                                                    \
    std::stringstream ss;                                                                                                                                                                              \
    ss << "Your test required the following\n            '";                                                                                                                                           \
    ss << #L << " == " << #R << "'\n             but this condition was not met.\n";                                                                                                                   \
    ss << "             " << L << "==" << R;                                                                                                                                                           \
    H5SUPPORT_TEST_THROW_EXCEPTION(ss.str())                                                                                                                                                           \
  }

#define H5SUPPORT_COMPARE_FLOATS(L, R, Ulps)                                                                                                                                                           \
  if(false == AlmostEqualUlpsFinal((L), (R), Ulps))                                                                                                                                                    \
  {                                                                                                                                                                                                    \
    std::stringstream ss;                                                                                                                                                                              \
    ss << "Your test required the following\n            '";                                                                                                                                           \
    ss << "AlmostEqualUlpsFinal(" << #L << ", " << #R << ", " << #Ulps << "'\n             but this condition was not met with MaxUlps=" << Ulps << "\n";                                              \
    ss << "             " << L << "==" << R;                                                                                                                                                           \
    H5SUPPORT_TEST_THROW_EXCEPTION(ss.str())                                                                                                                                                           \
  }

#define H5SUPPORT_TEST_POINTER(L, Q, R)                                                                                                                                                                \
  {                                                                                                                                                                                                    \
    std::stringstream ss;                                                                                                                                                                              \
    bool b = (L Q R);                                                                                                                                                                                  \
    if((b) == (false))                                                                                                                                                                                 \
    {                                                                                                                                                                                                  \
      ss << "Your test required the following\n            '";                                                                                                                                         \
      ss << #L << " " << #Q << " " << #R << "' but this condition was not met.\n";                                                                                                                     \
      ss << "            " << #L << " = ";                                                                                                                                                             \
      if(L)                                                                                                                                                                                            \
      {                                                                                                                                                                                                \
        ss << reinterpret_cast<void*>(L);                                                                                                                                                              \
      }                                                                                                                                                                                                \
      else                                                                                                                                                                                             \
      {                                                                                                                                                                                                \
        ss << "Left side was nullptr";                                                                                                                                                                 \
      }                                                                                                                                                                                                \
      ss << "\n";                                                                                                                                                                                      \
      ss << "            " << #R << " = ";                                                                                                                                                             \
      if(R)                                                                                                                                                                                            \
      {                                                                                                                                                                                                \
        ss << reinterpret_cast<void*>(R);                                                                                                                                                              \
      }                                                                                                                                                                                                \
      else                                                                                                                                                                                             \
      {                                                                                                                                                                                                \
        ss << "Right Side was nullptr";                                                                                                                                                                \
      }                                                                                                                                                                                                \
      ss << "\n";                                                                                                                                                                                      \
      H5SUPPORT_TEST_THROW_EXCEPTION(ss.str())                                                                                                                                                         \
    }                                                                                                                                                                                                  \
  }

#define H5SUPPORT_REQUIRE_VALID_POINTER(L)                                                                                                                                                             \
  {                                                                                                                                                                                                    \
    std::stringstream ss;                                                                                                                                                                              \
    if(L == nullptr)                                                                                                                                                                                   \
    {                                                                                                                                                                                                  \
      ss << "Your test requires\n            '";                                                                                                                                                       \
      ss << #L << "' != nullptr' but this condition was not met.\n";                                                                                                                                   \
      ss << "\n";                                                                                                                                                                                      \
      H5SUPPORT_TEST_THROW_EXCEPTION(ss.str())                                                                                                                                                         \
    }                                                                                                                                                                                                  \
  }

#define H5SUPPORT_REQUIRE_NULL_POINTER(L)                                                                                                                                                              \
  {                                                                                                                                                                                                    \
    std::stringstream ss;                                                                                                                                                                              \
    if(L != nullptr)                                                                                                                                                                                   \
    {                                                                                                                                                                                                  \
      ss << "Your test requires\n            '";                                                                                                                                                       \
      ss << #L << " == nullptr' but this condition was not met.\n";                                                                                                                                    \
      ss << "\n";                                                                                                                                                                                      \
      H5SUPPORT_TEST_THROW_EXCEPTION(ss.str())                                                                                                                                                         \
    }                                                                                                                                                                                                  \
  }

// -----------------------------------------------------------------------------
// Private Macros. The Normal developer should NOT be using these.
// -----------------------------------------------------------------------------
#define H5SUPPORT_TEST_THROW_EXCEPTION(P) throw TestException(P, __FILE__, __LINE__);

#define H5SUPPORT_ASSERT(P) assert((P));

#define H5SUPPORT_ENTER_TEST(test)                                                                                                                                                                     \
  UnitTestSupport::CurrentMethod = #test;                                                                                                                                                              \
  UnitTestSupport::numTests++;

#define H5SUPPORT_LEAVE_TEST(test)                                                                                                                                                                     \
  TestPassed(#test);                                                                                                                                                                                   \
  UnitTestSupport::CurrentMethod = "";

#define H5SUPPORT_REGISTER_TEST(test)                                                                                                                                                                  \
  try                                                                                                                                                                                                  \
  {                                                                                                                                                                                                    \
    H5SUPPORT_ENTER_TEST(test);                                                                                                                                                                        \
    test;                                                                                                                                                                                              \
    H5SUPPORT_LEAVE_TEST(test)                                                                                                                                                                         \
  } catch(TestException & e)                                                                                                                                                                           \
  {                                                                                                                                                                                                    \
    TestFailed(UnitTestSupport::CurrentMethod);                                                                                                                                                        \
    std::cout << e.what() << std::endl;                                                                                                                                                                \
  }

#define PRINT_TEST_SUMMARY(error)                                                                                                                                                                      \
  std::cout << "Test Summary:" << std::endl;                                                                                                                                                           \
  std::cout << "  Tests Passed: " << UnitTestSupport::numTestsPass << std::endl;                                                                                                                       \
  std::cout << "  Tests Failed: " << UnitTestSupport::numTestFailed << std::endl;                                                                                                                      \
  std::cout << "  Total Tests:  " << UnitTestSupport::numTests << std::endl;                                                                                                                           \
  if(UnitTestSupport::numTestFailed > 0)                                                                                                                                                               \
  {                                                                                                                                                                                                    \
    error = EXIT_FAILURE;                                                                                                                                                                              \
  }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T, typename K>
void require_equal(T l, const std::string& L, K r, const std::string& R, const std::string file = "", int line = 0)
{
  if(l != r)
  {
    std::stringstream ss;
    ss << "Your test required the following\n            '";
    ss << L << " == " << R << "'\n             but this condition was not met.\n";
    ss << "             " << l << "==" << r;
    throw TestException(ss.str(), file, line);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T, typename K>
void require_less_than(T l, const std::string& L, K r, const std::string& R, const std::string file = "", int line = 0)
{
  if(l >= r)
  {
    std::stringstream ss;
    ss << "Your test required the following\n            '";
    ss << L << " < " << R << "'\n             but this condition was not met.\n";
    ss << "             " << l << "==" << r;
    throw TestException(ss.str(), file, line);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T, typename K>
void require_greater_than(T l, const std::string& L, K r, const std::string& R, const std::string file = "", int line = 0)
{
  if(l <= r)
  {
    std::stringstream ss;
    ss << "Your test required the following\n            '";
    ss << L << " < " << R << "'\n             but this condition was not met.\n";
    ss << "             " << l << "==" << r;
    throw TestException(ss.str(), file, line);
  }
}
