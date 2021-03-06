/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2015 INGENICO S.A.
 28-32 boulevard de Grenelle 75015 Paris, France.
 All rights reserved.
 This source program is the property of the INGENICO Company mentioned above
 and may not be copied in any form or by any means, whether in part or in whole,
 except under license expressly granted by such INGENICO company.
 All copies of this source program, whether in part or in whole, and
 whether modified or not, must display this and all other
 embedded copyright and ownership notices in full.
 ------------------------------------------------------------------------------
 */

#include <map>
#include <types/capk.h>

namespace bdor {

std::map<unsigned int, types::Capk> GetVisaLiveCapks() {
  std::map<unsigned int, types::Capk> output;

  types::Capk key1;
  key1.index = 0x07;
  key1.modulus = {0xA8, 0x9F, 0x25, 0xA5, 0x6F, 0xA6, 0xDA, 0x25, 0x8C, 0x8C, 0xA8, 0xB4, 0x04, 0x27, 0xD9, 0x27, 0xB4, 0xA1, 0xEB, 0x4D,
      0x7E, 0xA3, 0x26, 0xBB, 0xB1, 0x2F, 0x97, 0xDE, 0xD7, 0x0A, 0xE5, 0xE4, 0x48, 0x0F, 0xC9, 0xC5, 0xE8, 0xA9, 0x72, 0x17,
      0x71, 0x10, 0xA1, 0xCC, 0x31, 0x8D, 0x06, 0xD2, 0xF8, 0xF5, 0xC4, 0x84, 0x4A, 0xC5, 0xFA, 0x79, 0xA4, 0xDC, 0x47, 0x0B,
      0xB1, 0x1E, 0xD6, 0x35, 0x69, 0x9C, 0x17, 0x08, 0x1B, 0x90, 0xF1, 0xB9, 0x84, 0xF1, 0x2E, 0x92, 0xC1, 0xC5, 0x29, 0x27,
      0x6D, 0x8A, 0xF8, 0xEC, 0x7F, 0x28, 0x49, 0x20, 0x97, 0xD8, 0xCD, 0x5B, 0xEC, 0xEA, 0x16, 0xFE, 0x40, 0x88, 0xF6, 0xCF,
      0xAB, 0x4A, 0x1B, 0x42, 0x32, 0x8A, 0x1B, 0x99, 0x6F, 0x92, 0x78, 0xB0, 0xB7, 0xE3, 0x31, 0x1C, 0xA5, 0xEF, 0x85, 0x6C,
      0x2F, 0x88, 0x84, 0x74, 0xB8, 0x36, 0x12, 0xA8, 0x2E, 0x4E, 0x00, 0xD0, 0xCD, 0x40, 0x69, 0xA6, 0x78, 0x31, 0x40, 0x43,
      0x3D, 0x50, 0x72, 0x5F};
  key1.exponent = {0x03};
  key1.sha1 = {0xB4, 0xBC, 0x56, 0xCC, 0x4E, 0x88, 0x32, 0x49, 0x32, 0xCB, 0xC6, 0x43, 0xD6, 0x89, 0x8F, 0x6F, 0xE5, 0x93, 0xB1, 0x72};
  output[key1.index] = key1;

  types::Capk key2;
  key2.index = 0x08;
  key2.modulus = {0xD9, 0xFD, 0x6E, 0xD7, 0x5D, 0x51, 0xD0, 0xE3, 0x06, 0x64, 0xBD, 0x15, 0x70, 0x23, 0xEA, 0xA1, 0xFF, 0xA8,
      0x71, 0xE4, 0xDA, 0x65, 0x67, 0x2B, 0x86, 0x3D, 0x25, 0x5E, 0x81, 0xE1, 0x37, 0xA5, 0x1D, 0xE4, 0xF7, 0x2B, 0xCC, 0x9E,
      0x44, 0xAC, 0xE1, 0x21, 0x27, 0xF8, 0x7E, 0x26, 0x3D, 0x3A, 0xF9, 0xDD, 0x9C, 0xF3, 0x5C, 0xA4, 0xA7, 0xB0, 0x1E, 0x90,
      0x70, 0x00, 0xBA, 0x85, 0xD2, 0x49, 0x54, 0xC2, 0xFC, 0xA3, 0x07, 0x48, 0x25, 0xDD, 0xD4, 0xC0, 0xC8, 0xF1, 0x86, 0xCB,
      0x02, 0x0F, 0x68, 0x3E, 0x02, 0xF2, 0xDE, 0xAD, 0x39, 0x69, 0x13, 0x3F, 0x06, 0xF7, 0x84, 0x51, 0x66, 0xAC, 0xEB, 0x57,
      0xCA, 0x0F, 0xC2, 0x60, 0x34, 0x45, 0x46, 0x98, 0x11, 0xD2, 0x93, 0xBF, 0xEF, 0xBA, 0xFA, 0xB5, 0x76, 0x31, 0xB3, 0xDD,
      0x91, 0xE7, 0x96, 0xBF, 0x85, 0x0A, 0x25, 0x01, 0x2F, 0x1A, 0xE3, 0x8F, 0x05, 0xAA, 0x5C, 0x4D, 0x6D, 0x03, 0xB1, 0xDC,
      0x2E, 0x56, 0x86, 0x12, 0x78, 0x59, 0x38, 0xBB, 0xC9, 0xB3, 0xCD, 0x3A, 0x91, 0x0C, 0x1D, 0xA5, 0x5A, 0x5A, 0x92, 0x18,
      0xAC, 0xE0, 0xF7, 0xA2, 0x12, 0x87, 0x75, 0x26, 0x82, 0xF1, 0x58, 0x32, 0xA6, 0x78, 0xD6, 0xE1, 0xED, 0x0B};
  key2.exponent = {0x03};
  key2.sha1 = {0x20, 0xD2, 0x13, 0x12, 0x69, 0x55, 0xDE, 0x20, 0x5A, 0xDC, 0x2F, 0xD2, 0x82, 0x2B, 0xD2, 0x2D, 0xE2, 0x1C, 0xF9, 0xA8};
  output[key2.index] = key2;

  types::Capk key3;
  key3.index = 0x09;
  key3.modulus = {0x9D, 0x91, 0x22, 0x48, 0xDE, 0x0A, 0x4E, 0x39, 0xC1, 0xA7, 0xDD, 0xE3, 0xF6, 0xD2, 0x58, 0x89, 0x92, 0xC1,
      0xA4, 0x09, 0x5A, 0xFB, 0xD1, 0x82, 0x4D, 0x1B, 0xA7, 0x48, 0x47, 0xF2, 0xBC, 0x49, 0x26, 0xD2, 0xEF, 0xD9, 0x04, 0xB4,
      0xB5, 0x49, 0x54, 0xCD, 0x18, 0x9A, 0x54, 0xC5, 0xD1, 0x17, 0x96, 0x54, 0xF8, 0xF9, 0xB0, 0xD2, 0xAB, 0x5F, 0x03, 0x57,
      0xEB, 0x64, 0x2F, 0xED, 0xA9, 0x5D, 0x39, 0x12, 0xC6, 0x57, 0x69, 0x45, 0xFA, 0xB8, 0x97, 0xE7, 0x06, 0x2C, 0xAA, 0x44,
      0xA4, 0xAA, 0x06, 0xB8, 0xFE, 0x6E, 0x3D, 0xBA, 0x18, 0xAF, 0x6A, 0xE3, 0x73, 0x8E, 0x30, 0x42, 0x9E, 0xE9, 0xBE, 0x03,
      0x42, 0x7C, 0x9D, 0x64, 0xF6, 0x95, 0xFA, 0x8C, 0xAB, 0x4B, 0xFE, 0x37, 0x68, 0x53, 0xEA, 0x34, 0xAD, 0x1D, 0x76, 0xBF,
      0xCA, 0xD1, 0x59, 0x08, 0xC0, 0x77, 0xFF, 0xE6, 0xDC, 0x55, 0x21, 0xEC, 0xEF, 0x5D, 0x27, 0x8A, 0x96, 0xE2, 0x6F, 0x57,
      0x35, 0x9F, 0xFA, 0xED, 0xA1, 0x94, 0x34, 0xB9, 0x37, 0xF1, 0xAD, 0x99, 0x9D, 0xC5, 0xC4, 0x1E, 0xB1, 0x19, 0x35, 0xB4,
      0x4C, 0x18, 0x10, 0x0E, 0x85, 0x7F, 0x43, 0x1A, 0x4A, 0x5A, 0x6B, 0xB6, 0x51, 0x14, 0xF1, 0x74, 0xC2, 0xD7, 0xB5, 0x9F,
      0xDF, 0x23, 0x7D, 0x6B, 0xB1, 0xDD, 0x09, 0x16, 0xE6, 0x44, 0xD7, 0x09, 0xDE, 0xD5, 0x64, 0x81, 0x47, 0x7C, 0x75, 0xD9,
      0x5C, 0xDD, 0x68, 0x25, 0x46, 0x15, 0xF7, 0x74, 0x0E, 0xC0, 0x7F, 0x33, 0x44, 0x0A, 0xC5, 0xD6, 0x7B, 0xCD, 0x75, 0xBF,
      0x23, 0xD2, 0x8A, 0x14, 0x08, 0x26, 0xC0, 0x26, 0xDB, 0xDE, 0x97, 0x1A, 0x37, 0xCD, 0x3E, 0xF9, 0xB8, 0xDF, 0x64, 0x4A,
      0xC3, 0x85, 0x01, 0x05, 0x01, 0xEF, 0xC6, 0x50, 0x9D, 0x7A, 0x41};
  key3.exponent = {0x03};
  key3.sha1 = {0x1F, 0xF8, 0x0A, 0x40, 0x17, 0x3F, 0x52, 0xD7, 0xD2, 0x7E, 0x0F, 0x26, 0xA1, 0x46, 0xA1, 0xC8, 0xCC, 0xB2, 0x90, 0x46};
  output[key3.index] = key3;
  return output;
}

std::map<unsigned int, types::Capk> GetVisaTestCapks() {
  std::map<unsigned int, types::Capk> output;
  output = GetVisaLiveCapks();
  types::Capk key1;
  key1.index = 0x95;
  key1.modulus = {0xBE, 0x9E, 0x1F, 0xA5, 0xE9, 0xA8, 0x03, 0x85, 0x29, 0x99, 0xC4, 0xAB, 0x43, 0x2D, 0xB2, 0x86, 0x00, 0xDC, 0xD9, 0xDA, 0xB7,
    0x6D, 0xFA, 0xAA, 0x47, 0x35, 0x5A, 0x0F, 0xE3, 0x7B, 0x15, 0x08, 0xAC, 0x6B, 0xF3, 0x88, 0x60, 0xD3, 0xC6, 0xC2, 0xE5, 0xB1,
    0x2A, 0x3C, 0xAA, 0xF2, 0xA7, 0x00, 0x5A, 0x72, 0x41, 0xEB, 0xAA, 0x77, 0x71, 0x11, 0x2C, 0x74, 0xCF, 0x9A, 0x06, 0x34, 0x65,
    0x2F, 0xBC, 0xA0, 0xE5, 0x98, 0x0C, 0x54, 0xA6, 0x47, 0x61, 0xEA, 0x10, 0x1A, 0x11, 0x4E, 0x0F, 0x0B, 0x55, 0x72, 0xAD, 0xD5,
    0x7D, 0x01, 0x0B, 0x7C, 0x9C, 0x88, 0x7E, 0x10, 0x4C, 0xA4, 0xEE, 0x12, 0x72, 0xDA, 0x66, 0xD9, 0x97, 0xB9, 0xA9, 0x0B, 0x5A,
    0x6D, 0x62, 0x4A, 0xB6, 0xC5, 0x7E, 0x73, 0xC8, 0xF9, 0x19, 0x00, 0x0E, 0xB5, 0xF6, 0x84, 0x89, 0x8E, 0xF8, 0xC3, 0xDB, 0xEF,
    0xB3, 0x30, 0xC6, 0x26, 0x60, 0xBE, 0xD8, 0x8E, 0xA7, 0x8E, 0x90, 0x9A, 0xFF, 0x05, 0xF6, 0xDA, 0x62, 0x7B};
  key1.exponent = {0x03};
  key1.sha1 = {0xEE, 0x15, 0x11, 0xCE, 0xC7, 0x10, 0x20, 0xA9, 0xB9, 0x04, 0x43, 0xB3, 0x7B, 0x1D, 0x5F, 0x6E, 0x70, 0x30, 0x30, 0xF6};
  output[key1.index] = key1;

  types::Capk key2;
  key2.index = 0x92;
  key2.modulus = {0x99, 0x6A, 0xF5, 0x6F, 0x56, 0x91, 0x87, 0xD0, 0x92, 0x93, 0xC1, 0x48, 0x10, 0x45, 0x0E, 0xD8, 0xEE, 0x33, 0x57, 0x39, 0x7B,
    0x18, 0xA2, 0x45, 0x8E, 0xFA, 0xA9, 0x2D, 0xA3, 0xB6, 0xDF, 0x65, 0x14, 0xEC, 0x06, 0x01, 0x95, 0x31, 0x8F, 0xD4, 0x3B, 0xE9,
    0xB8, 0xF0, 0xCC, 0x66, 0x9E, 0x3F, 0x84, 0x40, 0x57, 0xCB, 0xDD, 0xF8, 0xBD, 0xA1, 0x91, 0xBB, 0x64, 0x47, 0x3B, 0xC8, 0xDC,
    0x9A, 0x73, 0x0D, 0xB8, 0xF6, 0xB4, 0xED, 0xE3, 0x92, 0x41, 0x86, 0xFF, 0xD9, 0xB8, 0xC7, 0x73, 0x57,
    0x89, 0xC2, 0x3A, 0x36, 0xBA, 0x0B, 0x8A, 0xF6, 0x53, 0x72, 0xEB, 0x57, 0xEA, 0x5D, 0x89, 0xE7, 0xD1, 0x4E, 0x9C, 0x7B, 0x6B,
    0x55, 0x74, 0x60, 0xF1, 0x08, 0x85, 0xDA, 0x16, 0xAC, 0x92, 0x3F, 0x15, 0xAF, 0x37, 0x58, 0xF0, 0xF0, 0x3E, 0xBD, 0x3C, 0x5C,
    0x2C, 0x94, 0x9C, 0xBA, 0x30, 0x6D, 0xB4, 0x4E, 0x6A, 0x2C, 0x07, 0x6C, 0x5F, 0x67, 0xE2, 0x81, 0xD7, 0xEF, 0x56, 0x78, 0x5D,
    0xC4, 0xD7, 0x59, 0x45, 0xE4, 0x91, 0xF0, 0x19, 0x18, 0x80, 0x0A, 0x9E, 0x2D, 0xC6, 0x6F, 0x60, 0x08, 0x05, 0x66, 0xCE, 0x0D,
    0xAF, 0x8D, 0x17, 0xEA, 0xD4, 0x6A, 0xD8, 0xE3, 0x0A, 0x24, 0x7C, 0x9F};
  key2.exponent = {0x03};
  key2.sha1 = {0x42, 0x9C, 0x95, 0x4A, 0x38, 0x59, 0xCE, 0xF9, 0x12, 0x95, 0xF6, 0x63, 0xC9, 0x63, 0xE5, 0x82, 0xED, 0x6E, 0xB2, 0x53};
  output[key2.index] = key2;

  types::Capk key3;
  key3.index = 0x94;
  key3.modulus = {0xAC, 0xD2, 0xB1, 0x23, 0x02, 0xEE, 0x64, 0x4F, 0x3F, 0x83, 0x5A, 0xBD, 0x1F, 0xC7, 0xA6, 0xF6, 0x2C, 0xCE, 0x48, 0xFF, 0xEC,
    0x62, 0x2A, 0xA8, 0xEF, 0x06, 0x2B, 0xEF, 0x6F, 0xB8, 0xBA, 0x8B, 0xC6, 0x8B, 0xBF, 0x6A, 0xB5, 0x87, 0x0E, 0xED, 0x57, 0x9B,
    0xC3, 0x97, 0x3E, 0x12, 0x13, 0x03, 0xD3, 0x48, 0x41, 0xA7, 0x96, 0xD6, 0xDC, 0xBC, 0x41, 0xDB, 0xF9, 0xE5, 0x2C, 0x46, 0x09,
    0x79, 0x5C, 0x0C, 0xCF, 0x7E, 0xE8, 0x6F, 0xA1, 0xD5, 0xCB, 0x04, 0x10, 0x71, 0xED, 0x2C, 0x51, 0xD2, 0x20, 0x2F, 0x63, 0xF1,
    0x15, 0x6C, 0x58, 0xA9, 0x2D, 0x38, 0xBC, 0x60, 0xBD, 0xF4, 0x24, 0xE1, 0x77, 0x6E, 0x2B, 0xC9, 0x64, 0x80, 0x78, 0xA0, 0x3B,
    0x36, 0xFB, 0x55, 0x43, 0x75, 0xFC, 0x53, 0xD5, 0x7C, 0x73, 0xF5, 0x16, 0x0E, 0xA5, 0x9F, 0x3A, 0xFC, 0x53, 0x98, 0xEC, 0x7B,
    0x67, 0x75, 0x8D, 0x65, 0xC9, 0xBF, 0xF7, 0x82, 0x8B, 0x6B, 0x82, 0xD4, 0xBE, 0x12, 0x4A, 0x41, 0x6A, 0xB7, 0x30, 0x19, 0x14,
    0x31, 0x1E, 0xA4, 0x62, 0xC1, 0x9F, 0x77, 0x1F, 0x31, 0xB3, 0xB5, 0x73, 0x36, 0x00, 0x0D, 0xFF, 0x73, 0x2D, 0x3B, 0x83, 0xDE,
    0x07, 0x05, 0x2D, 0x73, 0x03, 0x54, 0xD2, 0x97, 0xBE, 0xC7, 0x28, 0x71, 0xDC, 0xCF, 0x0E, 0x19, 0x3F, 0x17, 0x1A, 0xBA, 0x27,
    0xEE, 0x46, 0x4C, 0x6A, 0x97, 0x69, 0x09, 0x43, 0xD5, 0x9B, 0xDA, 0xBB, 0x2A, 0x27, 0xEB, 0x71, 0xCE, 0xEB, 0xDA, 0xFA, 0x11,
    0x76, 0x04, 0x64, 0x78, 0xFD, 0x62, 0xFE, 0xC4, 0x52, 0xD5, 0xCA, 0x39, 0x32, 0x96, 0x53, 0x0A, 0xA3, 0xF4, 0x19, 0x27, 0xAD,
    0xFE, 0x43, 0x4A, 0x2D, 0xF2, 0xAE, 0x30, 0x54, 0xF8, 0x84, 0x06, 0x57, 0xA2, 0x6E, 0x0F, 0xC6, 0x17};
  key3.exponent = {0x03};
  key3.sha1 = {0xC4, 0xA3, 0xC4, 0x3C, 0xCF, 0x87, 0x32, 0x7D, 0x13, 0x6B, 0x80, 0x41, 0x60, 0xE4, 0x7D, 0x43,
    0xB6, 0x0E, 0x6E, 0x0F};
  output[key3.index] = key3;

  return output;
}

}
