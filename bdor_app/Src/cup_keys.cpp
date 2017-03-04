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
std::map<unsigned int, types::Capk> GetCupLiveCapks() {
  std::map<unsigned int, types::Capk> output;

  types::Capk key1;
  key1.index = 0x02;
  key1.modulus = {0xA3, 0x76, 0x7A, 0xBD, 0x1B, 0x6A, 0xA6, 0x9D, 0x7F, 0x3F, 0xBF, 0x28, 0xC0, 0x92, 0xDE, 0x9E, 0xD1, 0xE6, 0x58,
    0xBA, 0x5F, 0x09, 0x09, 0xAF, 0x7A, 0x1C, 0xCD, 0x90, 0x73, 0x73, 0xB7, 0x21, 0x0F, 0xDE, 0xB1, 0x62, 0x87, 0xBA, 0x8E, 0x78,
    0xE1, 0x52, 0x9F, 0x44, 0x39, 0x76, 0xFD, 0x27, 0xF9, 0x91, 0xEC, 0x67, 0xD9, 0x5E, 0x5F, 0x4E, 0x96, 0xB1, 0x27, 0xCA, 0xB2,
    0x39, 0x6A, 0x94, 0xD6, 0xE4, 0x5C, 0xDA, 0x44, 0xCA, 0x4C, 0x48, 0x67, 0x57, 0x0D, 0x6B, 0x07, 0x54, 0x2F, 0x8D, 0x4B, 0xF9,
    0xFF, 0x97, 0x97, 0x5D, 0xB9, 0x89, 0x15, 0x15, 0xE6, 0x6F, 0x52, 0x5D, 0x2B, 0x3C, 0xBE, 0xB6, 0xD6, 0x62, 0xBF, 0xB6, 0xC3,
    0xF3, 0x38, 0xE9, 0x3B, 0x02, 0x14, 0x2B, 0xFC, 0x44, 0x17, 0x3A, 0x37, 0x64, 0xC5, 0x6A, 0xAD, 0xD2, 0x02, 0x07, 0x5B, 0x26,
    0xDC, 0x2F, 0x9F, 0x7D, 0x7A, 0xE7, 0x4B, 0xD7, 0xD0, 0x0F, 0xD0, 0x5E, 0xE4, 0x30, 0x03, 0x26, 0x63, 0xD2, 0x7A, 0x57};
  key1.exponent = {0x03};
  key1.sha1 = {0x03, 0xBB, 0x33, 0x5A, 0x85, 0x49, 0xA0, 0x3B, 0x87, 0xAB, 0x08, 0x9D, 0x00, 0x6F, 0x60, 0x85, 0x2E, 0x4B, 0x80, 0x60};
  output[key1.index] = key1;

  types::Capk key2;
  key2.index = 0x03;
  key2.modulus = {0xB0, 0x62, 0x7D, 0xEE, 0x87, 0x86, 0x4F, 0x9C, 0x18, 0xC1, 0x3B, 0x9A, 0x1F, 0x02, 0x54, 0x48, 0xBF, 0x13, 0xC5,
    0x83, 0x80, 0xC9, 0x1F, 0x4C, 0xEB, 0xA9, 0xF9, 0xBC, 0xB2, 0x14, 0xFF, 0x84, 0x14, 0xE9, 0xB5, 0x9D, 0x6A, 0xBA, 0x10, 0xF9,
    0x41, 0xC7, 0x33, 0x17, 0x68, 0xF4, 0x7B, 0x21, 0x27, 0x90, 0x7D, 0x85, 0x7F, 0xA3, 0x9A, 0xAF, 0x8C, 0xE0, 0x20, 0x45, 0xDD,
    0x01, 0x61, 0x9D, 0x68, 0x9E, 0xE7, 0x31, 0xC5, 0x51, 0x15, 0x9B, 0xE7, 0xEB, 0x2D, 0x51, 0xA3, 0x72, 0xFF, 0x56, 0xB5, 0x56,
    0xE5, 0xCB, 0x2F, 0xDE, 0x36, 0xE2, 0x30, 0x73, 0xA4, 0x4C, 0xA2, 0x15, 0xD6, 0xC2, 0x6C, 0xA6, 0x88, 0x47, 0xB3, 0x88, 0xE3,
    0x95, 0x20, 0xE0, 0x02, 0x6E, 0x62, 0x29, 0x4B, 0x55, 0x7D, 0x64, 0x70, 0x44, 0x0C, 0xA0, 0xAE, 0xFC, 0x94, 0x38, 0xC9, 0x23,
    0xAE, 0xC9, 0xB2, 0x09, 0x8D, 0x6D, 0x3A, 0x1A, 0xF5, 0xE8, 0xB1, 0xDE, 0x36, 0xF4, 0xB5, 0x30, 0x40, 0x10, 0x9D, 0x89, 0xB7,
    0x7C, 0xAF, 0xAF, 0x70, 0xC2, 0x6C, 0x60, 0x1A, 0xBD, 0xF5, 0x9E, 0xEC, 0x0F, 0xDC, 0x8A, 0x99, 0x08, 0x91, 0x40, 0xCD, 0x2E,
    0x81, 0x7E, 0x33, 0x51, 0x75, 0xB0, 0x3B, 0x7A, 0xA3, 0x3D};
  key2.exponent = {0x03};
  key2.sha1 = {0x87, 0xF0, 0xCD, 0x7C, 0x0E, 0x86, 0xF3, 0x8F, 0x89, 0xA6, 0x6F, 0x8C, 0x47, 0x07, 0x1A, 0x8B, 0x88, 0x58, 0x6F, 0x26};
  output[key2.index] = key2;

  types::Capk key3;
  key3.index = 0x04;
  key3.modulus = {0xBC, 0x85, 0x3E, 0x6B, 0x53, 0x65, 0xE8, 0x9E, 0x7E, 0xE9, 0x31, 0x7C, 0x94, 0xB0, 0x2D, 0x0A, 0xBB, 0x0D, 0xBD,
    0x91, 0xC0, 0x5A, 0x22, 0x4A, 0x25, 0x54, 0xAA, 0x29, 0xED, 0x9F, 0xCB, 0x9D, 0x86, 0xEB, 0x9C, 0xCB, 0xB3, 0x22, 0xA5, 0x78,
    0x11, 0xF8, 0x61, 0x88, 0xAA, 0xC7, 0x35, 0x1C, 0x72, 0xBD, 0x9E, 0xF1, 0x96, 0xC5, 0xA0, 0x1A, 0xCE, 0xF7, 0xA4, 0xEB, 0x0D,
    0x2A, 0xD6, 0x3D, 0x9E, 0x6A, 0xC2, 0xE7, 0x83, 0x65, 0x47, 0xCB, 0x15, 0x95, 0xC6, 0x8B, 0xCB, 0xAF, 0xD0, 0xF6, 0x72, 0x87,
    0x60, 0xF3, 0xA7, 0xCA, 0x7B, 0x97, 0x30, 0x1B, 0x7E, 0x02, 0x20, 0x18, 0x4E, 0xFC, 0x4F, 0x65, 0x30, 0x08, 0xD9, 0x3C, 0xE0,
    0x98, 0xC0, 0xD9, 0x3B, 0x45, 0x20, 0x10, 0x96, 0xD1, 0xAD, 0xFF, 0x4C, 0xF1, 0xF9, 0xFC, 0x02, 0xAF, 0x75, 0x9D, 0xA2, 0x7C,
    0xD6, 0xDF, 0xD6, 0xD7, 0x89, 0xB0, 0x99, 0xF1, 0x6F, 0x37, 0x8B, 0x61, 0x00, 0x33, 0x4E, 0x63, 0xF3, 0xD3, 0x5F, 0x32, 0x51,
    0xA5, 0xEC, 0x78, 0x69, 0x37, 0x31, 0xF5, 0x23, 0x35, 0x19, 0xCD, 0xB3, 0x80, 0xF5, 0xAB, 0x8C, 0x0F, 0x02, 0x72, 0x8E, 0x91,
    0xD4, 0x69, 0xAB, 0xD0, 0xEA, 0xE0, 0xD9, 0x3B, 0x1C, 0xC6, 0x6C, 0xE1, 0x27, 0xB2, 0x9C, 0x7D, 0x77, 0x44, 0x1A, 0x49, 0xD0,
    0x9F, 0xCA, 0x5D, 0x6D, 0x97, 0x62, 0xFC, 0x74, 0xC3, 0x1B, 0xB5, 0x06, 0xC8, 0xBA, 0xE3, 0xC7, 0x9A, 0xD6, 0xC2, 0x57, 0x87,
    0x75, 0xB9, 0x59, 0x56, 0xB5, 0x37, 0x0D, 0x1D, 0x05, 0x19, 0xE3, 0x79, 0x06, 0xB3, 0x84, 0x73, 0x62, 0x33, 0x25, 0x1E, 0x8F,
    0x09, 0xAD, 0x79, 0xDF, 0xBE, 0x2C, 0x6A, 0xBF, 0xAD, 0xAC, 0x8E, 0x4D, 0x86, 0x24, 0x31, 0x8C, 0x27, 0xDA, 0xF1};
  key3.exponent = {0x03};
  key3.sha1 = {0xF5, 0x27, 0x08, 0x1C, 0xF3, 0x71, 0xDD, 0x7E, 0x1F, 0xD4, 0xFA, 0x41, 0x4A, 0x66, 0x50, 0x36, 0xE0, 0xF5, 0xE6, 0xE5};
  output[key3.index] = key3;

  return output;
}

std::map<unsigned int, types::Capk> GetCupTestCapks() {
  std::map<unsigned int, types::Capk> output;
  output = GetCupLiveCapks();

  types::Capk key1;
  key1.index = 0x08;
  key1.modulus = {0xB6, 0x16, 0x45, 0xED, 0xFD, 0x54, 0x98, 0xFB, 0x24, 0x64, 0x44, 0x03, 0x7A, 0x0F, 0xA1, 0x8C, 0x0F, 0x10, 0x1E,
    0xBD, 0x8E, 0xFA, 0x54, 0x57, 0x3C, 0xE6, 0xE6, 0xA7, 0xFB, 0xF6, 0x3E, 0xD2, 0x1D, 0x66, 0x34, 0x08, 0x52, 0xB0, 0x21, 0x1C,
    0xF5, 0xEE, 0xF6, 0xA1, 0xCD, 0x98, 0x9F, 0x66, 0xAF, 0x21, 0xA8, 0xEB, 0x19, 0xDB, 0xD8, 0xDB, 0xC3, 0x70, 0x6D, 0x13, 0x53,
    0x63, 0xA0, 0xD6, 0x83, 0xD0, 0x46, 0x30, 0x4F, 0x5A, 0x83, 0x6B, 0xC1, 0xBC, 0x63, 0x28, 0x21, 0xAF, 0xE7, 0xA2, 0xF7, 0x5D,
    0xA3, 0xC5, 0x0A, 0xC7, 0x4C, 0x54, 0x5A, 0x75, 0x45, 0x62, 0x20, 0x41, 0x37, 0x16, 0x96, 0x63, 0xCF, 0xCC, 0x0B, 0x06, 0xE6,
    0x7E, 0x21, 0x09, 0xEB, 0xA4, 0x1B, 0xC6, 0x7F, 0xF2, 0x0C, 0xC8, 0xAC, 0x80, 0xD7, 0xB6, 0xEE, 0x1A, 0x95, 0x46, 0x5B, 0x3B,
    0x26, 0x57, 0x53, 0x3E, 0xA5, 0x6D, 0x92, 0xD5, 0x39, 0xE5, 0x06, 0x43, 0x60, 0xEA, 0x48, 0x50, 0xFE, 0xD2, 0xD1, 0xBF};
  key1.exponent = {0x03};
  key1.sha1 = {0xEE, 0x23, 0xB6, 0x16, 0xC9, 0x5C, 0x02, 0x65, 0x2A, 0xD1, 0x88, 0x60, 0xE4, 0x87, 0x87, 0xC0, 0x79, 0xE8, 0xE8, 0x5A};
  output[key1.index] = key1;

  types::Capk key2;
  key2.index = 0x09;
  key2.modulus = {0xEB, 0x37, 0x4D, 0xFC, 0x5A, 0x96, 0xB7, 0x1D, 0x28, 0x63, 0x87, 0x5E, 0xDA, 0x2E, 0xAF, 0xB9, 0x6B, 0x1B, 0x43,
    0x9D, 0x3E, 0xCE, 0x0B, 0x18, 0x26, 0xA2, 0x67, 0x2E, 0xEE, 0xFA, 0x79, 0x90, 0x28, 0x67, 0x76, 0xF8, 0xBD, 0x98, 0x9A, 0x15,
    0x14, 0x1A, 0x75, 0xC3, 0x84, 0xDF, 0xC1, 0x4F, 0xEF, 0x92, 0x43, 0xAA, 0xB3, 0x27, 0x07, 0x65, 0x9B, 0xE9, 0xE4, 0x79, 0x7A,
    0x24, 0x7C, 0x2F, 0x0B, 0x6D, 0x99, 0x37, 0x2F, 0x38, 0x4A, 0xF6, 0x2F, 0xE2, 0x3B, 0xC5, 0x4B, 0xCD, 0xC5, 0x7A, 0x9A, 0xCD,
    0x1D, 0x55, 0x85, 0xC3, 0x03, 0xF2, 0x01, 0xEF, 0x4E, 0x8B, 0x80, 0x6A, 0xFB, 0x80, 0x9D, 0xB1, 0xA3, 0xDB, 0x1C, 0xD1, 0x12,
    0xAC, 0x88, 0x4F, 0x16, 0x4A, 0x67, 0xB9, 0x9C, 0x7D, 0x6E, 0x5A, 0x8A, 0x6D, 0xF1, 0xD3, 0xCA, 0xE6, 0xD7, 0xED, 0x3D, 0x5B,
    0xE7, 0x25, 0xB2, 0xDE, 0x4A, 0xDE, 0x23, 0xFA, 0x67, 0x9B, 0xF4, 0xEB, 0x15, 0xA9, 0x3D, 0x8A, 0x6E, 0x29, 0xC7, 0xFF, 0xA1,
    0xA7, 0x0D, 0xE2, 0xE5, 0x4F, 0x59, 0x3D, 0x90, 0x8A, 0x3B, 0xF9, 0xEB, 0xBD, 0x76, 0x0B, 0xBF, 0xDC, 0x8D, 0xB8, 0xB5, 0x44,
    0x97, 0xE6, 0xC5, 0xBE, 0x0E, 0x4A, 0x4D, 0xAC, 0x29, 0xE5};
  key2.exponent = {0x03};
  key2.sha1 = {0xA0, 0x75, 0x30, 0x6E, 0xAB, 0x00, 0x45, 0xBA, 0xF7, 0x2C, 0xDD, 0x33, 0xB3, 0xB6, 0x78, 0x77, 0x9D, 0xE1, 0xF5, 0x27};
  output[key2.index] = key2;

  types::Capk key3;
  key3.index = 0x0A;
  key3.modulus = {0xB2, 0xAB, 0x1B, 0x6E, 0x9A, 0xC5, 0x5A, 0x75, 0xAD, 0xFD, 0x5B, 0xBC, 0x34, 0x49, 0x0E, 0x53, 0xC4, 0xC3, 0x38,
    0x1F, 0x34, 0xE6, 0x0E, 0x7F, 0xAC, 0x21, 0xCC, 0x2B, 0x26, 0xDD, 0x34, 0x46, 0x2B, 0x64, 0xA6, 0xFA, 0xE2, 0x49, 0x5E, 0xD1,
    0xDD, 0x38, 0x3B, 0x81, 0x38, 0xBE, 0xA1, 0x00, 0xFF, 0x9B, 0x7A, 0x11, 0x18, 0x17, 0xE7, 0xB9, 0x86, 0x9A, 0x97, 0x42, 0xB1,
    0x9E, 0x5C, 0x9D, 0xAC, 0x56, 0xF8, 0xB8, 0x82, 0x7F, 0x11, 0xB0, 0x5A, 0x08, 0xEC, 0xCF, 0x9E, 0x8D, 0x5E, 0x85, 0xB0, 0xF7,
    0xCF, 0xA6, 0x44, 0xEF, 0xF3, 0xE9, 0xB7, 0x96, 0x68, 0x8F, 0x38, 0xE0, 0x06, 0xDE, 0xB2, 0x1E, 0x10, 0x1C, 0x01, 0x02, 0x89,
    0x03, 0xA0, 0x60, 0x23, 0xAC, 0x5A, 0xAB, 0x86, 0x35, 0xF8, 0xE3, 0x07, 0xA5, 0x3A, 0xC7, 0x42, 0xBD, 0xCE, 0x6A, 0x28, 0x3F,
    0x58, 0x5F, 0x48, 0xEF};
  key3.exponent = {0x03};
  key3.sha1 = {0xC8, 0x8B, 0xE6, 0xB2, 0x41, 0x7C, 0x4F, 0x94, 0x1C, 0x93, 0x71, 0xEA, 0x35, 0xA3, 0x77, 0x15, 0x87, 0x67, 0xE4, 0xE3};
  output[key3.index] = key3;

  types::Capk key4;
  key4.index = 0x0B;
  key4.modulus = {0xCF, 0x9F, 0xDF, 0x46, 0xB3, 0x56, 0x37, 0x8E, 0x9A, 0xF3, 0x11, 0xB0, 0xF9, 0x81, 0xB2, 0x1A, 0x1F, 0x22, 0xF2,
    0x50, 0xFB, 0x11, 0xF5, 0x5C, 0x95, 0x87, 0x09, 0xE3, 0xC7, 0x24, 0x19, 0x18, 0x29, 0x34, 0x83, 0x28, 0x9E, 0xAE, 0x68, 0x8A,
    0x09, 0x4C, 0x02, 0xC3, 0x44, 0xE2, 0x99, 0x9F, 0x31, 0x5A, 0x72, 0x84, 0x1F, 0x48, 0x9E, 0x24, 0xB1, 0xBA, 0x00, 0x56, 0xCF,
    0xAB, 0x3B, 0x47, 0x9D, 0x0E, 0x82, 0x64, 0x52, 0x37, 0x5D, 0xCD, 0xBB, 0x67, 0xE9, 0x7E, 0xC2, 0xAA, 0x66, 0xF4, 0x60, 0x1D,
    0x77, 0x4F, 0xEA, 0xEF, 0x77, 0x5A, 0xCC, 0xC6, 0x21, 0xBF, 0xEB, 0x65, 0xFB, 0x00, 0x53, 0xFC, 0x5F, 0x39, 0x2A, 0xA5, 0xE1,
    0xD4, 0xC4, 0x1A, 0x4D, 0xE9, 0xFF, 0xDF, 0xDF, 0x13, 0x27, 0xC4, 0xBB, 0x87, 0x4F, 0x1F, 0x63, 0xA5, 0x99, 0xEE, 0x39, 0x02,
    0xFE, 0x95, 0xE7, 0x29, 0xFD, 0x78, 0xD4, 0x23, 0x4D, 0xC7, 0xE6, 0xCF, 0x1A, 0xBA, 0xBA, 0xA3, 0xF6, 0xDB, 0x29, 0xB7, 0xF0,
    0x5D, 0x1D, 0x90, 0x1D, 0x2E, 0x76, 0xA6, 0x06, 0xA8, 0xCB, 0xFF, 0xFF, 0xEC, 0xBD, 0x91, 0x8F, 0xA2, 0xD2, 0x78, 0xBD, 0xB4,
    0x3B, 0x04, 0x34, 0xF5, 0xD4, 0x51, 0x34, 0xBE, 0x1C, 0x27, 0x81, 0xD1, 0x57, 0xD5, 0x01, 0xFF, 0x43, 0xE5, 0xF1, 0xC4, 0x70,
    0x96, 0x7C, 0xD5, 0x7C, 0xE5, 0x3B, 0x64, 0xD8, 0x29, 0x74, 0xC8, 0x27, 0x59, 0x37, 0xC5, 0xD8, 0x50, 0x2A, 0x12, 0x52, 0xA8,
    0xA5, 0xD6, 0x08, 0x8A, 0x25, 0x9B, 0x69, 0x4F, 0x98, 0x64, 0x8D, 0x9A, 0xF2, 0xCB, 0x0E, 0xFD, 0x9D, 0x94, 0x3C, 0x69, 0xF8,
    0x96, 0xD4, 0x9F, 0xA3, 0x97, 0x02, 0x16, 0x2A, 0xCB, 0x5A, 0xF2, 0x9B, 0x90, 0xBA, 0xDE, 0x00, 0x5B, 0xC1, 0x57};
  key4.exponent = {0x03};
  key4.sha1 = {0xBD, 0x33, 0x1F, 0x99, 0x96, 0xA4, 0x90, 0xB3, 0x3C, 0x13, 0x44, 0x10, 0x66, 0xA0, 0x9A, 0xD3, 0xFE, 0xB5, 0xF6, 0x6C};
  output[key4.index] = key4;

  types::Capk key5;
  key5.index = 0x0C;
  key5.modulus = {0xDE, 0xD9, 0xE1, 0xBC, 0x8E, 0x74, 0x9C, 0xAD, 0x74, 0x94, 0x84, 0xBF, 0xB4, 0x72, 0x44, 0x5B, 0xC8, 0x1F, 0xFA,
    0xA8, 0x97, 0x07, 0x64, 0x8C, 0x34, 0x2A, 0xA3, 0x0D, 0x1B, 0xE6, 0x0D, 0x5E, 0xD0, 0xF6, 0xCE, 0xAB, 0xA2, 0x5C, 0x68, 0x3D,
    0x45, 0x03, 0xCB, 0x11, 0xCA, 0xF9, 0x1A, 0x39, 0x72, 0x75, 0x93, 0xCF, 0x2B, 0xEE, 0xAE, 0x80, 0x32, 0xEF, 0xAC, 0xC4, 0x4F,
    0xDF, 0x8D, 0xA3, 0x1D, 0x60, 0x07, 0x13, 0x9D, 0x45, 0x95, 0xE8, 0x65, 0x5C, 0x74, 0x95, 0xCF, 0x46, 0xA9, 0xD5, 0x93, 0xA8,
    0x3E, 0x3C, 0x65, 0xB2, 0xCB, 0xF2, 0xAF, 0x1E, 0xEA, 0x02, 0xD1, 0xF9, 0x69, 0x51, 0xA9, 0x46, 0x61, 0x6B, 0x5A, 0xB2, 0x1C,
    0xA0, 0xBF, 0x34, 0xD1, 0x2D, 0x05, 0xF6, 0xAE, 0x18, 0x35, 0x08, 0xA7, 0xAC, 0x7A, 0x46, 0x91, 0x3B, 0xDC, 0xE5, 0xFD, 0xC3,
    0x91, 0x4C, 0xA7, 0x50, 0x01, 0x8B, 0x13, 0x0C, 0xA5, 0xBA, 0xD4, 0x9A, 0xD8, 0xC0, 0x22, 0x91, 0xAC, 0xA5, 0xCF, 0xFD};
  key5.exponent = {0x03};
  key5.sha1 = {0xD7, 0xDD, 0x7A, 0xAC, 0x8B, 0x67, 0xA9, 0xA3, 0xCC, 0x72, 0xF3, 0x5F, 0x5D, 0x96, 0xF2, 0x65, 0xE1, 0x6E, 0xB3, 0xFE};
  output[key5.index] = key5;

  return output;
}

}
