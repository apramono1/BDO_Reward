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

#include <sstream>

#include <iso8583/field_types.h>
#include "protocol.h"

using namespace iso8583;

namespace bdor {

const ApduSpec& bdor_spec() {
  static ApduSpec spec = ApduSpec().SetFieldSpecs(
      { { kFieldPan, Bcd2VarCompressedNumericRightPadded<32, 15>::spec() }, {
          kFieldProcessingCode, FixedCompressedNumeric<6>::spec() }, {
          kFieldAmount, FixedCompressedNumeric<12>::spec() }, {
          kFieldDccAmount, FixedCompressedNumeric<12>::spec() }, {
          kFieldDccConversionRate, FixedCompressedNumeric<8>::spec() }, {
          kFieldStan, FixedCompressedNumeric<6>::spec() }, {
          kFieldTimeLocalTransaction, FixedCompressedNumeric<6>::spec() }, {
          kFieldDateLocalTransaction, FixedCompressedNumeric<4>::spec() }, {
          kFieldDateExpiration, FixedCompressedNumeric<4>::spec() }, {
          kFieldPosEntryMode, FixedCompressedNumeric<4>::spec() }, {
          kFieldNii, FixedCompressedNumeric<4>::spec() }, {
          kFieldPosConditionCode, FixedCompressedNumeric<2>::spec() }, {
          kFieldTrack2Data, Track2Data<99>::spec() }, {  // TODO: check for ANS
          kFieldRrn, FixedAnsRightPadded<12>::spec() }, {
          kFieldAutorizationId, FixedAnsRightPadded<6>::spec() }, {
          kFieldResponseCode, FixedAnsRightPadded<2>::spec() }, {
          kFieldCardAcceptorTerminalId, FixedAnsRightPadded<8>::spec() }, {
          kFieldCardAcceptorId, FixedAnsRightPadded<15>::spec() }, {
          kFieldCurrencyCode, FixedAnsRightPadded<3>::spec() }, {
          kFieldPinBlock, FixedBinary<64>::spec() }, {
          kFieldAdditionalAmount, Bcd3VarAns<12>::spec() },{
          kFieldIccData, Bcd3VarBytes<255>::spec() }, {  // TODO: double confirm
          kField60, Bcd3VarAns<22>::spec() }, {  // TODO: to check, specification not clear...
          kField62, Bcd3VarAns<85>::spec() }, {  // TODO: to check, specification not clear...
          kField63, Bcd3VarBytes<256>::spec() }, { // TODO: to check, specification not clear...
  	  	  kField64, FixedBinary<64>::spec() }, }); // TODO: to check, specification not clear...

  return spec;
}

}




