// Copyright (c) 2023 The Hellar Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef HEL_PRODB_H
#define HEL_PRODB_H

#include "dbwrapper.h"
#include "sync.h"
#include "uint256.h"

static const std::string PRODB_BEST_BLOCK = "b_b";

class CProDB
{
private:
    CCriticalSection cs;
    CDBWrapper db;

    typedef CDBTransaction<CDBWrapper, CDBBatch> RootTransaction;
    typedef CDBTransaction<RootTransaction, RootTransaction> CurTransaction;
    typedef CScopedDBTransaction<RootTransaction, RootTransaction> ScopedTransaction;

    CDBBatch rootBatch;
    RootTransaction rootDBTransaction;
    CurTransaction curDBTransaction;

public:
    CProDB(size_t nCacheSize, bool fMemory = false, bool fWipe = false);

    std::unique_ptr<ScopedTransaction> BeginTransaction()
    {
        LOCK(cs);
        auto t = ScopedTransaction::Begin(curDBTransaction);
        return t;
    }

    template <typename K, typename V>
    bool Read(const K& key, V& value)
    {
        LOCK(cs);
        return curDBTransaction.Read(key, value);
    }

    template <typename K, typename V>
    void Write(const K& key, const V& value)
    {
        LOCK(cs);
        curDBTransaction.Write(key, value);
    }

    template <typename K>
    bool Exists(const K& key)
    {
        LOCK(cs);
        return curDBTransaction.Exists(key);
    }

    template <typename K>
    void Erase(const K& key)
    {
        LOCK(cs);
        curDBTransaction.Erase(key);
    }

    CDBWrapper& GetRawDB()
    {
        return db;
    }

    bool CommitRootTransaction();

    bool VerifyBestBlock(const uint256& hash);
    void WriteBestBlock(const uint256& hash);
};

extern CProDB* proDb;

#endif //HEL_PRODB_H
