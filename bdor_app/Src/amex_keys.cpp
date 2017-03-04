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

std::map<unsigned int, types::Capk> GetAmexTestCapks() {
  std::map<unsigned int, types::Capk> output;

  types::Capk key1;
  key1.index = 0xC8;
  key1.modulus = {0xBF, 0x0C, 0xFC, 0xED, 0x70, 0x8F, 0xB6, 0xB0, 0x48, 0xE3, 0x01, 0x43, 0x36, 0xEA, 0x24,
    0xAA, 0x00, 0x7D, 0x79, 0x67, 0xB8, 0xAA, 0x4E, 0x61, 0x3D, 0x26, 0xD0, 0x15, 0xC4, 0xFE, 0x78, 0x05,
    0xD9, 0xDB, 0x13, 0x1C, 0xED, 0x0D, 0x2A, 0x8E, 0xD5, 0x04, 0xC3, 0xB5, 0xCC, 0xD4, 0x8C, 0x33, 0x19,
    0x9E, 0x5A, 0x5B, 0xF6, 0x44, 0xDA, 0x04, 0x3B, 0x54, 0xDB, 0xF6, 0x02, 0x76, 0xF0, 0x5B, 0x17, 0x50,
    0xFA, 0xB3, 0x90, 0x98, 0xC7, 0x51, 0x1D, 0x04, 0xBA, 0xBC, 0x64, 0x94, 0x82, 0xDD, 0xCF, 0x7C, 0xC4,
    0x2C, 0x8C, 0x43, 0x5B, 0xAB, 0x8D, 0xD0, 0xEB, 0x1A, 0x62, 0x0C, 0x31, 0x11, 0x1D, 0x1A, 0xAA, 0xF9,
    0xAF, 0x65, 0x71, 0xEE, 0xBD, 0x4C, 0xF5, 0xA0, 0x84, 0x96, 0xD5, 0x7E, 0x7A, 0xBD, 0xBB, 0x51, 0x80,
    0xE0, 0xA4, 0x2D, 0xA8, 0x69, 0xAB, 0x95, 0xFB, 0x62, 0x0E, 0xFF, 0x26, 0x41, 0xC3, 0x70, 0x2A, 0xF3,
    0xBE, 0x0B, 0x0C, 0x13, 0x8E, 0xAE, 0xF2, 0x02, 0xE2, 0x1D};
  key1.exponent = {0x03};
  output[key1.index] = key1;

  types::Capk key2;
  key2.index = 0xC9;
  key2.modulus = {0xB3, 0x62, 0xDB, 0x57, 0x33, 0xC1, 0x5B, 0x87, 0x97, 0xB8, 0xEC, 0xEE, 0x55, 0xCB, 0x1A,
    0x37, 0x1F, 0x76, 0x0E, 0x0B, 0xED, 0xD3, 0x71, 0x5B, 0xB2, 0x70, 0x42, 0x4F, 0xD4, 0xEA, 0x26, 0x06,
    0x2C, 0x38, 0xC3, 0xF4, 0xAA, 0xA3, 0x73, 0x2A, 0x83, 0xD3, 0x6E, 0xA8, 0xE9, 0x60, 0x2F, 0x66, 0x83,
    0xEE, 0xCC, 0x6B, 0xAF, 0xF6, 0x3D, 0xD2, 0xD4, 0x90, 0x14, 0xBD, 0xE4, 0xD6, 0xD6, 0x03, 0xCD, 0x74,
    0x42, 0x06, 0xB0, 0x5B, 0x4B, 0xAD, 0x0C, 0x64, 0xC6, 0x3A, 0xB3, 0x97, 0x6B, 0x5C, 0x8C, 0xAA, 0xF8,
    0x53, 0x95, 0x49, 0xF5, 0x92, 0x1C, 0x0B, 0x70, 0x0D, 0x5B, 0x0F, 0x83, 0xC4, 0xE7, 0xE9, 0x46, 0x06,
    0x8B, 0xAA, 0xAB, 0x54, 0x63, 0x54, 0x4D, 0xB1, 0x8C, 0x63, 0x80, 0x11, 0x18, 0xF2, 0x18, 0x2E, 0xFC,
    0xC8, 0xA1, 0xE8, 0x5E, 0x53, 0xC2, 0xA7, 0xAE, 0x83, 0x9A, 0x5C, 0x6A, 0x3C, 0xAB, 0xE7, 0x37, 0x62,
    0xB7, 0x0D, 0x17, 0x0A, 0xB6, 0x4A, 0xFC, 0x6C, 0xA4, 0x82, 0x94, 0x49, 0x02, 0x61, 0x1F, 0xB0, 0x06,
    0x1E, 0x09, 0xA6, 0x7A, 0xCB, 0x77, 0xE4, 0x93, 0xD9, 0x98, 0xA0, 0xCC, 0xF9, 0x3D, 0x81, 0xA4, 0xF6,
    0xC0, 0xDC, 0x6B, 0x7D, 0xF2, 0x2E, 0x62, 0xDB};
  key2.exponent = {0x03};
  output[key2.index] = key2;

  types::Capk key3;
  key3.index = 0xCA;
  key3.modulus = {0xC2, 0x3E, 0xCB, 0xD7, 0x11, 0x9F, 0x47, 0x9C, 0x2E, 0xE5, 0x46, 0xC1, 0x23, 0xA5, 0x85, 0xD6,
    0x97, 0xA7, 0xD1, 0x0B, 0x55, 0xC2, 0xD2, 0x8B, 0xEF, 0x0D, 0x29, 0x9C, 0x01, 0xDC, 0x65, 0x42, 0x0A, 0x03,
    0xFE, 0x52, 0x27, 0xEC, 0xDE, 0xCB, 0x80, 0x25, 0xFB, 0xC8, 0x6E, 0xEB, 0xC1, 0x93, 0x52, 0x98, 0xC1, 0x75,
    0x3A, 0xB8, 0x49, 0x93, 0x67, 0x49, 0x71, 0x95, 0x91, 0x75, 0x8C, 0x31, 0x5F, 0xA1, 0x50, 0x40, 0x07, 0x89,
    0xBB, 0x14, 0xFA, 0xDD, 0x6E, 0xAE, 0x2A, 0xD6, 0x17, 0xDA, 0x38, 0x16, 0x31, 0x99, 0xD1, 0xBA, 0xD5, 0xD3,
    0xF8, 0xF6, 0xA7, 0xA2, 0x0A, 0xEF, 0x42, 0x0A, 0xDF, 0xE2, 0x40, 0x4D, 0x30, 0xB2, 0x19, 0x35, 0x9C, 0x6A,
    0x49, 0x52, 0x56, 0x5C, 0xCC, 0xA6, 0xF1, 0x1E, 0xC5, 0xBE, 0x56, 0x4B, 0x49, 0xB0, 0xEA, 0x5B, 0xF5, 0xB3,
    0xDC, 0x8C, 0x5C, 0x64, 0x01, 0x20, 0x8D, 0x00, 0x29, 0xC3, 0x95, 0x7A, 0x8C, 0x59, 0x22, 0xCB, 0xDE, 0x39,
    0xD3, 0xA5, 0x64, 0xC6, 0xDE, 0xBB, 0x6B, 0xD2, 0xAE, 0xF9, 0x1F, 0xC2, 0x7B, 0xB3, 0xD3, 0x89, 0x2B, 0xEB,
    0x96, 0x46, 0xDC, 0xE2, 0xE1, 0xEF, 0x85, 0x81, 0xEF, 0xFA, 0x71, 0x21, 0x58, 0xAA, 0xEC, 0x54, 0x1C, 0x0B,
    0xBB, 0x4B, 0x3E, 0x27, 0x9D, 0x7D, 0xA5, 0x4E, 0x45, 0xA0, 0xAC, 0xC3, 0x57, 0x0E, 0x71, 0x2C, 0x9F, 0x7C,
    0xDF, 0x98, 0x5C, 0xFA, 0xFD, 0x38, 0x2A, 0xE1, 0x3A, 0x3B, 0x21, 0x4A, 0x9E, 0x8E, 0x1E, 0x71, 0xAB, 0x1E,
    0xA7, 0x07, 0x89, 0x51, 0x12, 0xAB, 0xC3, 0xA9, 0x7D, 0x0F, 0xCB, 0x0A, 0xE2, 0xEE, 0x5C, 0x85, 0x49, 0x2B,
    0x6C, 0xFD, 0x54, 0x88, 0x5C, 0xDD, 0x63, 0x37, 0xE8, 0x95, 0xCC, 0x70, 0xFB, 0x32, 0x55, 0xE3};
  key3.exponent = {0x03};
  output[key3.index] = key3;

  return output;
}

}

