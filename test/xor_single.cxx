#include "gtest/gtest.h"

#include <ciphey/swig.hpp>
#include "common.hpp"

static std::vector<uint8_t> ctext {
  0x0a, 0x17, 0x43, 0x14, 0x02, 0x10, 0x43, 0x17, 0x0b, 0x06, 0x43, 0x07,
  0x0c, 0x15, 0x06, 0x11, 0x43, 0x11, 0x0c, 0x02, 0x07, 0x43, 0x17, 0x0b,
  0x02, 0x17, 0x43, 0x0f, 0x02, 0x1a, 0x4f, 0x43, 0x0c, 0x0d, 0x43, 0x02,
  0x43, 0x05, 0x11, 0x0a, 0x07, 0x02, 0x1a, 0x43, 0x0d, 0x0a, 0x04, 0x0b,
  0x17, 0x43, 0x0f, 0x02, 0x17, 0x06, 0x43, 0x0a, 0x0d, 0x43, 0x0d, 0x0c,
  0x15, 0x06, 0x0e, 0x01, 0x06, 0x11, 0x4f, 0x43, 0x01, 0x06, 0x05, 0x0c,
  0x11, 0x06, 0x43, 0x17, 0x0b, 0x06, 0x43, 0x05, 0x0a, 0x11, 0x10, 0x17,
  0x43, 0x0c, 0x05, 0x43, 0x17, 0x0b, 0x06, 0x43, 0x13, 0x06, 0x11, 0x10,
  0x0c, 0x0d, 0x10, 0x43, 0x14, 0x0a, 0x17, 0x0b, 0x43, 0x14, 0x0b, 0x0c,
  0x0e, 0x43, 0x17, 0x0b, 0x0a, 0x10, 0x43, 0x0b, 0x0a, 0x10, 0x17, 0x0c,
  0x11, 0x1a, 0x43, 0x0b, 0x02, 0x10, 0x43, 0x01, 0x16, 0x10, 0x0a, 0x0d,
  0x06, 0x10, 0x10, 0x4d
};

static std::vector<uint8_t> ptext {
  0x69, 0x74, 0x20, 0x77, 0x61, 0x73, 0x20, 0x74, 0x68, 0x65, 0x20, 0x64,
  0x6f, 0x76, 0x65, 0x72, 0x20, 0x72, 0x6f, 0x61, 0x64, 0x20, 0x74, 0x68,
  0x61, 0x74, 0x20, 0x6c, 0x61, 0x79, 0x2c, 0x20, 0x6f, 0x6e, 0x20, 0x61,
  0x20, 0x66, 0x72, 0x69, 0x64, 0x61, 0x79, 0x20, 0x6e, 0x69, 0x67, 0x68,
  0x74, 0x20, 0x6c, 0x61, 0x74, 0x65, 0x20, 0x69, 0x6e, 0x20, 0x6e, 0x6f,
  0x76, 0x65, 0x6d, 0x62, 0x65, 0x72, 0x2c, 0x20, 0x62, 0x65, 0x66, 0x6f,
  0x72, 0x65, 0x20, 0x74, 0x68, 0x65, 0x20, 0x66, 0x69, 0x72, 0x73, 0x74,
  0x20, 0x6f, 0x66, 0x20, 0x74, 0x68, 0x65, 0x20, 0x70, 0x65, 0x72, 0x73,
  0x6f, 0x6e, 0x73, 0x20, 0x77, 0x69, 0x74, 0x68, 0x20, 0x77, 0x68, 0x6f,
  0x6d, 0x20, 0x74, 0x68, 0x69, 0x73, 0x20, 0x68, 0x69, 0x73, 0x74, 0x6f,
  0x72, 0x79, 0x20, 0x68, 0x61, 0x73, 0x20, 0x62, 0x75, 0x73, 0x69, 0x6e,
  0x65, 0x73, 0x73, 0x2e
};

static uint8_t key = 'c';

TEST(cipheyCore, xorSingle) {
  auto analysis = ciphey::analyse_bytes(ctext);

  auto res_collection = ciphey::xor_single_crack(analysis, ciphey::test::expected());

  for (size_t i = 0; i < res_collection.size(); ++i) {
    if (res_collection[i].key == key) {
      std::cout << "True key had p-value of " << res_collection[i].p_value << " and was position " << i << std::endl;
      goto found;
    }
    else {
      std::cout << "False key " << res_collection[i].key << " had p-value of " << res_collection[i].p_value << std::endl;
    }
  }
  EXPECT_TRUE(false) << "Key was not found";
  found:

  auto ptext_res = ciphey::xor_single_decrypt(ctext, key);
  EXPECT_TRUE(std::equal(ptext_res.begin(), ptext_res.end(), ptext.begin()));
  ptext_res = ciphey::xor_single_decrypt(ptext_res, key);
  EXPECT_TRUE(std::equal(ptext_res.begin(), ptext_res.end(), ctext.begin()));

  std::cout << reinterpret_cast<char*>(ptext.data()) << std::endl;
}