/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2014 INGENICO S.A.
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

#include <stdx/memory>
#include <stdx/optional>
#include <gtest/gtest.h>
#include <types/pos_entry_mode.h>
#include <types/pos_condition_code.h>
#include <bdor/sqlite_batch.h>

using std::unique_ptr;
using stdx::make_unique;
using namespace bdor;

class BatchTest : public ::testing::Test {
 protected:

  BatchTest() {
    remove("batch.db");
  }

  virtual ~BatchTest() {
    remove("batch.db");
  }

  virtual void SetUp() {
    batch_ = make_unique<SqliteBatch>(app_settings_, "batch.db", true);
  }

  virtual void TearDown() {
    // release all sqlite objects
    batch_ = nullptr;
  }

  ApplicationSettings app_settings_;
  unique_ptr<Batch> batch_;
};

TEST_F(BatchTest, UnknownBatchStatus) {
  ASSERT_EQ(BATCH_CLOSED, batch_->GetBatchStatus(1));
}

TEST_F(BatchTest, OpenNewBatch) {
  unsigned int index = 1;
  bool ret;

  ASSERT_FALSE(batch_->IsBatchOpen(index));

  ret = batch_->OpenBatch(index);
  ASSERT_TRUE(ret);
  ASSERT_EQ(BATCH_OPEN, batch_->GetBatchStatus(index));
  ASSERT_NE(0, batch_->GetCurrentBatchNumber(index));
  ASSERT_NE(0, batch_->GetBatchInfo(index).opening_datetime);
  ASSERT_EQ(index, batch_->GetBatchInfo(index).host_index);
  ASSERT_TRUE(batch_->IsBatchOpen(index));
}

TEST_F(BatchTest, LockBatch) {
  int index = 1;
  bool ret;

  ret = batch_->OpenBatch(index);
  ASSERT_TRUE(ret);
  ASSERT_EQ(BATCH_OPEN, batch_->GetBatchStatus(index));
  ASSERT_FALSE(batch_->IsBatchLocked(index));

  ret = batch_->LockBatch(index);
  ASSERT_TRUE(ret);
  ASSERT_EQ(BATCH_SETTLE_IN_PROGRESS, batch_->GetBatchStatus(index));
  ASSERT_TRUE(batch_->IsBatchLocked(index));
}

TEST_F(BatchTest, AttemptOpenLockedBatch) {
  int index = 1;
  bool ret;

  ret = batch_->OpenBatch(index);
  ASSERT_TRUE(ret);
  ASSERT_EQ(BATCH_OPEN, batch_->GetBatchStatus(index));
  ASSERT_TRUE(batch_->IsBatchOpen(index));

  ret = batch_->LockBatch(index);
  ASSERT_TRUE(ret);
  ASSERT_FALSE(batch_->IsBatchOpen(index));

  ret = batch_->OpenBatch(index);
  ASSERT_FALSE(ret);
  ASSERT_EQ(BATCH_SETTLE_IN_PROGRESS, batch_->GetBatchStatus(index));
  ASSERT_FALSE(batch_->IsBatchOpen(index));
}

TEST_F(BatchTest, CloseBatch) {
  int index = 1;
  bool ret;

  ret = batch_->OpenBatch(index);
  ASSERT_TRUE(ret);
  ASSERT_EQ(BATCH_OPEN, batch_->GetBatchStatus(index));
  ASSERT_FALSE(batch_->IsBatchClosed(index));

  Transaction txn1;
  txn1.tid = "12345678";
  txn1.nii = 111;
  txn1.invoice_num = 5;
  txn1.host_index = index;
  txn1.secondary_amount = types::Amount("INR", 1000);
  ret = batch_->Insert(txn1);

  auto all_tx = batch_->GetAllTransactions(index);
  ASSERT_EQ(1u, all_tx.size());

  ret = batch_->CloseBatch(index);
  ASSERT_TRUE(ret);
  ASSERT_TRUE(batch_->IsBatchClosed(index));

  all_tx = batch_->GetAllTransactions(index);
  ASSERT_EQ(0u, all_tx.size());
  ASSERT_EQ(BATCH_CLOSED, batch_->GetBatchStatus(index));
}

TEST_F(BatchTest, TestInsertAndFind1) {
  Transaction txn1;

  txn1.tid = "12345678";
  txn1.nii = 111;
  txn1.invoice_num = 5;
  txn1.host_index = 1;
  txn1.secondary_amount = types::Amount("INR", 1000);
  txn1.pos_entry_mode = types::PosEntryMode::CHIP;
  txn1.pos_condition_code = types::PosConditionCode::NORMAL;
  bool ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  auto v = batch_->FindTransactionByInvoice(2);
  EXPECT_TRUE(v.empty());

  auto v1 = batch_->FindTransactionByInvoice(5);
  ASSERT_EQ(1u, v1.size());
  EXPECT_EQ(txn1.tid, v1[0].tid);
  EXPECT_EQ(txn1.nii, v1[0].nii);
  EXPECT_EQ(txn1.invoice_num, v1[0].invoice_num);
  EXPECT_EQ(txn1.host_index, v1[0].host_index);
  EXPECT_FALSE(v1[0].pan.has_value());
  EXPECT_EQ(1000u, v1[0].secondary_amount->GetValue());
  EXPECT_FALSE(bool(txn1.pan_sequence_number));

  EXPECT_EQ(types::PosEntryMode::CHIP, v1[0].pos_entry_mode);
  EXPECT_EQ(types::PosConditionCode::NORMAL, v1[0].pos_condition_code);
}

TEST_F(BatchTest, TestInsertAndFind2) {
  Transaction txn1;
  txn1.tid = "11111111";
  txn1.nii = 111;
  txn1.invoice_num = 1;
  txn1.host_index = 1;
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.secondary_amount = types::Amount("INR", 1000);
  txn1.pos_entry_mode = types::PosEntryMode::MANUAL;
  txn1.pos_condition_code = types::PosConditionCode::MOTO;
  bool ret = batch_->InsertOrUpdate(txn1);
  EXPECT_TRUE(ret);

  auto v1 = batch_->FindTransactionByInvoice(1);
  ASSERT_EQ(1u, v1.size());
  EXPECT_EQ("11111111", v1[0].tid);
  EXPECT_EQ(111u, v1[0].nii);
  EXPECT_EQ(1u, v1[0].invoice_num);
  EXPECT_EQ(1000u, v1[0].secondary_amount->GetValue());
  EXPECT_EQ(types::PosEntryMode::MANUAL, v1[0].pos_entry_mode);
  EXPECT_EQ(types::PosConditionCode::MOTO, v1[0].pos_condition_code);

  Transaction txn2;
  txn2.tid = "22222222";
  txn2.nii = 222;
  txn2.invoice_num = 2;
  txn2.host_index = 3;
  txn2.transaction_status = TransactionStatus::APPROVED;
  txn2.secondary_amount = types::Amount("INR", 2000);
  ret = batch_->InsertOrUpdate(txn2);
  EXPECT_TRUE(ret);

  auto v2 = batch_->FindTransactionByInvoice(2);
  ASSERT_EQ(1u, v2.size());
  EXPECT_EQ("22222222", v2[0].tid);
  EXPECT_EQ(222u, v2[0].nii);
  EXPECT_EQ(2u, v2[0].invoice_num);
  EXPECT_EQ(2000u, v2[0].secondary_amount->GetValue());

  auto v3 = batch_->GetAllTransactions(2);
  EXPECT_EQ(0u, v3.size());
  auto v4 = batch_->GetAllTransactions(1);
  ASSERT_FALSE(v4.empty());
  EXPECT_EQ(1u, v4[0].host_index);
  auto v5 = batch_->GetAllTransactions(3);
  ASSERT_FALSE(v5.empty());
  EXPECT_EQ(3u, v5[0].host_index);
  EXPECT_EQ("22222222", v5[0].tid);
  EXPECT_EQ(111u, v4[0].nii);
  EXPECT_EQ(1000u, v4[0].secondary_amount->GetValue());
  EXPECT_EQ(2u, v5[0].invoice_num);
  EXPECT_EQ(2000u, v5[0].secondary_amount->GetValue());
}

TEST_F(BatchTest, TestInsertAndUpdate1) {
  Transaction txn1;
  txn1.tid = "12345678";
  txn1.nii = 111;
  txn1.host_index = 2;
  txn1.invoice_num = 1;
  txn1.host_index = 2;
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.secondary_amount = types::Amount("INR", 1000);

  bool ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  auto v1 = batch_->FindTransactionByInvoice(1);
  EXPECT_EQ(1u, v1.size());

  txn1.tid = "11111111";
  txn1.nii = 222;
  txn1.host_index = 3;
  txn1.secondary_amount = types::Amount("INR", 2000);

  ret = batch_->Update(txn1);
  EXPECT_TRUE(ret);

  auto v2 = batch_->GetAllTransactions(3);
  ASSERT_EQ(1u, v2.size());
  EXPECT_EQ("11111111", v2[0].tid);
  EXPECT_EQ(222u, v2[0].nii);
  EXPECT_EQ(1u, v2[0].invoice_num);
  EXPECT_EQ(3u, v2[0].host_index);
  EXPECT_EQ(2000u, v2[0].secondary_amount->GetValue());
}

TEST_F(BatchTest, TestInsertAndUpdate2) {
  Transaction txn1;
  txn1.tid = "12345678";
  txn1.nii = 111;
  txn1.invoice_num = 1;
  txn1.host_index = 3;
  bool ret = batch_->InsertOrUpdate(txn1);
  EXPECT_TRUE(ret);

  auto v1 = batch_->FindTransactionByInvoice(5);
  ASSERT_TRUE(v1.size() == 0);

  txn1.tid = "11111111";
  txn1.nii = 222;
  txn1.host_index = 1;
  txn1.transaction_status = TransactionStatus::APPROVED;
  ret = batch_->InsertOrUpdate(txn1);
  EXPECT_TRUE(ret);

  auto v2 = batch_->GetAllTransactions(1);
  EXPECT_FALSE(v2.empty());
  ASSERT_TRUE(v2.size() == 1);
  EXPECT_EQ("11111111", v2[0].tid);
  EXPECT_EQ(222u, v2[0].nii);
  EXPECT_EQ(1u, v2[0].invoice_num);
  EXPECT_EQ(1u, v2[0].host_index);
}

TEST_F(BatchTest, TestDelete) {
  Transaction txn1;

  bool ret = batch_->Delete(txn1);
  EXPECT_TRUE(ret);

  txn1.tid = "12345678";
  txn1.nii = 111;
  txn1.invoice_num = 1;
  txn1.host_index = 2;
  ret = batch_->InsertOrUpdate(txn1);
  EXPECT_TRUE(ret);

  auto v1 = batch_->FindTransactionByInvoice(1);
  EXPECT_EQ(1u, v1.size());

  Transaction txn2;
  txn2.tid = "22222222";
  txn2.nii = 222;
  txn2.invoice_num = 2;
  txn2.host_index = 1;
  txn2.transaction_status = TransactionStatus::APPROVED;
  ret = batch_->InsertOrUpdate(txn2);
  EXPECT_TRUE(ret);

  auto v2 = batch_->GetAllTransactions(1);
  EXPECT_EQ(1u, v2.size());

  txn1.tid = "22222222";
  txn1.invoice_num = 3;
  txn2.tid = "11111111";
  txn2.nii = 123;

  ret = batch_->InsertOrUpdate(txn1);
  EXPECT_TRUE(ret);
  ret = batch_->InsertOrUpdate(txn2);
  EXPECT_TRUE(ret);
  ret = batch_->Delete(txn1);
  EXPECT_TRUE(ret);

  auto v3 = batch_->GetAllTransactions(1);
  ASSERT_EQ(1u, v3.size());
  EXPECT_EQ("11111111", v3[0].tid);
  EXPECT_EQ(123u, v3[0].nii);
  EXPECT_EQ(2u, v3[0].invoice_num);
}

TEST_F(BatchTest, TestGetPendingAdvicesForOfflineSale) {
  Transaction txn1;
  txn1.tid = "11111111";
  txn1.nii = 111;
  txn1.invoice_num = 1;
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.online_authorization_required = true;
  txn1.transaction_type = TransactionType::PREAUTH;
  txn1.host_index = 2;
  bool ret = batch_->InsertOrUpdate(txn1);
  EXPECT_TRUE(ret);

  Transaction txn2;
  txn2.tid = "22222222";
  txn2.nii = 222;
  txn2.invoice_num = 2;
  txn2.transaction_status = TransactionStatus::IN_PROGRESS;
  txn2.online_authorization_required = false;
  txn2.transaction_type = TransactionType::SALE;
  txn2.host_index = 1;
  ret = batch_->InsertOrUpdate(txn2);
  EXPECT_TRUE(ret);

  Transaction txn3;
  txn3.tid = "33333333";
  txn3.nii = 333;
  txn3.invoice_num = 3;
  txn3.transaction_status = TransactionStatus::TO_REVERSE;
  txn3.online_authorization_required = true;
  txn3.transaction_type = TransactionType::SALE;
  txn3.host_index = 1;
  ret = batch_->InsertOrUpdate(txn3);
  EXPECT_TRUE(ret);

  Transaction txn4;
  txn4.tid = "44444444";
  txn4.nii = 444;
  txn4.invoice_num = 4;
  txn4.transaction_status = TransactionStatus::TO_ADVISE;
  txn4.online_authorization_required = false;
  txn4.transaction_type = TransactionType::SALE;
  txn4.host_index = 2;
  ret = batch_->InsertOrUpdate(txn4);
  EXPECT_TRUE(ret);

  Transaction txn5;
  txn5.tid = "55555555";
  txn5.nii = 555;
  txn5.invoice_num = 5;
  txn5.transaction_status = TransactionStatus::IN_PROGRESS;
  txn5.online_authorization_required = false;
  txn5.transaction_type = TransactionType::PREAUTH;
  txn5.host_index = 1;
  ret = batch_->InsertOrUpdate(txn5);
  EXPECT_TRUE(ret);

  Transaction txn6;
  txn6.tid = "66666666";
  txn6.nii = 666;
  txn6.invoice_num = 6;
  txn6.transaction_status = TransactionStatus::TO_ADVISE;
  txn6.online_authorization_required = false;
  txn6.transaction_type = REFUND;
  txn6.host_index = 1;
  ret = batch_->InsertOrUpdate(txn6);
  EXPECT_TRUE(ret);

  auto v = batch_->GetPendingAdvices(1);
  ASSERT_EQ(1u, v.size());
  EXPECT_EQ("66666666", v[0].tid);
  EXPECT_EQ(666u, v[0].nii);
  EXPECT_EQ(6u, v[0].invoice_num);
  EXPECT_EQ(TransactionStatus::TO_ADVISE, v[0].transaction_status);
  EXPECT_FALSE(v[0].online_authorization_required);
  EXPECT_EQ(REFUND, v[0].transaction_type);
}

TEST_F(BatchTest, TestGetReversal) {
  Transaction txn1;
  txn1.tid = "11111111";
  txn1.nii = 111;
  txn1.invoice_num = 1;
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.online_authorization_required = true;
  txn1.transaction_type = TransactionType::PREAUTH;
  bool ret = batch_->InsertOrUpdate(txn1);
  EXPECT_TRUE(ret);

  Transaction txn2;
  txn2.tid = "22222222";
  txn2.nii = 222;
  txn2.invoice_num = 2;
  txn2.transaction_status = TransactionStatus::CANCELLED;
  txn2.online_authorization_required = false;
  txn2.transaction_type = TransactionType::SALE;
  ret = batch_->InsertOrUpdate(txn2);
  EXPECT_TRUE(ret);

  Transaction txn3;
  txn3.tid = "33333333";
  txn3.nii = 333;
  txn3.invoice_num = 3;
  txn3.transaction_status = TransactionStatus::TO_REVERSE;
  txn3.online_authorization_required = true;
  txn3.transaction_type = TransactionType::SALE;
  txn3.host_index = 1;
  ret = batch_->InsertOrUpdate(txn3);
  EXPECT_TRUE(ret);

  Transaction txn4;
  txn4.tid = "44444444";
  txn4.nii = 444;
  txn4.invoice_num = 4;
  txn4.transaction_status = TransactionStatus::DECLINED;
  txn4.online_authorization_required = false;
  txn4.transaction_type = TransactionType::PREAUTH;
  ret = batch_->InsertOrUpdate(txn4);
  EXPECT_TRUE(ret);

  Transaction txn5;
  txn5.tid = "55555555";
  txn5.nii = 555;
  txn5.invoice_num = 5;
  txn5.transaction_status = TransactionStatus::IN_PROGRESS;
  txn5.online_authorization_required = false;
  txn5.transaction_type = TransactionType::PREAUTH;
  txn5.host_index = 1;
  ret = batch_->InsertOrUpdate(txn5);
  EXPECT_TRUE(ret);

  auto v = batch_->GetReversal(1);
  ASSERT_EQ(2u, v.size());
  EXPECT_EQ("33333333", v[0].tid);
  EXPECT_EQ(333u, v[0].nii);
  EXPECT_EQ(3u, v[0].invoice_num);
  EXPECT_EQ(TransactionStatus::IN_PROGRESS, v[1].transaction_status);
  EXPECT_FALSE(v[1].online_authorization_required);
  EXPECT_EQ(PREAUTH, v[1].transaction_type);
}

TEST_F(BatchTest, TestGetPendingPreAuthSale) {
  Transaction txn1;
  txn1.tid = "11111111";
  txn1.nii = 111;
  txn1.invoice_num = 1;
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.online_authorization_required = true;
  txn1.transaction_type = TransactionType::PREAUTH;
  txn1.is_preauth_completed = false;
  txn1.tx_datetime = stdx::time(NULL) - (22 * 24 * 60 * 60);
  txn1.host_index = 2;
  bool ret = batch_->InsertOrUpdate(txn1);
  EXPECT_TRUE(ret);

  Transaction txn2;
  txn2.tid = "22222222";
  txn2.nii = 222;
  txn2.invoice_num = 2;
  txn2.transaction_status = TransactionStatus::CANCELLED;
  txn2.online_authorization_required = false;
  txn2.transaction_type = TransactionType::SALE;
  txn2.tx_datetime = stdx::time(NULL) - (19 * 24 * 60 * 60);
  txn2.host_index = 3;
  ret = batch_->InsertOrUpdate(txn2);
  EXPECT_TRUE(ret);

  Transaction txn3;
  txn3.tid = "33333333";
  txn3.nii = 333;
  txn3.invoice_num = 3;
  txn3.transaction_status = TransactionStatus::TO_REVERSE;
  txn3.online_authorization_required = true;
  txn3.transaction_type = TransactionType::SALE;
  txn3.tx_datetime = stdx::time(NULL) - (30 * 24 * 60 * 60);
  txn3.host_index = 3;
  ret = batch_->InsertOrUpdate(txn3);
  EXPECT_TRUE(ret);

  Transaction txn4;
  txn4.tid = "44444444";
  txn4.nii = 444;
  txn4.invoice_num = 4;
  txn4.transaction_status = TransactionStatus::DECLINED;
  txn4.online_authorization_required = false;
  txn4.is_preauth_completed = true;
  txn4.transaction_type = TransactionType::PREAUTH;
  txn4.tx_datetime = stdx::time(NULL) - (1 * 24 * 60 * 60);
  txn4.host_index = 2;
  ret = batch_->InsertOrUpdate(txn4);
  EXPECT_TRUE(ret);

  Transaction txn5;
  txn5.tid = "55555555";
  txn5.nii = 555;
  txn5.invoice_num = 5;
  txn5.transaction_status = TransactionStatus::IN_PROGRESS;
  txn5.online_authorization_required = false;
  txn5.is_preauth_completed = false;
  txn5.transaction_type = TransactionType::PREAUTH;
  txn5.tx_datetime = stdx::time(NULL) - (20 * 24 * 60 * 60);
  txn5.host_index = 1;
  ret = batch_->InsertOrUpdate(txn5);
  EXPECT_TRUE(ret);

  Transaction txn6;
  txn6.tid = "66666666";
  txn6.nii = 666;
  txn6.invoice_num = 6;
  txn6.transaction_status = TransactionStatus::APPROVED;
  txn6.online_authorization_required = true;
  txn6.transaction_type = TransactionType::PREAUTH;
  txn6.is_preauth_completed = false;
  txn6.tx_datetime = stdx::time(NULL) - (18 * 24 * 60 * 60);
  txn6.host_index = 1;
  ret = batch_->InsertOrUpdate(txn6);
  EXPECT_TRUE(ret);

  Transaction txn7;
  txn7.tid = "77777777";
  txn7.nii = 777;
  txn7.invoice_num = 7;
  txn7.transaction_status = TransactionStatus::APPROVED;
  txn7.online_authorization_required = true;
  txn7.transaction_type = TransactionType::PREAUTH;
  txn7.is_preauth_completed = false;
  txn7.tx_datetime = stdx::time(NULL) - (22 * 24 * 60 * 60);
  txn7.host_index = 3;
  ret = batch_->InsertOrUpdate(txn7);
  EXPECT_TRUE(ret);

  auto v1 = batch_->GetPendingPreAuth(2);
  ASSERT_EQ(1u, v1.size());
  EXPECT_EQ("11111111", v1[0].tid);
  EXPECT_EQ(111u, v1[0].nii);
  EXPECT_EQ(1u, v1[0].invoice_num);
  EXPECT_EQ(TransactionStatus::APPROVED, v1[0].transaction_status);
  EXPECT_TRUE(v1[0].online_authorization_required);
  EXPECT_EQ(PREAUTH, v1[0].transaction_type);
  EXPECT_EQ(2u, v1[0].host_index);

  auto v2 = batch_->GetPendingPreAuth(1);
  ASSERT_EQ(1u, v2.size());
  EXPECT_EQ("66666666", v2[0].tid);
  EXPECT_EQ(666u, v2[0].nii);
  EXPECT_EQ(6u, v2[0].invoice_num);
  EXPECT_EQ(TransactionStatus::APPROVED, v2[0].transaction_status);
  EXPECT_TRUE(v2[0].online_authorization_required);
  EXPECT_EQ(PREAUTH, v2[0].transaction_type);
  EXPECT_EQ(1u, v2[0].host_index);

  auto v3 = batch_->GetPendingPreAuth(3);
  ASSERT_EQ(1u, v3.size());
  EXPECT_EQ("77777777", v3[0].tid);
  EXPECT_EQ(777u, v3[0].nii);
  EXPECT_EQ(7u, v3[0].invoice_num);
  EXPECT_EQ(TransactionStatus::APPROVED, v3[0].transaction_status);
  EXPECT_TRUE(v3[0].online_authorization_required);
  EXPECT_EQ(PREAUTH, v3[0].transaction_type);
  EXPECT_EQ(3u, v3[0].host_index);
}

TEST_F(BatchTest, TestClearHost2Batch) {
  Transaction txn1;
  txn1.tid = "11111111";
  txn1.nii = 111;
  txn1.invoice_num = 1;
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.is_preauth_completed = true;
  txn1.transaction_type = TransactionType::PREAUTH;
  txn1.is_voided = false;
  txn1.host_index = 1;
  bool ret = batch_->InsertOrUpdate(txn1);
  EXPECT_TRUE(ret);

  Transaction txn2;
  txn2.tid = "22222222";
  txn2.nii = 222;
  txn2.invoice_num = 2;
  txn2.transaction_status = TransactionStatus::TO_ADVISE;
  txn2.is_preauth_completed = false;
  txn2.transaction_type = TransactionType::SALE;
  txn2.is_voided = false;
  txn2.host_index = 2;
  ret = batch_->InsertOrUpdate(txn2);
  EXPECT_TRUE(ret);

  Transaction txn3;
  txn3.tid = "33333333";
  txn3.nii = 333;
  txn3.invoice_num = 3;
  txn3.transaction_status = TransactionStatus::TO_REVERSE;
  txn3.is_preauth_completed = false;
  txn3.transaction_type = TransactionType::SALE;
  txn3.is_voided = false;
  txn3.host_index = 3;
  ret = batch_->InsertOrUpdate(txn3);
  EXPECT_TRUE(ret);

  Transaction txn4;
  txn4.tid = "44444444";
  txn4.nii = 444;
  txn4.invoice_num = 4;
  txn4.transaction_status = TransactionStatus::APPROVED;
  txn4.is_preauth_completed = true;
  txn4.tx_datetime = stdx::time(NULL) - (22 * 24 * 60 * 60);
  txn4.transaction_type = TransactionType::PREAUTH;
  txn4.is_voided = false;
  txn4.host_index = 2;
  ret = batch_->InsertOrUpdate(txn4);
  EXPECT_TRUE(ret);

  Transaction txn5;
  txn5.tid = "55555555";
  txn5.nii = 555;
  txn5.invoice_num = 5;
  txn5.transaction_status = TransactionStatus::APPROVED;
  txn5.is_preauth_completed = false;
  txn5.transaction_type = TransactionType::PREAUTH;
  txn5.tx_datetime = stdx::time(NULL) - (10 * 24 * 60 * 60);
  txn5.is_voided = false;
  txn5.host_index = 2;
  ret = batch_->InsertOrUpdate(txn5);
  EXPECT_TRUE(ret);

  auto v1 = batch_->GetPendingPreAuth(2);
  EXPECT_EQ(1u, v1.size());
  auto v2 = batch_->GetReversal(3);
  EXPECT_EQ(1u, v2.size());
  auto v3 = batch_->GetPendingAdvices(2);
  EXPECT_EQ(1u, v3.size());
  auto v4 = batch_->GetAllTransactions(2);
  EXPECT_EQ(3u, v4.size());

  ret = batch_->ClearBatch(2);
  EXPECT_TRUE(ret);

  auto v5 = batch_->GetAllTransactions(2);
  EXPECT_EQ(1u, v5.size());
  auto v6 = batch_->GetReversal(3);
  EXPECT_EQ(1u, v6.size());
  auto v7 = batch_->GetPendingAdvices(2);
  EXPECT_EQ(0u, v7.size());
  auto v8 = batch_->GetPendingPreAuth(2);
  EXPECT_EQ(1u, v8.size());
}

TEST_F(BatchTest, TestClearHost1Batch) {
  Transaction txn1;
  txn1.tid = "11111111";
  txn1.nii = 111;
  txn1.invoice_num = 1;
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.is_preauth_completed = true;
  txn1.transaction_type = TransactionType::PREAUTH;
  txn1.is_voided = false;
  txn1.host_index = 2;
  bool ret = batch_->InsertOrUpdate(txn1);
  EXPECT_TRUE(ret);

  Transaction txn2;
  txn2.tid = "22222222";
  txn2.nii = 222;
  txn2.invoice_num = 2;
  txn2.transaction_status = TransactionStatus::TO_ADVISE;
  txn2.is_preauth_completed = false;
  txn2.transaction_type = TransactionType::SALE;
  txn2.is_voided = false;
  txn2.host_index = 1;
  ret = batch_->InsertOrUpdate(txn2);
  EXPECT_TRUE(ret);

  Transaction txn3;
  txn3.tid = "33333333";
  txn3.nii = 333;
  txn3.invoice_num = 3;
  txn3.transaction_status = TransactionStatus::TO_REVERSE;
  txn3.is_preauth_completed = false;
  txn3.transaction_type = TransactionType::SALE;
  txn3.is_voided = false;
  txn3.host_index = 3;
  ret = batch_->InsertOrUpdate(txn3);
  EXPECT_TRUE(ret);

  Transaction txn4;
  txn4.tid = "44444444";
  txn4.nii = 444;
  txn4.invoice_num = 4;
  txn4.transaction_status = TransactionStatus::APPROVED;
  txn4.is_preauth_completed = false;
  txn4.tx_datetime = stdx::time(NULL) - (22 * 24 * 60 * 60);
  txn4.transaction_type = TransactionType::PREAUTH;
  txn4.is_voided = false;
  txn4.host_index = 2;
  ret = batch_->InsertOrUpdate(txn4);
  EXPECT_TRUE(ret);

  Transaction txn5;
  txn5.tid = "55555555";
  txn5.nii = 555;
  txn5.invoice_num = 5;
  txn5.transaction_status = TransactionStatus::APPROVED;
  txn5.is_preauth_completed = false;
  txn5.transaction_type = TransactionType::PREAUTH;
  txn5.tx_datetime = stdx::time(NULL) - (10 * 24 * 60 * 60);
  txn5.is_voided = false;
  txn5.host_index = 1;
  ret = batch_->InsertOrUpdate(txn5);
  EXPECT_TRUE(ret);

  auto v1 = batch_->GetPendingPreAuth(1);
  EXPECT_EQ(1u, v1.size());
  auto v2 = batch_->GetReversal(3);
  EXPECT_EQ(1u, v2.size());
  auto v3 = batch_->GetPendingAdvices(1);
  EXPECT_EQ(1u, v3.size());
  auto v4 = batch_->GetAllTransactions(2);
  EXPECT_EQ(2u, v4.size());

  ret = batch_->ClearBatch(1);
  EXPECT_TRUE(ret);

  auto v5 = batch_->GetAllTransactions(2);
  EXPECT_EQ(1u, v5.size());
  auto v6 = batch_->GetReversal(3);
  EXPECT_EQ(1u, v6.size());
  auto v7 = batch_->GetPendingAdvices(1);
  EXPECT_EQ(0u, v7.size());
  auto v8 = batch_->GetPendingPreAuth(1);
  EXPECT_EQ(1u, v8.size());
}

TEST_F(BatchTest, TestClearHost3Batch) {
  Transaction txn1;
  txn1.tid = "11111111";
  txn1.nii = 111;
  txn1.invoice_num = 1;
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.is_preauth_completed = true;
  txn1.transaction_type = TransactionType::PREAUTH;
  txn1.is_voided = false;
  txn1.host_index = 2;
  bool ret = batch_->InsertOrUpdate(txn1);
  EXPECT_TRUE(ret);

  Transaction txn2;
  txn2.tid = "22222222";
  txn2.nii = 222;
  txn2.invoice_num = 2;
  txn2.transaction_status = TransactionStatus::TO_ADVISE;
  txn2.is_preauth_completed = false;
  txn2.transaction_type = TransactionType::SALE;
  txn2.is_voided = false;
  txn2.host_index = 1;
  ret = batch_->InsertOrUpdate(txn2);
  EXPECT_TRUE(ret);

  Transaction txn3;
  txn3.tid = "33333333";
  txn3.nii = 333;
  txn3.invoice_num = 3;
  txn3.transaction_status = TransactionStatus::TO_REVERSE;
  txn3.is_preauth_completed = false;
  txn3.transaction_type = TransactionType::SALE;
  txn3.is_voided = false;
  txn3.host_index = 3;
  ret = batch_->InsertOrUpdate(txn3);
  EXPECT_TRUE(ret);

  Transaction txn4;
  txn4.tid = "44444444";
  txn4.nii = 444;
  txn4.invoice_num = 4;
  txn4.transaction_status = TransactionStatus::APPROVED;
  txn4.is_preauth_completed = false;
  txn4.tx_datetime = stdx::time(NULL) - (22 * 24 * 60 * 60);
  txn4.transaction_type = TransactionType::PREAUTH;
  txn4.is_voided = false;
  txn4.host_index = 2;
  ret = batch_->InsertOrUpdate(txn4);
  EXPECT_TRUE(ret);

  Transaction txn5;
  txn5.tid = "55555555";
  txn5.nii = 555;
  txn5.invoice_num = 5;
  txn5.transaction_status = TransactionStatus::APPROVED;
  txn5.is_preauth_completed = false;
  txn5.transaction_type = TransactionType::PREAUTH;
  txn5.tx_datetime = stdx::time(NULL) - (10 * 24 * 60 * 60);
  txn5.is_voided = false;
  txn5.host_index = 1;
  ret = batch_->InsertOrUpdate(txn5);
  EXPECT_TRUE(ret);

  auto v1 = batch_->GetPendingPreAuth(3);
  EXPECT_EQ(0u, v1.size());
  auto v2 = batch_->GetReversal(3);
  EXPECT_EQ(1u, v2.size());
  auto v3 = batch_->GetPendingAdvices(1);
  EXPECT_EQ(1u, v3.size());
  auto v4 = batch_->GetAllTransactions(2);
  EXPECT_EQ(2u, v4.size());

  ret = batch_->ClearBatch(3);
  EXPECT_TRUE(ret);

  auto v5 = batch_->GetAllTransactions(2);
  EXPECT_EQ(1u, v5.size());
  auto v6 = batch_->GetReversal(3);
  EXPECT_EQ(0u, v6.size());
  auto v7 = batch_->GetPendingAdvices(1);
  EXPECT_EQ(1u, v7.size());
  auto v8 = batch_->GetPendingPreAuth(3);
  EXPECT_EQ(0u, v8.size());
}

TEST_F(BatchTest, TestClearReversal) {
  bool ret = batch_->IsReversalPending(2);
  EXPECT_FALSE(ret);

  Transaction txn1;
  txn1.tid = "11111111";
  txn1.nii = 111;
  txn1.invoice_num = 1;
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.online_authorization_required = true;
  txn1.transaction_type = TransactionType::PREAUTH;
  ret = batch_->InsertOrUpdate(txn1);
  EXPECT_TRUE(ret);

  Transaction txn2;
  txn2.tid = "22222222";
  txn2.nii = 222;
  txn2.invoice_num = 2;
  txn2.transaction_status = TransactionStatus::CANCELLED;
  txn2.online_authorization_required = false;
  txn2.transaction_type = TransactionType::SALE;
  ret = batch_->InsertOrUpdate(txn2);
  EXPECT_TRUE(ret);

  Transaction txn3;
  txn3.tid = "33333333";
  txn3.nii = 333;
  txn3.invoice_num = 3;
  txn3.transaction_status = TransactionStatus::TO_REVERSE;
  txn3.online_authorization_required = true;
  txn3.transaction_type = TransactionType::SALE;
  txn3.host_index = 2;
  ret = batch_->InsertOrUpdate(txn3);
  EXPECT_TRUE(ret);

  ret = batch_->IsReversalPending(2);
  EXPECT_TRUE(ret);

  ret = batch_->ClearReversal(2);
  EXPECT_TRUE(ret);

  ret = batch_->IsReversalPending(2);
  EXPECT_FALSE(ret);
}

TEST_F(BatchTest, TestDeleteHost2PreAuthTxns) {
  auto preauths1 = batch_->GetPendingPreAuth(2);
  EXPECT_TRUE(preauths1.empty());

  bool ret = batch_->PurgeOldPreAuth(2);
  EXPECT_TRUE(ret);

  Transaction txn1;
  txn1.tid = "11111111";
  txn1.nii = 111;
  txn1.is_voided = false;
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.is_preauth_completed = false;
  txn1.transaction_type = TransactionType::PREAUTH;
  txn1.tx_datetime = stdx::time(NULL) - (22 * 24 * 60 * 60);
  txn1.host_index = 2;
  ret = batch_->InsertOrUpdate(txn1);
  EXPECT_TRUE(ret);

  Transaction txn2;
  txn2.tid = "22222222";
  txn2.nii = 222;
  txn2.is_voided = false;
  txn2.transaction_status = TransactionStatus::APPROVED;
  txn2.is_preauth_completed = false;
  txn2.transaction_type = TransactionType::SALE;
  txn2.tx_datetime = stdx::time(NULL) - (2 * 24 * 60 * 60);
  txn2.host_index = 1;
  ret = batch_->InsertOrUpdate(txn2);
  EXPECT_TRUE(ret);

  Transaction txn3;
  txn3.tid = "33333333";
  txn3.nii = 333;
  txn3.is_voided = true;
  txn3.transaction_status = TransactionStatus::TO_REVERSE;
  txn3.is_preauth_completed = false;
  txn3.transaction_type = TransactionType::SALE;
  txn3.tx_datetime = stdx::time(NULL) - (1 * 24 * 60 * 60);
  txn3.host_index = 2;
  ret = batch_->InsertOrUpdate(txn3);
  EXPECT_TRUE(ret);

  Transaction txn4;
  txn4.tid = "44444444";
  txn4.nii = 444;
  txn4.is_voided = false;
  txn4.transaction_status = TransactionStatus::APPROVED;
  txn4.is_preauth_completed = false;
  txn4.transaction_type = TransactionType::PREAUTH;
  txn4.tx_datetime = stdx::time(NULL) - (10 * 24 * 60 * 60);
  txn4.host_index = 3;
  ret = batch_->InsertOrUpdate(txn4);
  EXPECT_TRUE(ret);

  auto preauths2 = batch_->GetPendingPreAuth(2);
  EXPECT_EQ(1u, preauths2.size());

  ret = batch_->PurgeOldPreAuth(2);
  EXPECT_TRUE(ret);

  auto preauths3 = batch_->GetPendingPreAuth(2);
  EXPECT_EQ(0u, preauths3.size());
}

TEST_F(BatchTest, TestDeleteHost1PreAuthTxns) {
  bool ret = batch_->PurgeOldPreAuth(1);
  auto preauths1 = batch_->GetPendingPreAuth(2);
  EXPECT_TRUE(preauths1.empty());

  ret = batch_->PurgeOldPreAuth(1);
  EXPECT_TRUE(ret);

  Transaction txn1;
  txn1.tid = "11111111";
  txn1.nii = 111;
  txn1.is_voided = false;
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.is_preauth_completed = false;
  txn1.transaction_type = TransactionType::PREAUTH;
  txn1.tx_datetime = stdx::time(NULL) - (22 * 24 * 60 * 60);
  txn1.host_index = 1;
  ret = batch_->InsertOrUpdate(txn1);
  EXPECT_TRUE(ret);

  Transaction txn2;
  txn2.tid = "22222222";
  txn2.nii = 222;
  txn2.is_voided = false;
  txn2.transaction_status = TransactionStatus::APPROVED;
  txn2.is_preauth_completed = false;
  txn2.transaction_type = TransactionType::SALE;
  txn2.tx_datetime = stdx::time(NULL) - (2 * 24 * 60 * 60);
  txn2.host_index = 2;
  ret = batch_->InsertOrUpdate(txn2);
  EXPECT_TRUE(ret);

  Transaction txn3;
  txn3.tid = "33333333";
  txn3.nii = 333;
  txn3.is_voided = true;
  txn3.transaction_status = TransactionStatus::TO_REVERSE;
  txn3.is_preauth_completed = false;
  txn3.transaction_type = TransactionType::SALE;
  txn3.tx_datetime = stdx::time(NULL) - (1 * 24 * 60 * 60);
  txn3.host_index = 1;
  ret = batch_->InsertOrUpdate(txn3);
  EXPECT_TRUE(ret);

  Transaction txn4;
  txn4.tid = "44444444";
  txn4.nii = 444;
  txn4.is_voided = false;
  txn4.transaction_status = TransactionStatus::APPROVED;
  txn4.is_preauth_completed = false;
  txn4.transaction_type = TransactionType::PREAUTH;
  txn4.tx_datetime = stdx::time(NULL) - (10 * 24 * 60 * 60);
  txn4.host_index = 3;
  ret = batch_->InsertOrUpdate(txn4);
  EXPECT_TRUE(ret);

  auto preauths2 = batch_->GetPendingPreAuth(1);
  EXPECT_EQ(1u, preauths2.size());

  ret = batch_->PurgeOldPreAuth(1);
  EXPECT_TRUE(ret);

  auto preauths3 = batch_->GetPendingPreAuth(1);
  EXPECT_EQ(0u, preauths3.size());
}

TEST_F(BatchTest, TestDeleteHost3PreAuthTxns) {
  bool ret;
  Transaction txn1;
  txn1.tid = "11111111";
  txn1.nii = 111;
  txn1.is_voided = false;
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.is_preauth_completed = false;
  txn1.transaction_type = TransactionType::PREAUTH;
  txn1.tx_datetime = stdx::time(NULL) - (22 * 24 * 60 * 60);
  txn1.host_index = 3;
  ret = batch_->InsertOrUpdate(txn1);
  EXPECT_TRUE(ret);

  Transaction txn2;
  txn2.tid = "22222222";
  txn2.nii = 222;
  txn2.is_voided = false;
  txn2.transaction_status = TransactionStatus::APPROVED;
  txn2.is_preauth_completed = false;
  txn2.transaction_type = TransactionType::SALE;
  txn2.tx_datetime = stdx::time(NULL) - (2 * 24 * 60 * 60);
  txn2.host_index = 1;
  ret = batch_->InsertOrUpdate(txn2);
  EXPECT_TRUE(ret);

  Transaction txn3;
  txn3.tid = "33333333";
  txn3.nii = 333;
  txn3.is_voided = true;
  txn3.transaction_status = TransactionStatus::TO_REVERSE;
  txn3.is_preauth_completed = false;
  txn3.transaction_type = TransactionType::SALE;
  txn3.tx_datetime = stdx::time(NULL) - (1 * 24 * 60 * 60);
  txn3.host_index = 3;
  ret = batch_->InsertOrUpdate(txn3);
  EXPECT_TRUE(ret);

  Transaction txn4;
  txn4.tid = "44444444";
  txn4.nii = 444;
  txn4.is_voided = false;
  txn4.transaction_status = TransactionStatus::APPROVED;
  txn4.is_preauth_completed = false;
  txn4.transaction_type = TransactionType::PREAUTH;
  txn4.tx_datetime = stdx::time(NULL) - (10 * 24 * 60 * 60);
  txn4.host_index = 3;
  ret = batch_->InsertOrUpdate(txn4);
  EXPECT_TRUE(ret);

  auto preauths2 = batch_->GetPendingPreAuth(3);
  EXPECT_EQ(2u, preauths2.size());

  ret = batch_->PurgeOldPreAuth(3);
  EXPECT_TRUE(ret);

  auto preauths3 = batch_->GetPendingPreAuth(3);
  EXPECT_EQ(1u, preauths3.size());
}

TEST_F(BatchTest, TestInsertAndFindSecondaryAmount) {
  Transaction txn1;
  txn1.invoice_num = 1;
  txn1.host_index = 1;
  txn1.amount = types::Amount("INR", 1000);
  bool ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  Transaction txn2;
  txn2.invoice_num = 2;
  txn2.host_index = 1;
  txn2.amount = types::Amount("INR", 1000);
  txn2.secondary_amount = types::Amount("INR", 500);
  ret = batch_->Insert(txn2);
  EXPECT_TRUE(ret);

  Transaction txn3;
  txn3.invoice_num = 3;
  txn3.host_index = 1;
  txn3.amount = types::Amount("INR", 1000);
  ret = batch_->Insert(txn3);
  EXPECT_TRUE(ret);

  auto v1 = batch_->FindTransactionByInvoice(1);
  ASSERT_EQ(1u, v1.size());
  EXPECT_EQ(txn1.invoice_num, v1[0].invoice_num);
  EXPECT_EQ(1000u, v1[0].amount->GetValue());
  EXPECT_FALSE(v1[0].secondary_amount);

  auto v2 = batch_->FindTransactionByInvoice(2);
  ASSERT_EQ(1u, v2.size());
  EXPECT_EQ(txn2.invoice_num, v2[0].invoice_num);
  EXPECT_EQ(1000u, v2[0].amount->GetValue());
  EXPECT_EQ(500u, v2[0].secondary_amount->GetValue());

  auto v3 = batch_->FindTransactionByInvoice(3);
  ASSERT_EQ(1u, v3.size());
  EXPECT_EQ(txn3.invoice_num, v3[0].invoice_num);
  EXPECT_EQ(1000u, v3[0].amount->GetValue());
  EXPECT_FALSE(v3[0].secondary_amount);
}

TEST_F(BatchTest, TestGetPendingTcUpload) {
  Transaction txn1;
  txn1.tid = "11111111";
  txn1.nii = 111;
  txn1.invoice_num = 1;
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.tc_upload_status = TcUploadStatus::TC_UPLOAD_COMPLETED;
  txn1.online_authorization_required = true;
  txn1.transaction_type = TransactionType::PREAUTH;
  txn1.host_index = 1;
  bool ret = batch_->InsertOrUpdate(txn1);
  EXPECT_TRUE(ret);

  Transaction txn2;
  txn2.tid = "22222222";
  txn2.nii = 222;
  txn2.invoice_num = 2;
  txn2.transaction_status = TransactionStatus::APPROVED;
  txn2.tc_upload_status = TcUploadStatus::TC_UPLOAD_NEEDED;
  txn2.online_authorization_required = false;
  txn2.transaction_type = TransactionType::SALE;
  txn2.host_index = 1;
  ret = batch_->InsertOrUpdate(txn2);
  EXPECT_TRUE(ret);

  Transaction txn3;
  txn3.tid = "33333333";
  txn3.nii = 333;
  txn3.invoice_num = 3;
  txn3.transaction_status = TransactionStatus::APPROVED;
  txn3.tc_upload_status = TcUploadStatus::TC_UPLOAD_NOT_NEEDED;
  txn3.online_authorization_required = true;
  txn3.transaction_type = TransactionType::SALE;
  txn3.host_index = 1;
  ret = batch_->InsertOrUpdate(txn3);
  EXPECT_TRUE(ret);

  auto v = batch_->GetPendingTcUpload(1);
  ASSERT_EQ(1u, v.size());
  EXPECT_EQ("22222222", v[0].tid);
  EXPECT_EQ(222u, v[0].nii);
  EXPECT_EQ(2u, v[0].invoice_num);
  EXPECT_EQ(TransactionStatus::APPROVED, v[0].transaction_status);
  EXPECT_FALSE(v[0].online_authorization_required);
  EXPECT_EQ(SALE, v[0].transaction_type);
  EXPECT_EQ(TcUploadStatus::TC_UPLOAD_NEEDED, v[0].tc_upload_status);
}

TEST_F(BatchTest, TestInsertAndFindByStan) {
  Transaction txn1;

  txn1.tid = "12345678";
  txn1.nii = 111;
  txn1.invoice_num = 5;
  txn1.host_index = 1;
  txn1.stan = 123;
  txn1.secondary_amount = types::Amount("INR", 1000);
  bool ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  auto v = batch_->FindTransactionByStan(12);
  EXPECT_TRUE(v.empty());

  auto v1 = batch_->FindTransactionByStan(123);
  ASSERT_EQ(1u, v1.size());
  EXPECT_EQ(txn1.tid, v1[0].tid);
  EXPECT_EQ(txn1.nii, v1[0].nii);
  EXPECT_EQ(txn1.invoice_num, v1[0].invoice_num);
  EXPECT_EQ(txn1.stan, v1[0].stan);
  EXPECT_EQ(txn1.host_index, v1[0].host_index);
  EXPECT_EQ(1000u, v1[0].secondary_amount->GetValue());
  EXPECT_FALSE(bool(txn1.pan_sequence_number));
}

TEST_F(BatchTest, TestInsertAndFindByPan) {
  Transaction txn1;

  const std::vector<uint8_t> pan_v = { 0x12, 0x34, 0x56, 0x78, 0x90, 0x12, 0x34,
      0x56 };
  types::Pan pan(pan_v, true);

  const std::vector<uint8_t> pan_v2 = { 0x12, 0x34, 0x56, 0x78, 0x90, 0x11,
      0x34, 0x56 };
  types::Pan pan2(pan_v2, true);

  txn1.tid = "12345678";
  txn1.nii = 111;
  txn1.invoice_num = 5;
  txn1.host_index = 1;
  txn1.stan = 123;
  txn1.pan = pan;
  txn1.secondary_amount = types::Amount("INR", 1000);
  bool ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  auto v = batch_->FindTransactionByPan(pan2);
  EXPECT_TRUE(v.empty());

  auto v1 = batch_->FindTransactionByPan(pan);
  ASSERT_EQ(1u, v1.size());
  EXPECT_EQ(txn1.tid, v1[0].tid);
  EXPECT_EQ(txn1.nii, v1[0].nii);
  EXPECT_EQ(txn1.invoice_num, v1[0].invoice_num);
  EXPECT_EQ(txn1.stan, v1[0].stan);
  EXPECT_EQ(*txn1.pan, *v1[0].pan);
  EXPECT_EQ(txn1.host_index, v1[0].host_index);
  EXPECT_EQ(1000u, v1[0].secondary_amount->GetValue());
  EXPECT_FALSE(bool(txn1.pan_sequence_number));
}

TEST_F(BatchTest, TestInsertAndFindByAmount) {
  Transaction txn1;

  const std::vector<uint8_t> pan_v = { 0x12, 0x34, 0x56, 0x78, 0x90, 0x12, 0x34,
      0x56 };
  types::Pan pan(pan_v, true);

  const std::vector<uint8_t> pan_v2 = { 0x12, 0x34, 0x56, 0x78, 0x90, 0x11,
      0x34, 0x56 };
  types::Pan pan2(pan_v2, true);

  txn1.tid = "12345678";
  txn1.nii = 111;
  txn1.invoice_num = 5;
  txn1.host_index = 1;
  txn1.stan = 123;
  txn1.pan = pan;
  txn1.amount = types::Amount("INR", 1000);
  bool ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  auto v = batch_->FindTransactionByAmount(types::Amount("INR", 2000));
  EXPECT_TRUE(v.empty());

  auto v1 = batch_->FindTransactionByAmount(types::Amount("INR", 1000));
  ASSERT_EQ(1u, v1.size());
  EXPECT_EQ(txn1.tid, v1[0].tid);
  EXPECT_EQ(txn1.nii, v1[0].nii);
  EXPECT_EQ(txn1.invoice_num, v1[0].invoice_num);
  EXPECT_EQ(txn1.stan, v1[0].stan);
  EXPECT_EQ(*txn1.pan, *v1[0].pan);
  EXPECT_EQ(txn1.host_index, v1[0].host_index);
  EXPECT_EQ(1000u, v1[0].amount->GetValue());
  EXPECT_FALSE(bool(txn1.pan_sequence_number));

  txn1.tid = "12345678";
  txn1.nii = 111;
  txn1.invoice_num = 6;
  txn1.host_index = 1;
  txn1.stan = 124;
  txn1.pan = pan;
  txn1.amount = types::Amount("INR", 1000);
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  txn1.tid = "12345678";
  txn1.nii = 111;
  txn1.invoice_num = 7;
  txn1.host_index = 1;
  txn1.stan = 125;
  txn1.pan = pan;
  txn1.amount = types::Amount("INR", 2000);
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  auto v2 = batch_->FindTransactionByAmount(types::Amount("INR", 1000));
  ASSERT_EQ(2u, v2.size());
  EXPECT_EQ(txn1.tid, v2[0].tid);
  EXPECT_EQ(txn1.nii, v2[0].nii);
  EXPECT_EQ(5u, v2[0].invoice_num);
  EXPECT_EQ(123u, v2[0].stan);
  EXPECT_EQ(*txn1.pan, *v2[0].pan);
  EXPECT_EQ(txn1.host_index, v2[0].host_index);
  EXPECT_EQ(1000u, v2[0].amount->GetValue());

  EXPECT_EQ(txn1.tid, v2[1].tid);
  EXPECT_EQ(txn1.nii, v2[1].nii);
  EXPECT_EQ(6u, v2[1].invoice_num);
  EXPECT_EQ(124u, v2[1].stan);
  EXPECT_EQ(*txn1.pan, *v2[1].pan);
  EXPECT_EQ(txn1.host_index, v2[1].host_index);
  EXPECT_EQ(1000u, v2[1].amount->GetValue());
}

TEST_F(BatchTest, GetBatchTotalsForHost) {

  const std::vector<uint8_t> pan_v = { 0x12, 0x34, 0x56, 0x78, 0x90, 0x12, 0x34,
      0x56 };
  types::Pan pan(pan_v, true);

  Transaction txn1;
  txn1.tid = "12345678";
  txn1.nii = 111;
  txn1.invoice_num = 5;
  txn1.host_index = 1;
  txn1.stan = 123;
  txn1.pan = pan;
  txn1.amount = types::Amount("SGD", 1000);
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.transaction_type = TransactionType::SALE;
  txn1.host_index = 1;
  bool ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  BatchTotalsForHost totals = batch_->GetBatchTotalsForHost(1);

  EXPECT_EQ(1u, totals.sales_total.count);
  EXPECT_EQ(1000u, totals.sales_total.total);
  EXPECT_EQ(0u, totals.refunds_total.count);
  EXPECT_EQ(0u, totals.refunds_total.total);

  txn1.invoice_num = 6;
  txn1.stan = 124;
  txn1.amount = types::Amount("SGD", 2000);
  txn1.transaction_status = TransactionStatus::TO_ADVISE;
  txn1.transaction_type = TransactionType::SALE;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  totals = batch_->GetBatchTotalsForHost(1);
  EXPECT_EQ(2u, totals.sales_total.count);
  EXPECT_EQ(3000u, totals.sales_total.total);
  EXPECT_EQ(0u, totals.refunds_total.count);
  EXPECT_EQ(0u, totals.refunds_total.total);

  txn1.invoice_num = 6;
  txn1.stan = 125;
  txn1.amount = types::Amount("SGD", 3000);
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.transaction_type = TransactionType::OFFLINE_SALE;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  totals = batch_->GetBatchTotalsForHost(1);
  EXPECT_EQ(3u, totals.sales_total.count);
  EXPECT_EQ(6000u, totals.sales_total.total);
  EXPECT_EQ(0u, totals.refunds_total.count);
  EXPECT_EQ(0u, totals.refunds_total.total);

  txn1.invoice_num = 7;
  txn1.stan = 126;
  txn1.amount = types::Amount("SGD", 4000);
  txn1.transaction_status = TransactionStatus::TO_ADVISE;
  txn1.transaction_type = TransactionType::OFFLINE_SALE;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  totals = batch_->GetBatchTotalsForHost(1);
  EXPECT_EQ(4u, totals.sales_total.count);
  EXPECT_EQ(10000u, totals.sales_total.total);
  EXPECT_EQ(0u, totals.refunds_total.count);
  EXPECT_EQ(0u, totals.refunds_total.total);

  txn1.invoice_num = 8;
  txn1.stan = 127;
  txn1.amount = types::Amount("SGD", 5000);
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.transaction_type = TransactionType::SALE;
  txn1.is_voided = true;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  totals = batch_->GetBatchTotalsForHost(1);
  EXPECT_EQ(4u, totals.sales_total.count);
  EXPECT_EQ(10000u, totals.sales_total.total);
  EXPECT_EQ(0u, totals.refunds_total.count);
  EXPECT_EQ(0u, totals.refunds_total.total);

  txn1.invoice_num = 10;
  txn1.stan = 129;
  txn1.amount = types::Amount("SGD", 1100);
  txn1.preauth_amount = types::Amount("SGD", 1150);
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.transaction_type = TransactionType::PREAUTH;
  txn1.is_voided = false;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  totals = batch_->GetBatchTotalsForHost(1);
  EXPECT_EQ(4u, totals.sales_total.count);
  EXPECT_EQ(10000u, totals.sales_total.total);
  EXPECT_EQ(0u, totals.refunds_total.count);
  EXPECT_EQ(0u, totals.refunds_total.total);

  txn1.invoice_num = 11;
  txn1.stan = 130;
  txn1.amount = types::Amount("SGD", 1200);
  txn1.preauth_amount = types::Amount("SGD", 1250);
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.transaction_type = TransactionType::PREAUTH;
  txn1.is_voided = true;
  txn1.is_adjusted = false;
  txn1.is_preauth_completed = true;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  totals = batch_->GetBatchTotalsForHost(1);
  EXPECT_EQ(4u, totals.sales_total.count);
  EXPECT_EQ(10000u, totals.sales_total.total);
  EXPECT_EQ(0u, totals.refunds_total.count);
  EXPECT_EQ(0u, totals.refunds_total.total);

  txn1.invoice_num = 12;
  txn1.stan = 131;
  txn1.amount = types::Amount("SGD", 1310);
  txn1.preauth_amount = types::Amount("SGD", 1300);
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.transaction_type = TransactionType::PREAUTH;
  txn1.is_voided = false;
  txn1.is_adjusted = false;
  txn1.is_preauth_completed = false;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  totals = batch_->GetBatchTotalsForHost(1);
  EXPECT_EQ(4u, totals.sales_total.count);
  EXPECT_EQ(10000u, totals.sales_total.total);
  EXPECT_EQ(0u, totals.refunds_total.count);
  EXPECT_EQ(0u, totals.refunds_total.total);

  txn1.invoice_num = 13;
  txn1.stan = 132;
  txn1.amount = types::Amount("SGD", 1410);
  txn1.preauth_amount = types::Amount("SGD", 1400);
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.transaction_type = TransactionType::PREAUTH_COMPLETION_ONLINE;
  txn1.is_voided = false;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  totals = batch_->GetBatchTotalsForHost(1);
  EXPECT_EQ(5u, totals.sales_total.count);
  EXPECT_EQ(11410u, totals.sales_total.total);
  EXPECT_EQ(0u, totals.refunds_total.count);
  EXPECT_EQ(0u, totals.refunds_total.total);

  txn1.invoice_num = 14;
  txn1.stan = 133;
  txn1.amount = types::Amount("SGD", 1510);
  txn1.preauth_amount = types::Amount("SGD", 1500);
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.transaction_type = TransactionType::PREAUTH_COMPLETION_ONLINE;
  txn1.is_voided = true;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  totals = batch_->GetBatchTotalsForHost(1);
  EXPECT_EQ(5u, totals.sales_total.count);
  EXPECT_EQ(11410u, totals.sales_total.total);
  EXPECT_EQ(0u, totals.refunds_total.count);
  EXPECT_EQ(0u, totals.refunds_total.total);

  txn1.invoice_num = 15;
  txn1.stan = 134;
  txn1.amount = types::Amount("SGD", 1610);
  txn1.preauth_amount = types::Amount("SGD", 1600);
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.transaction_type = TransactionType::PREAUTH_COMPLETION_OFFLINE;
  txn1.is_voided = false;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  totals = batch_->GetBatchTotalsForHost(1);
  EXPECT_EQ(6u, totals.sales_total.count);
  EXPECT_EQ(13020u, totals.sales_total.total);
  EXPECT_EQ(0u, totals.refunds_total.count);
  EXPECT_EQ(0u, totals.refunds_total.total);

  txn1.invoice_num = 16;
  txn1.stan = 135;
  txn1.amount = types::Amount("SGD", 1610);
  txn1.preauth_amount = types::Amount("SGD", 1700);
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.transaction_type = TransactionType::PREAUTH_COMPLETION_OFFLINE;
  txn1.is_voided = true;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  totals = batch_->GetBatchTotalsForHost(1);
  EXPECT_EQ(6u, totals.sales_total.count);
  EXPECT_EQ(13020u, totals.sales_total.total);
  EXPECT_EQ(0u, totals.refunds_total.count);
  EXPECT_EQ(0u, totals.refunds_total.total);

  txn1.invoice_num = 17;
  txn1.stan = 136;
  txn1.amount = types::Amount("SGD", 1000);
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.transaction_type = TransactionType::REFUND;
  txn1.is_voided = false;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  totals = batch_->GetBatchTotalsForHost(1);
  EXPECT_EQ(6u, totals.sales_total.count);
  EXPECT_EQ(13020u, totals.sales_total.total);
  EXPECT_EQ(1u, totals.refunds_total.count);
  EXPECT_EQ(1000u, totals.refunds_total.total);

  txn1.invoice_num = 18;
  txn1.stan = 137;
  txn1.amount = types::Amount("SGD", 1100);
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.transaction_type = TransactionType::REFUND;
  txn1.is_voided = true;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  totals = batch_->GetBatchTotalsForHost(1);
  EXPECT_EQ(6u, totals.sales_total.count);
  EXPECT_EQ(13020u, totals.sales_total.total);
  EXPECT_EQ(1u, totals.refunds_total.count);
  EXPECT_EQ(1000u, totals.refunds_total.total);

  //to be counted
  txn1.invoice_num = 9;
  txn1.stan = 128;
  txn1.amount = types::Amount("SGD", 6000);
  txn1.secondary_amount = types::Amount("SGD", 10);
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.transaction_type = TransactionType::SALE;
  txn1.is_voided = false;
  txn1.is_adjusted = true;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  totals = batch_->GetBatchTotalsForHost(1);
  EXPECT_EQ(7u, totals.sales_total.count);
  EXPECT_EQ(19030u, totals.sales_total.total);
  EXPECT_EQ(1u, totals.refunds_total.count);
  EXPECT_EQ(1000u, totals.refunds_total.total);
}

TEST_F(BatchTest, GetBatchTotalsForReport) {

  const std::vector<uint8_t> pan_v = { 0x12, 0x34, 0x56, 0x78, 0x90, 0x12, 0x34,
      0x56 };
  types::Pan pan(pan_v, true);

  //to be counted in sale
  Transaction txn1;
  txn1.tid = "12345678";
  txn1.nii = 111;
  txn1.invoice_num = 5;
  txn1.host_index = 1;
  txn1.stan = 123;
  txn1.pan = pan;
  txn1.amount = types::Amount("SGD", 1000);
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.transaction_type = TransactionType::SALE;
  txn1.online_authorization_required = true;
  txn1.host_index = 1;
  bool ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  //to be counted in sale
  txn1.invoice_num = 6;
  txn1.stan = 124;
  txn1.amount = types::Amount("SGD", 2000);
  txn1.transaction_status = TransactionStatus::TO_ADVISE;
  txn1.transaction_type = TransactionType::SALE;
  txn1.online_authorization_required = true;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  //to be counted  in offline
  txn1.invoice_num = 6;
  txn1.stan = 125;
  txn1.amount = types::Amount("SGD", 3000);
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.transaction_type = TransactionType::OFFLINE_SALE;
  txn1.online_authorization_required = false;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  //to be counted in offline
  txn1.invoice_num = 7;
  txn1.stan = 126;
  txn1.amount = types::Amount("SGD", 4000);
  txn1.transaction_status = TransactionStatus::TO_ADVISE;
  txn1.transaction_type = TransactionType::OFFLINE_SALE;
  txn1.online_authorization_required = false;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  //to be counted in void sale
  txn1.invoice_num = 8;
  txn1.stan = 127;
  txn1.amount = types::Amount("SGD", 5000);
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.transaction_type = TransactionType::SALE;
  txn1.online_authorization_required = true;
  txn1.is_voided = true;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  //to be counted in sale
  txn1.invoice_num = 10;
  txn1.stan = 129;
  txn1.amount = types::Amount("SGD", 1100);
  txn1.preauth_amount = types::Amount("SGD", 1150);
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.transaction_type = TransactionType::PREAUTH_COMPLETION_ONLINE;
  txn1.is_voided = false;
  txn1.is_adjusted = false;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  //to be counted in void sale
  txn1.invoice_num = 11;
  txn1.stan = 130;
  txn1.amount = types::Amount("SGD", 1200);
  txn1.preauth_amount = types::Amount("SGD", 1250);
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.transaction_type = TransactionType::PREAUTH_COMPLETION_ONLINE;
  txn1.is_voided = true;
  txn1.is_adjusted = false;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  //not counted
  txn1.invoice_num = 12;
  txn1.stan = 131;
  txn1.preauth_amount = types::Amount("SGD", 1300);
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.transaction_type = TransactionType::PREAUTH_CANCELLATION;
  txn1.is_voided = true;
  txn1.is_adjusted = false;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  //not counted
  txn1.invoice_num = 13;
  txn1.stan = 132;
  txn1.preauth_amount = types::Amount("SGD", 1400);
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.transaction_type = TransactionType::PREAUTH;
  txn1.is_voided = false;
  txn1.is_adjusted = false;
  txn1.is_preauth_completed = false;
  txn1.online_authorization_required = true;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  //to be counted in refund
  txn1.invoice_num = 14;
  txn1.stan = 133;
  txn1.amount = types::Amount("SGD", 1500);
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.transaction_type = TransactionType::REFUND;
  txn1.is_voided = false;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  //to be counted in void refund
  txn1.invoice_num = 15;
  txn1.stan = 134;
  txn1.amount = types::Amount("SGD", 1600);
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.transaction_type = TransactionType::REFUND;
  txn1.is_voided = true;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  //to be counted in offline sale
  txn1.invoice_num = 16;
  txn1.stan = 135;
  txn1.amount = types::Amount("SGD", 1700);
  txn1.preauth_amount = types::Amount("SGD", 1150);
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.transaction_type = TransactionType::PREAUTH_COMPLETION_OFFLINE;
  txn1.is_voided = false;
  txn1.is_adjusted = false;
  txn1.is_preauth_completed = true;
  txn1.online_authorization_required = false;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  //to be counted in sale and tip
  txn1.invoice_num = 9;
  txn1.stan = 128;
  txn1.amount = types::Amount("SGD", 6000);
  txn1.secondary_amount = types::Amount("SGD", 10);
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.transaction_type = TransactionType::SALE;
  txn1.online_authorization_required = true;
  txn1.is_voided = false;
  txn1.is_adjusted = true;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  //to be counted in sale and tip
  txn1.invoice_num = 17;
  txn1.stan = 136;
  txn1.amount = types::Amount("SGD", 6000);
  txn1.secondary_amount = types::Amount("SGD", 50);
  txn1.transaction_status = TransactionStatus::TO_ADVISE;
  txn1.transaction_type = TransactionType::SALE;
  txn1.online_authorization_required = true;
  txn1.is_voided = false;
  txn1.is_adjusted = true;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  BatchTotalsReport totals = batch_->GetBatchTotalsForReport(1);

  EXPECT_EQ(5u, totals.global_batch_totals.sales_total.count);
  EXPECT_EQ(16100u, totals.global_batch_totals.sales_total.total);

  EXPECT_EQ(2u, totals.global_batch_totals.tips_total.count);
  EXPECT_EQ(60u, totals.global_batch_totals.tips_total.total);

  EXPECT_EQ(1u, totals.global_batch_totals.refunds_total.count);
  EXPECT_EQ(1500u, totals.global_batch_totals.refunds_total.total);

  EXPECT_EQ(3u, totals.global_batch_totals.offline_sales_total.count);
  EXPECT_EQ(8700u, totals.global_batch_totals.offline_sales_total.total);

  EXPECT_EQ(2u, totals.global_batch_totals.void_sales_total.count);
  EXPECT_EQ(6200u, totals.global_batch_totals.void_sales_total.total);

  EXPECT_EQ(1u, totals.global_batch_totals.void_refunds_total.count);
  EXPECT_EQ(1600u, totals.global_batch_totals.void_refunds_total.total);

  EXPECT_EQ(10u, totals.global_batch_totals.total.count);
  EXPECT_EQ(24860u, totals.global_batch_totals.total.total);

  EXPECT_EQ("ABC", totals.batch_totals_list[0].card_type);
}

TEST_F(BatchTest, GetBatchUploadTransactions) {
  Transaction txn1;

  const std::vector<uint8_t> pan_v = { 0x12, 0x34, 0x56, 0x78, 0x90, 0x12, 0x34,
      0x56 };
  types::Pan pan(pan_v, true);

  txn1.tid = "12345678";
  txn1.nii = 111;
  txn1.invoice_num = 5;
  txn1.host_index = 1;
  txn1.stan = 123;
  txn1.pan = pan;
  txn1.amount = types::Amount("SGD", 1000);
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.transaction_type = TransactionType::SALE;
  txn1.host_index = 1;
  txn1.issuer_index = 1;
  bool ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  txn1.invoice_num = 6;
  txn1.stan = 123;
  txn1.transaction_type = TransactionType::OFFLINE_SALE;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  txn1.invoice_num = 7;
  txn1.stan = 123;
  txn1.transaction_type = TransactionType::CASH_ADVANCE;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  txn1.invoice_num = 8;
  txn1.stan = 123;
  txn1.transaction_type = TransactionType::REFUND;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  txn1.invoice_num = 9;
  txn1.stan = 123;
  txn1.transaction_type = TransactionType::PREAUTH_COMPLETION_ONLINE;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  txn1.invoice_num = 10;
  txn1.stan = 123;
  txn1.transaction_type = TransactionType::PREAUTH_COMPLETION_OFFLINE;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  txn1.invoice_num = 11;
  txn1.stan = 123;
  txn1.transaction_type = TransactionType::INSTALMENT_SALE;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  txn1.invoice_num = 12;
  txn1.stan = 123;
  txn1.transaction_type = TransactionType::PREAUTH;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  txn1.invoice_num = 13;
  txn1.stan = 123;
  txn1.transaction_type = TransactionType::SALE;
  txn1.is_voided = true;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  txn1.invoice_num = 14;
  txn1.stan = 125;
  txn1.transaction_type = TransactionType::SALE;
  txn1.is_voided = false;
  txn1.host_index = 2;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  Transactions tx_list = batch_->GetBatchUploadTransactions(1);

  EXPECT_EQ(7u, tx_list.size());
}

TEST_F(BatchTest, GetBatchDetailTransactions) {
  Transaction txn1;

  const std::vector<uint8_t> pan_v = { 0x12, 0x34, 0x56, 0x78, 0x90, 0x12, 0x34,
      0x56 };
  types::Pan pan(pan_v, true);

  txn1.tid = "12345678";
  txn1.nii = 111;
  txn1.invoice_num = 5;
  txn1.stan = 123;
  txn1.pan = pan;
  txn1.amount = types::Amount("SGD", 1000);
  txn1.transaction_status = TransactionStatus::APPROVED;
  txn1.transaction_type = TransactionType::SALE;
  txn1.host_index = 1;
  txn1.issuer_index = 1;
  bool ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  txn1.invoice_num = 6;
  txn1.stan = 123;
  txn1.transaction_type = TransactionType::OFFLINE_SALE;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  txn1.invoice_num = 7;
  txn1.stan = 123;
  txn1.transaction_type = TransactionType::CASH_ADVANCE;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  txn1.invoice_num = 8;
  txn1.stan = 123;
  txn1.transaction_type = TransactionType::REFUND;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  txn1.invoice_num = 9;
  txn1.stan = 123;
  txn1.transaction_type = TransactionType::PREAUTH_COMPLETION_ONLINE;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  txn1.invoice_num = 10;
  txn1.stan = 123;
  txn1.transaction_type = TransactionType::PREAUTH_COMPLETION_OFFLINE;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  txn1.invoice_num = 11;
  txn1.stan = 123;
  txn1.transaction_type = TransactionType::INSTALMENT_SALE;
  txn1.host_index = 1;
  txn1.issuer_index = 2;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  txn1.invoice_num = 13;
  txn1.stan = 123;
  txn1.transaction_type = TransactionType::SALE;
  txn1.is_voided = true;
  txn1.host_index = 2;
  txn1.issuer_index = 1;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  txn1.invoice_num = 13;
  txn1.stan = 123;
  txn1.secondary_amount = types::Amount("SGD", 100);
  txn1.transaction_type = TransactionType::SALE;
  txn1.is_voided = false;
  txn1.is_adjusted = true;
  txn1.host_index = 2;
  txn1.issuer_index = 1;
  ret = batch_->Insert(txn1);
  EXPECT_TRUE(ret);

  DetailReportSingleHost detail_report1 = batch_->GetBatchDetailTransactions(1);

  EXPECT_EQ(7u, detail_report1.transaction_count);

  DetailReportSingleHost detail_report2 = batch_->GetBatchDetailTransactions(2);

  EXPECT_EQ(2u, detail_report2.transaction_count);

  DetailReportSingleHost detail_report3 = batch_->GetBatchDetailTransactions(3);

  EXPECT_EQ(0u, detail_report3.transaction_count);
}
