/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2015-2016 INGENICO S.A.
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
#ifndef BDOR__MESSAGE_H_
#define BDOR__MESSAGE_H_

#include <vector>
#include <stdint.h>
#include <iso8583/apdu.h>
#include <bdor/transaction.h>
#include <bdor/batch.h>
#include <bdor/key_exchange.h>
#include <bdor/settlement_data.h>
#include <bdor/test_transaction.h>

namespace bdor {

iso8583::Apdu BuildRefundRequest(Transaction& tx);
bool ReadRefundResponse(const std::vector<uint8_t>& data,
                        Transaction& tx);

iso8583::Apdu BuildPreAuthRequest(Transaction& tx);
bool ReadPreAuthResponse(const std::vector<uint8_t>& data,
                         Transaction& tx);

iso8583::Apdu BuildPreAuthCancellationRequest(Transaction& tx);
bool ReadPreAuthCancellationResponse(const std::vector<uint8_t>& data,
                                   Transaction& tx);

iso8583::Apdu BuildPreAuthCompletionRequest(Transaction& tx);
bool ReadPreAuthCompletionResponse(const std::vector<uint8_t>& data,
                                   Transaction& tx);

iso8583::Apdu BuildOfflineSaleRequest(Transaction& tx);
bool ReadOfflineSaleResponse(const std::vector<uint8_t>& data,
                             Transaction& tx);

iso8583::Apdu BuildTipAdjustRequest(Transaction& tx);
bool ReadTipAdjustResponse(const std::vector<uint8_t>& data, Transaction& tx);

iso8583::Apdu BuildInstalmentSaleRequest(Transaction& tx);
bool ReadInstalmentSaleResponse(const std::vector<uint8_t>& data, Transaction& tx);

iso8583::Apdu BuildKeyExchangeRequest(KeyExchange& tx);
bool ReadKeyExchangeResponse(const std::vector<uint8_t>& data, KeyExchange& tx);

iso8583::Apdu BuildTcUploadRequest(Transaction& tx);
bool ReadTcUploadResponse(const std::vector<uint8_t>& data, Transaction& tx);

iso8583::Apdu BuildVoidTransactionRequest(Transaction& tx);
bool ReadVoidTransactionResponse(const std::vector<uint8_t>& data,
                                 Transaction& tx);

iso8583::Apdu BuildBatchUploadRequest(Transaction& tx);
bool ReadBatchUploadResponse(const std::vector<uint8_t>& data,
                             Transaction& tx);

iso8583::Apdu BuildSettlementRequest(SettlementData& settlement_data, bool after_batch_upload);
bool ReadSettlemenResponse(const std::vector<uint8_t>& data,
                           SettlementData& settlement_data);

iso8583::Apdu BuildReversalRequest(Transaction& tx);
bool ReadReversalResponse(const std::vector<uint8_t>& data,
                          Transaction& tx);

iso8583::Apdu BuildTestTransactionRequest(TestTransaction& test_tx);
bool ReadTestTransactionResponse(const std::vector<uint8_t>& data,
                                 TestTransaction& test_tx);

}

#endif
