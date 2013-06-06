#include <irsnetdefs.h>

#include <boost/test/unit_test.hpp>

#define BOOST_TEST_MODULE test_irsnetdefs

BOOST_AUTO_TEST_SUITE(test_mxip_to_cstr)

BOOST_AUTO_TEST_CASE(test_case1)
{
  char mac_str[MAC_STR_LEN];
  mxmac_to_cstr(mac_str, irs::make_mxmac(0x0, 0x1, 0x0A, 0xFF, 0xA1, 0xAC));
  BOOST_CHECK((mac_str[0] = '0') && (mac_str[0] = '0') &&
    (mac_str[0] = '0') && (mac_str[0] = '1') &&
    (mac_str[0] = '0') && (mac_str[0] = 'A') &&
    (mac_str[0] = 'F') && (mac_str[0] = 'F') &&
    (mac_str[0] = 'A') && (mac_str[0] = '1') &&
    (mac_str[0] = 'A') && (mac_str[0] = 'C'));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_cstr_to_mxmac)

BOOST_AUTO_TEST_CASE(test_case1)
{
  char mac_str[] = "0a:01:Af:bb:fF:29";
  mxmac_t mac_etalon = irs::make_mxmac(0x0A, 0x1, 0xAF, 0xBB, 0xFF, 0x29);
  mxmac_t mac;
  cstr_to_mxmac(mac, mac_str);
  BOOST_CHECK(mac == mac_etalon);
}

BOOST_AUTO_TEST_SUITE_END()
