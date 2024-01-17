// Copyright (c) 2023 The Hellar Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "prodb.h"

CProDB* proDb;

CProDB::CProDB(size_t nCacheSize, bool fMemory, bool fWipe) :
    db(fMemory ? "" : (GetDataDir() / "prodb"), nCacheSize, fMemory, fWipe),
    rootBatch(db),
    rootDBTransaction(db, rootBatch),
    curDBTransaction(rootDBTransaction, rootDBTransaction)
{
}

bool CProDB::CommitRootTransaction()
{
    assert(curDBTransaction.IsClean());
    rootDBTransaction.Commit();
    bool ret = db.WriteBatch(rootBatch);
    rootBatch.Clear();
    return ret;
}

bool CProDB::VerifyBestBlock(const uint256& hash)
{
    // Make sure prodb is consistent.
    // If we already have best block hash saved, the previous block should match it.
    uint256 hashBestBlock;
    bool fHasBestBlock = Read(PRODB_BEST_BLOCK, hashBestBlock);
    uint256 hashBlockIndex = fHasBestBlock ? hash : uint256();
    assert(hashBestBlock == hashBlockIndex);

    return fHasBestBlock;
}

void CProDB::WriteBestBlock(const uint256& hash)
{
    Write(PRODB_BEST_BLOCK, hash);
}
