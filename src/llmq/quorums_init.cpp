// Copyright (c) 2023 The Hellar Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "quorums_init.h"

#include "quorums_blockprocessor.h"
#include "quorums_commitment.h"
#include "quorums_dummydkg.h"

namespace llmq
{

void InitLLMQSystem(CProDB& proDb)
{
    quorumBlockProcessor = new CQuorumBlockProcessor(proDb);
    quorumDummyDKG = new CDummyDKG();
}

void DestroyLLMQSystem()
{
    delete quorumDummyDKG;
    quorumDummyDKG = nullptr;
    delete quorumBlockProcessor;
    quorumBlockProcessor = nullptr;
}

}
