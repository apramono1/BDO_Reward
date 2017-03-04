#ifndef BDOR__CARD_VALIDATION_MESSAGE_H_
#define BDOR__CARD_VALIDATION_MESSAGE_H_

#include <vector>
#include <bdor/transaction.h>
#include <iso8583/apdu.h>


namespace bdor {

iso8583::Apdu BuildCardValidationRequest(const Transaction& tx);

bool ReadAndValidateCardValidationResponse(const iso8583::Apdu& request_apdu,
                      const std::vector<uint8_t>& data,
                      Transaction& tx);

iso8583::Apdu BuildCardValidationRequestWithDccEnquiry(const Transaction& tx);

bool ReadAndValidateCardValidationResponseWithDccEnquiry(const iso8583::Apdu& request_apdu,
                      const std::vector<uint8_t>& data,
                      Transaction& tx);

iso8583::Apdu BuildCardValidationRequestWithDccAllowed(const Transaction& tx);

bool ReadAndValidateCardValidationResponseWithDccAllowed(const iso8583::Apdu& request_apdu,
                      const std::vector<uint8_t>& data,
                      Transaction& tx);

}

#endif
