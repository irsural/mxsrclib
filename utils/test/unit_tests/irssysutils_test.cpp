#include <irssysutils.h>

#include <boost/test/unit_test.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#define BOOST_TEST_MODULE test_irssysutils

BOOST_AUTO_TEST_SUITE(test_absolute_path_to_relative)

BOOST_AUTO_TEST_CASE(test_case_1)
{
  const irs::string_t absolute(irst("C:\\Temp\\temp_prog\\1.txt"));
  const irs::string_t base(irst("C:\\Temp\\"));
  const irs::string_t relative_etalon(irst("temp_prog\\1.txt"));

  irs::string_t relative = irs::absolute_path_to_relative(absolute, base);

  BOOST_CHECK(relative == relative_etalon);
}

/*BOOST_AUTO_TEST_CASE(test_case_2)
{
  const irs::string_t absolute(irst("C:\\Temp\\prog\\1.txt"));
  const irs::string_t base(irst("C:\\Temp\\"));
  const irs::string_t relative_etalon(irst("prog\\1.txt"));

  typedef boost::filesystem::path::string_type path_str_type;

  boost::filesystem::path absolute_path(irs::str_conv<path_str_type>(absolute));
  boost::filesystem::path base_path(irs::str_conv<path_str_type>(base));
  boost::filesystem::path relative_etalon_path(
    irs::str_conv<path_str_type>(relative_etalon));

  boost::filesystem::path relative_path =
    irs::absolute_path_to_relative(absolute_path, base_path);

  BOOST_CHECK(relative_path == relative_etalon_path);
}*/

/*BOOST_AUTO_TEST_CASE(test_case_3)
{
  const irs::string_t absolute(irst("C:\\Temp\\progs\\1.txt"));
  const irs::string_t base(irst("C:\\Temp\\progs\\my_prog\\prog\\"));
  const irs::string_t relative_etalon(irst("..\\..\\1.txt"));

  typedef boost::filesystem::path::string_type path_str_type;

  boost::filesystem::path absolute_path(irs::str_conv<path_str_type>(absolute));
  boost::filesystem::path base_path(irs::str_conv<path_str_type>(base));
  boost::filesystem::path relative_etalon_path(
    irs::str_conv<path_str_type>(relative_etalon));

  boost::filesystem::path relative_path =
    irs::absolute_path_to_relative(absolute_path, base_path);

  BOOST_CHECK(relative_path == relative_etalon_path);
}*/

/*BOOST_AUTO_TEST_CASE(test_case_4)
{
  const irs::string_t absolute(irst("C:\\Temp\\progs\\file\\1.txt"));
  const irs::string_t base(irst("C:\\Temp\\progs\\myprog\\prog\\"));
  const irs::string_t relative_etalon(irst("..\\..\\file\\1.txt"));

  typedef boost::filesystem::path::string_type path_str_type;

  boost::filesystem::path absolute_path(irs::str_conv<path_str_type>(absolute));
  boost::filesystem::path base_path(irs::str_conv<path_str_type>(base));
  boost::filesystem::path relative_etalon_path(
    irs::str_conv<path_str_type>(relative_etalon));

  boost::filesystem::path relative_path =
    irs::absolute_path_to_relative(absolute_path, base_path);

  BOOST_CHECK(relative_path == relative_etalon_path);
}*/

/*BOOST_AUTO_TEST_CASE(test_case_5)
{
  const irs::string_t absolute(
    irst("C:\\Temp\\progs\\myprog\\prog\\..\\..\\file\\1.txt"));
  const irs::string_t base(irst("C:\\Temp\\progs\\myprog\\prog\\"));
  const irs::string_t relative_etalon(irst("..\\..\\file\\1.txt"));

  typedef boost::filesystem::path::string_type path_str_type;

  boost::filesystem::path absolute_path(irs::str_conv<path_str_type>(absolute));
  boost::filesystem::path base_path(irs::str_conv<path_str_type>(base));
  boost::filesystem::path relative_etalon_path(
    irs::str_conv<path_str_type>(relative_etalon));

  boost::filesystem::path relative_path =
    irs::absolute_path_to_relative(absolute_path, base_path);

  BOOST_CHECK(relative_path == relative_etalon_path);
}*/

/*BOOST_AUTO_TEST_CASE(test_case_6)
{
  const irs::string_t absolute(
    irst("C:\\Temp\\..\\Temp\\..\\Temp\\progs\\file\\1.txt"));
  const irs::string_t base(irst("C:\\Temp\\progs\\myprog\\prog\\"));
  const irs::string_t relative_etalon(irst("..\\..\\file\\1.txt"));

  typedef boost::filesystem::path::string_type path_str_type;

  boost::filesystem::path absolute_path(irs::str_conv<path_str_type>(absolute));
  boost::filesystem::path base_path(irs::str_conv<path_str_type>(base));
  boost::filesystem::path relative_etalon_path(
    irs::str_conv<path_str_type>(relative_etalon));

  boost::filesystem::path relative_path =
    irs::absolute_path_to_relative(absolute_path, base_path);

  BOOST_CHECK(relative_path == relative_etalon_path);
}*/

BOOST_AUTO_TEST_CASE(test_case_7)
{
  boost::filesystem::path::iterator it;


  const irs::string_t relative(irst("..\\..\\file\\1.txt"));
  const irs::string_t base(irst("C:\\Temp\\progs\\myprog\\prog\\"));
  const irs::string_t absolute_etalon(irst("C:\\Temp\\progs\\file\\1.txt"));

  typedef boost::filesystem::path::string_type path_str_type;

  boost::filesystem::path d = boost::filesystem::detail::dot_path();

  boost::filesystem::path base3(irst("C:\\Temp\\progs\\myprog\\prog\\"));
  boost::filesystem::path path1 = base3.filename();
  base3.remove_filename();
  boost::filesystem::path path2 = base3.filename();
  base3.remove_filename();
  boost::filesystem::path path3 = base3.filename();

  boost::filesystem::path relative_path(irs::str_conv<path_str_type>(relative));
  boost::filesystem::path base_path(irs::str_conv<path_str_type>(base));
  boost::filesystem::path norm = base_path.lexically_normal();

  irs::string_t s1 = base_path.native();
  base_path.remove_filename();
  base_path.remove_filename();
  irs::string_t s2 = base_path.native();

  boost::filesystem::path absolute_etalon_path(
    irs::str_conv<path_str_type>(absolute_etalon));

  irs::string_t absolute = irs::relative_path_to_absolute(relative, base);

  int a = 0;
  //BOOST_CHECK(relative_path == relative_etalon_path);
}

BOOST_AUTO_TEST_SUITE_END()
