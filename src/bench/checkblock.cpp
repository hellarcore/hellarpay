// Copyright (c) 2023-2024 The Hellar Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "bench.h"

#include "chainparams.h"
#include "validation.h"
#include "streams.h"
#include "consensus/validation.h"

#include "bench/data/block657390.raw.h"

// These are the two major time-sinks which happen after we have fully received
// a block off the wire, but before we can relay the block on to peers using
// compact block relay.

static void DeserializeBlockTest(benchmark::State& state)
{
    CDataStream stream((const char*)raw_bench::block657390,
            (const char*)&raw_bench::block657390[sizeof(raw_bench::block657390)],
            SER_NETWORK, PROTOCOL_VERSION);
    char a;
    stream.write(&a, 1); // Prevent compaction

    while (state.KeepRunning()) {
        CBlock block;
        stream >> block;
        assert(stream.Rewind(sizeof(raw_bench::block657390)));
    }
}

static void DeserializeAndCheckBlockTest(benchmark::State& state)
{
    CDataStream stream((const char*)raw_bench::block657390,
            (const char*)&raw_bench::block657390[sizeof(raw_bench::block657390)],
            SER_NETWORK, PROTOCOL_VERSION);
    char a;
    stream.write(&a, 1); // Prevent compaction

    Consensus::Params params = Params(CBaseChainParams::MAIN).GetConsensus();

    while (state.KeepRunning()) {
        CBlock block; // Note that CBlock caches its checked state, so we need to recreate it here
        stream >> block;
        assert(stream.Rewind(sizeof(raw_bench::block657390)));

        CValidationState validationState;
        assert(CheckBlock(block, validationState, params, block.GetBlockTime()));
    }
}

BENCHMARK(DeserializeBlockTest);
BENCHMARK(DeserializeAndCheckBlockTest);
