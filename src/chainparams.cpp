// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Copyright (c) 2014-2017 The Dash Core developers
// Copyright (c) 2021-2023 The Hellar Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "consensus/merkle.h"

#include "tinyformat.h"
#include "util.h"
#include "utilstrencodings.h"

#include "arith_uint256.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

#include "chainparamsseeds.h"

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

static CBlock CreateDevNetGenesisBlock(const uint256 &prevBlockHash, const std::string& devNetName, uint32_t nTime, uint32_t nNonce, uint32_t nBits, const CAmount& genesisReward)
{
    assert(!devNetName.empty());

    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    // put height (BIP34) and devnet name into coinbase
    txNew.vin[0].scriptSig = CScript() << 1 << std::vector<unsigned char>(devNetName.begin(), devNetName.end());
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = CScript() << OP_RETURN;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = 4;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock = prevBlockHash;
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "Hellar World Wide Cryptocurrency";
    const CScript genesisOutputScript = CScript() << ParseHex("044a3dcaf13ec4705e4dbc50a6e681926337b8bf625c1ac726942139ef8508649121cc01df8aa905eb69267ae63691bf07d2b7738834f277f7f4bed91a46d53b1f") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

static CBlock FindDevNetGenesisBlock(const Consensus::Params& params, const CBlock &prevBlock, const CAmount& reward)
{
    std::string devNetName = GetDevNetName();
    assert(!devNetName.empty());

    CBlock block = CreateDevNetGenesisBlock(prevBlock.GetHash(), devNetName.c_str(), prevBlock.nTime + 1, 0, prevBlock.nBits, reward);

    arith_uint256 bnTarget;
    bnTarget.SetCompact(block.nBits);

    for (uint32_t nNonce = 0; nNonce < UINT32_MAX; nNonce++) {
        block.nNonce = nNonce;

        uint256 hash = block.GetHash();
        if (UintToArith256(hash) <= bnTarget)
            return block;
    }

    // This is very unlikely to happen as we start the devnet with a very low difficulty. In many cases even the first
    // iteration of the above loop will give a result already
    error("FindDevNetGenesisBlock: could not find devnet genesis block for %s", devNetName);
    assert(false);
}

// this one is for testing only
static Consensus::LLMQParams llmq5_60 = {
        .type = Consensus::LLMQ_5_60,
        .name = "llmq_5_60",
        .size = 5,
        .minSize = 3,
        .threshold = 3,

        .dkgInterval = 24, // one DKG per hour
        .dkgPhaseBlocks = 2,
        .dkgMiningWindowStart = 10, // dkgPhaseBlocks * 5 = after finalization
        .dkgMiningWindowEnd = 18,
};

static Consensus::LLMQParams llmq40_60 = {
        .type = Consensus::LLMQ_40_60,
        .name = "llmq_40_60",
        .size = 40,
        .minSize = 32,
        .threshold = 24,

        .dkgInterval = 24, // one DKG per hour
        .dkgPhaseBlocks = 2,
        .dkgMiningWindowStart = 10, // dkgPhaseBlocks * 5 = after finalization
        .dkgMiningWindowEnd = 18,
};

static Consensus::LLMQParams llmq200_60 = {
        .type = Consensus::LLMQ_200_60,
        .name = "llmq_200_60",
        .size = 200,
        .minSize = 150,
        .threshold = 120,

        .dkgInterval = 24 * 12, // one DKG every 12 hours
        .dkgPhaseBlocks = 4,
        .dkgMiningWindowStart = 20, // dkgPhaseBlocks * 5 = after finalization
        .dkgMiningWindowEnd = 28,
};

// Used for deployment and min-proto-version signalling, so it needs a higher threshold
static Consensus::LLMQParams llmq200_85 = {
        .type = Consensus::LLMQ_200_85,
        .name = "llmq_200_85",
        .size = 200,
        .minSize = 180,
        .threshold = 170,

        .dkgInterval = 24 * 24, // one DKG every 24 hours
        .dkgPhaseBlocks = 4,
        .dkgMiningWindowStart = 20, // dkgPhaseBlocks * 5 = after finalization
        .dkgMiningWindowEnd = 48, // give it a larger mining window to make sure it is mined
};


/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */


class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";
        consensus.nSubsidyHalvingInterval = 210240; // Note: actual number of blocks per calendar year with DGW v3 is ~200700 (for example 449750 - 249050)
        consensus.nMasternodePaymentsStartBlock = 57075; // not true, but it's ok as long as it's less then nMasternodePaymentsIncreaseBlock
        consensus.nMasternodePaymentsIncreaseBlock = 158000; // actual historical value
        consensus.nMasternodePaymentsIncreasePeriod = 576*30; // 17280 - actual historical value
        consensus.nInstantSendConfirmationsRequired = 6;
        consensus.nInstantSendKeepLock = 24;
        consensus.nBudgetPaymentsStartBlock = 328008; // actual historical value
        consensus.nBudgetPaymentsCycleBlocks = 16616; // ~(60*24*30)/2.6, actual number of blocks per month is 200700 / 12 = 16725
        consensus.nBudgetPaymentsWindowBlocks = 100;
        consensus.nSuperblockStartBlock = 398784; // The block at which 12.1 goes live (end of final 12.0 budget cycle)
        consensus.nSuperblockStartHash = uint256S("000000000094d7db77ba41d4209a35501d464b30d8f4d8f5dec6d1b7a6e611da");
        consensus.nSuperblockCycle = 16616; // ~(60*24*30)/2.6, actual number of blocks per month is 200700 / 12 = 16725
        consensus.nGovernanceMinQuorum = 10;
        consensus.nGovernanceFilterElements = 20000;
        consensus.nMasternodeMinimumConfirmations = 15;
        consensus.BIP34Height = 0;
        consensus.BIP34Hash = uint256S("0x0000025b0952dab1f52e080d07e7be699504fae8afa56933fd402ff0551173d9");
        consensus.BIP65Height = 397152; // 00000000007f533c388f14f335132643d4cb53996959faa7e648341d8500c553
        consensus.BIP66Height = 951; // 0000025b0952dab1f52e080d07e7be699504fae8afa56933fd402ff0551173d9
        consensus.DIP0001Height = 717696; // Activate it on 27th November ///need to change to actual value
        consensus.RemoveDependencyRewardOnDiffHeight = 780000; // approx. on 4-5th of April
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 20
        consensus.nPowTargetTimespan = 24 * 60 * 60; // Helcon: 1 day
        consensus.nPowTargetSpacing = 2.5 * 60; // Helcon: 2.5 minutes
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nPowKGWHeight = 15200;
        consensus.nPowDGWHeight = 34140;
        consensus.nRuleChangeActivationThreshold = 1836; // 95% of 2024
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1748059200; // May 24th, 2025
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1779595200; // May 24th, 2026

        // Deployment of DIP0001
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nStartTime = 1773115200; // Mar 10th, 2026
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nTimeout = 1804651200; // Mar 10th, 2027
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nWindowSize = 4032;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nThreshold = 3226; // 80% of 4032

        // Deployment of BIP147
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nStartTime = 1773115200; // Mar 10th, 2026
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nTimeout = 1804651200; // Mar 10th, 2027
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nWindowSize = 4032;
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nThreshold = 3226; // 80% of 4032

        // Deployment of DIP0003
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0003].bit = 3;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0003].nStartTime = 1841457600; // May 9th, 2028
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0003].nTimeout = 1872993600; // May 9th, 2029
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0003].nWindowSize = 4032;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0003].nThreshold = 3226; // 80% of 4032

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");// Start

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00"); // 1025000

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xbf;
        pchMessageStart[1] = 0x0c;
        pchMessageStart[2] = 0x6b;
        pchMessageStart[3] = 0xbd;
        vAlertPubKey = ParseHex("041a87909b3e842c4b0fe800597974203698f4ab11af181c77d0395f2dbb9e0c2ae43121c45fa5cfc1898bc359f143d47c874fad122362cb036a905482d4404903");
        nDefaultPort = 7778;
        nPruneAfterHeight = 100000;

        genesis = CreateGenesisBlock(1692543600, 297485, 0x1e0ffff0, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x000007ad6b573a7045fff41a2abec2106cf4cbaef9917c3fc7523e3b2bd15b14"));
        assert(genesis.hashMerkleRoot == uint256S("0x8316be17c9135b8a13d53df7dace1bc504e8c1c7dbf35b072efc24206b1bbdcc"));

        vSeeds.push_back(CDNSSeedData("hellar.io", "dnsseed.hellar.io"));
        vSeeds.push_back(CDNSSeedData("masternode.pw", "dnsseed.masternode.pw"));

        // Main network P2PKH address starts with 'H'
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,40);
        // Main network P2SH addresses starts with 'h'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,64);
        // Hellar private keys start with '5'
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,168);
        // Hellar BIP32 pubkeys start with 'xpub' (Bitcoin defaults)
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x89)(0x0B)(0xBC).convert_to_container<std::vector<unsigned char> >();
        // Hellar BIP32 prvkeys start with 'xprv' (Bitcoin defaults)
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0x9D)(0xB2).convert_to_container<std::vector<unsigned char> >();

        // Hellar BIP44 coin type is '251'
        nExtCoinType = 251;

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        // long living quorum params
        consensus.llmqs[Consensus::LLMQ_40_60] = llmq40_60;
        consensus.llmqs[Consensus::LLMQ_200_60] = llmq200_60;
        consensus.llmqs[Consensus::LLMQ_200_85] = llmq200_85;

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fRequireRoutableExternalIP = true;
        fMineBlocksOnDemand = false;
        fAllowMultipleAddressesFromGroup = false;
        fAllowMultiplePorts = false;

        nPoolMaxTransactions = 3;
        nFulfilledRequestExpireTime = 60*60; // fulfilled requests expire in 1 hour

        vSporkAddresses = {"HDSdiA8GT8oe37y4QKNkYyZD62FAgSqY5V"};
        nMinSporkKeys = 1;
        fBIP9CheckMasternodesUpgraded = true;
        consensus.fLLMQAllowDummyCommitments = false;

        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            (  0, uint256S("0x000007ad6b573a7045fff41a2abec2106cf4cbaef9917c3fc7523e3b2bd15b14"))
            (  2000, uint256S("0x00000482a09ea57969835af201a45b7f07fffd046d58446b38317fb5701da37b"))
            (  3000, uint256S("0x00000c24c0a5c6bdee4ce02cf75c568625f8b5c126a74a2ca24fdd5180ed7a60"))
            (  5000, uint256S("0x00000e210955311e6c628a7158a509960e44e993e870faeef122806dacf3b38f"))
            (  7500, uint256S("0x0000033c09d39045185f3628b72357b00b5690f7aabe0aafdf035032efc4c292"))
            (  10000, uint256S("0x00000313fcccbda4464397e629e5f2f063476896453887bd8198b1e90d993f1e"))
            (  12500, uint256S("0x00000f4f7a509164481aa14bf6c093f8c7f5cf29486bfb0358013eca454b7445"))
            (  15000, uint256S("0x00000327e612d6cb12106b295c2d6f17c9fc763d4d2d2444b777496b767d1e67"))
            (  17500, uint256S("0x00000fdc1d38d18b824b6c24b2750749ca0dad670eef6ec6389dc6ac6ed50eb5"))
            (  19000, uint256S("0x00000a9c6c1116067765c44f2aea4cf14cd636a69e0193ae774f09439735ead2"))
            (  21500, uint256S("0x00000eda9bdaa34db601d95459a76c999c910b5a9dfa7caca61bf34df60edae6")) 
            (  22000, uint256S("0x000009b11e9de3263b1bb7195f48e92428587167c676dcdbb669f3389128f583"))
        };

        chainTxData = ChainTxData{
            1692543600, // * UNIX timestamp of last known number of transactions
            0,    // * total number of transactions between genesis and that timestamp
                        //   (the tx=... number in the SetBestChain debug.log lines)
            0.0        // * estimated number of transactions per second after that timestamp
        };
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        consensus.nSubsidyHalvingInterval = 210240;
        consensus.nMasternodePaymentsStartBlock = 10000; // not true, but it's ok as long as it's less then nMasternodePaymentsIncreaseBlock
        consensus.nMasternodePaymentsIncreaseBlock = 46000;
        consensus.nMasternodePaymentsIncreasePeriod = 10;
        consensus.nInstantSendConfirmationsRequired = 2;
        consensus.nInstantSendKeepLock = 6;
        consensus.nBudgetPaymentsStartBlock = 57500;
        consensus.nBudgetPaymentsCycleBlocks = 50;
        consensus.nBudgetPaymentsWindowBlocks = 10;
        consensus.nSuperblockStartBlock = 60000; // NOTE: Should satisfy nSuperblockStartBlock > nBudgetPeymentsStartBlock
        consensus.nSuperblockStartHash = uint256(); // do not check this on testnet
        consensus.nSuperblockCycle = 24; // Superblocks can be issued hourly on testnet
        consensus.nGovernanceMinQuorum = 1;
        consensus.nGovernanceFilterElements = 500;
        consensus.nMasternodeMinimumConfirmations = 1;
        consensus.BIP34Height = 0;
        consensus.BIP34Hash = uint256S("0x00000243719923dcc244bbb0e81bd5b03e3ce4fd3fe408358503ae5856ce3b40");
        consensus.BIP65Height = 66528; // 0000009d43b7778b3057c232afd3c7ce424a17adf8375b8b3ecc0578a4e0f272
        consensus.BIP66Height = 76; // 00000243719923dcc244bbb0e81bd5b03e3ce4fd3fe408358503ae5856ce3b40
        consensus.DIP0001Height = 340100; 
        consensus.RemoveDependencyRewardOnDiffHeight = 405151;       
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 20
        consensus.nPowTargetTimespan = 24 * 60 * 60; // Helcon: 1 day
        consensus.nPowTargetSpacing = 2.5 * 60; // Helcon: 2.5 minutes
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.nPowKGWHeight = 2000; // nPowKGWHeight >= nPowDGWHeight means "no KGW"
        consensus.nPowDGWHeight = 2000;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1748059200; // May 24th, 2025
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1779595200; // May 24th, 2026

        // Deployment of DIP0001
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nStartTime = 1773115200; // Mar 10th, 2026
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nTimeout = 1804651200; // Mar 10th, 2027
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nWindowSize = 4032;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nThreshold = 50; // 50% of 100

        // Deployment of BIP147
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nStartTime = 1773115200; // Mar 10th, 2026
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nTimeout = 1804651200; // Mar 10th, 2027
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nWindowSize = 100;
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nThreshold = 50; // 50% of 100

        // Deployment of DIP0003
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0003].bit = 3;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0003].nStartTime = 1841457600; // May 9th, 2028
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0003].nTimeout = 1872993600; // May 9th, 2029
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0003].nWindowSize = 100;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0003].nThreshold = 50; // 50% of 100

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");// Start
        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00"); // 665000

        pchMessageStart[0] = 0xcd;
        pchMessageStart[1] = 0xc2;
        pchMessageStart[2] = 0xcb;
        pchMessageStart[3] = 0xfc;
        vAlertPubKey = ParseHex("04b1018c140f82d22181ae6e06561841b791a56bec9a38e78ca55e46561e237ff33e55ebdae6c65c006962c4c02aa057f22d6ce690df33ffb84d866593edc9fc12");
        nDefaultPort = 27787;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1692554400UL, 108474UL, 0x1e0ffff0, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x000006923ac6b36bb68a5df7944e1717be197c03fc739046bc242a5e849b4cf8"));
        assert(genesis.hashMerkleRoot == uint256S("0x8316be17c9135b8a13d53df7dace1bc504e8c1c7dbf35b072efc24206b1bbdcc"));

        vFixedSeeds.clear();
        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        vSeeds.push_back(CDNSSeedData("masternode.pw", "test-dnsseed.masternode.pw"));

        // Testnet Hellar addresses starts with 'm'
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,140);
        // Testnet Hellar script addresses starts with '2'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,248);
        // Testnet private keys start with '9' or 'c' (Bitcoin defaults)
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        // Testnet Hellar BIP32 pubkeys start with 'tpub' (Bitcoin defaults)
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        // Testnet Hellar BIP32 prvkeys start with 'tprv' (Bitcoin defaults)
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        // Testnet Hellar BIP44 coin type is '1' (All coin's testnet default)
        nExtCoinType = 1;

        // long living quorum params
        consensus.llmqs[Consensus::LLMQ_40_60] = llmq40_60;
        consensus.llmqs[Consensus::LLMQ_200_60] = llmq200_60;
        consensus.llmqs[Consensus::LLMQ_200_85] = llmq200_85;

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fRequireRoutableExternalIP = true;
        fMineBlocksOnDemand = false;
        fAllowMultipleAddressesFromGroup = false;
        fAllowMultiplePorts = false;     

        nPoolMaxTransactions = 3;
        nFulfilledRequestExpireTime = 5*60; // fulfilled requests expire in 5 minutes

        vSporkAddresses = {"yTExB1w5TB9FpTuirGgd3Wmt2U1Vq53XAJ"};
        nMinSporkKeys = 1;
        fBIP9CheckMasternodesUpgraded = true;
        consensus.fLLMQAllowDummyCommitments = true;

        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            ( 0, uint256S("0x000006923ac6b36bb68a5df7944e1717be197c03fc739046bc242a5e849b4cf8"))
            (  2000, uint256S("0x000004404197f4ebf9fdc0eb660276af235e0875b079ddf599f3ae5327656c38"))
            (  2500, uint256S("0x000005dc280b28db0ceea9f9ed4b08a811e05e309974f3901281a903d878d5f7"))
            (  4000, uint256S("0x000006881548ad92d362bc7bfbd0322d0efaa8b85dede22b28cdaa2f41c0fa48"))
            (  6000, uint256S("0x00000edbcc994cceb93e87e783cdae8a28c35739ecac425bf854e14757a86040"))
            (  7000, uint256S("0x0000054f12e9898bafa54b4efcc58b52d7787d1f5806cc18f07e580db73b9e54"))
            };

        chainTxData = ChainTxData{        
            1692554400, // * UNIX timestamp of last known number of transactions
            0,    // * total number of transactions between genesis and that timestamp
                        //   (the tx=... number in the SetBestChain debug.log lines)
            0.00        // * estimated number of transactions per second after that timestamp
        };

    }
};
static CTestNetParams testNetParams;

/**
 * Devnet
 */
class CDevNetParams : public CChainParams {
public:
    CDevNetParams() {
        strNetworkID = "dev";
        consensus.nSubsidyHalvingInterval = 210240;
        consensus.nMasternodePaymentsStartBlock = 4010; // not true, but it's ok as long as it's less then nMasternodePaymentsIncreaseBlock
        consensus.nMasternodePaymentsIncreaseBlock = 4030;
        consensus.nMasternodePaymentsIncreasePeriod = 10;
        consensus.nInstantSendConfirmationsRequired = 2;
        consensus.nInstantSendKeepLock = 6;
        consensus.nBudgetPaymentsStartBlock = 4100;
        consensus.nBudgetPaymentsCycleBlocks = 50;
        consensus.nBudgetPaymentsWindowBlocks = 10;
        consensus.nSuperblockStartBlock = 4200; // NOTE: Should satisfy nSuperblockStartBlock > nBudgetPeymentsStartBlock
        consensus.nSuperblockStartHash = uint256(); // do not check this on devnet
        consensus.nSuperblockCycle = 24; // Superblocks can be issued hourly on devnet
        consensus.nGovernanceMinQuorum = 1;
        consensus.nGovernanceFilterElements = 500;
        consensus.nMasternodeMinimumConfirmations = 1;
        consensus.BIP34Height = 1; // BIP34 activated immediately on devnet
        consensus.BIP65Height = 1; // BIP65 activated immediately on devnet
        consensus.BIP66Height = 1; // BIP66 activated immediately on devnet
        consensus.DIP0001Height = 2; // DIP0001 activated immediately on devnet
        consensus.RemoveDependencyRewardOnDiffHeight = 10; // activated immediately on devnet       
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 1
        consensus.nPowTargetTimespan = 24 * 60 * 60; // Helcon: 1 day
        consensus.nPowTargetSpacing = 2.5 * 60; // Helcon: 2.5 minutes
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.nPowKGWHeight = 4001; // nPowKGWHeight >= nPowDGWHeight means "no KGW"
        consensus.nPowDGWHeight = 4001;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1748059200; // May 24th, 2025
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1779595200; // May 24th, 2026

        // Deployment of DIP0001
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nStartTime = 1773115200; // Mar 10th, 2026
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nTimeout = 1804651200; // Mar 10th, 2027
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nWindowSize = 4032;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nThreshold = 50; // 50% of 100

        // Deployment of BIP147
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nStartTime = 1773115200; // Mar 10th, 2026
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nTimeout = 1804651200; // Mar 10th, 2027
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nWindowSize = 100;
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nThreshold = 50; // 50% of 100

        // Deployment of DIP0003
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0003].bit = 3;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0003].nStartTime = 1841457600; // May 9th, 2028
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0003].nTimeout = 1872993600; // May 9th, 2029
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0003].nWindowSize = 100;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0003].nThreshold = 50; // 50% of 100

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");

        pchMessageStart[0] = 0xe3;
        pchMessageStart[1] = 0xcb;
        pchMessageStart[2] = 0xfd;
        pchMessageStart[3] = 0xcd;
        vAlertPubKey = ParseHex("04ac7f9628615259dc4cad8b5254dfa89cc9cbb17f2ceb987b808c124e9933fed2c0e3a6d5419d7f0bd10dfd1c7a3ea12467c27716706d12c4fdfac3bf7a12e1de");
        nDefaultPort = 27782;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1691863200, 0, 0x207fffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x64b91779eb7b765390682df128aeba887f9aef8ea16b58d85e529017f80d66cd"));
        assert(genesis.hashMerkleRoot == uint256S("0x8316be17c9135b8a13d53df7dace1bc504e8c1c7dbf35b072efc24206b1bbdcc"));

        devnetGenesis = FindDevNetGenesisBlock(consensus, genesis, 50 * COIN);
        consensus.hashDevnetGenesisBlock = devnetGenesis.GetHash();

        vFixedSeeds.clear();
        vSeeds.clear();
        //vSeeds.push_back(CDNSSeedData("hellardot.pw",  "devnet-seed.hellardot.pw"));

        // Testnet Helcon addresses starts with 'd'
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,83);
        // Testnet Helcon script addresses starts with '7' 
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,135);
        // Testnet private keys start with '9' or 'c' (Bitcoin defaults)
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,212);
        // Testnet Helcon BIP32 pubkeys start with 'tpub' (Bitcoin defaults)
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        // Testnet Helcon BIP32 prvkeys start with 'tprv' (Bitcoin defaults)
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        // Testnet Helcon BIP44 coin type is '1' (All coin's testnet default)
        nExtCoinType = 1;

        // long living quorum params
        consensus.llmqs[Consensus::LLMQ_40_60] = llmq40_60;
        consensus.llmqs[Consensus::LLMQ_200_60] = llmq200_60;
        consensus.llmqs[Consensus::LLMQ_200_85] = llmq200_85;

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fAllowMultipleAddressesFromGroup = true;
        fAllowMultiplePorts = true;   

        nPoolMaxTransactions = 3;
        nFulfilledRequestExpireTime = 5*60; // fulfilled requests expire in 5 minutes

        vSporkAddresses = {"aWya3pweytjKCkxnTMhTQNG39iKjGpzJ1H"};
        nMinSporkKeys = 1;
        // devnets are started with no blocks and no MN, so we can't check for upgraded MN (as there are none)
        fBIP9CheckMasternodesUpgraded = false;
        consensus.fLLMQAllowDummyCommitments = true;

        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            (      0, uint256S("0x64b91779eb7b765390682df128aeba887f9aef8ea16b58d85e529017f80d66cd"))
            (      1, devnetGenesis.GetHash())
        };

        chainTxData = ChainTxData{
            devnetGenesis.GetBlockTime(), // * UNIX timestamp of devnet genesis block
            2,                            // * we only have 2 coinbase transactions when a devnet is started up
            0.0                          // * estimated number of transactions per second
        };
    }

    void UpdateSubsidyAndDiffParams(int nMinimumDifficultyBlocks, int nHighSubsidyBlocks, int nHighSubsidyFactor)
    {
        consensus.nMinimumDifficultyBlocks = nMinimumDifficultyBlocks;
        consensus.nHighSubsidyBlocks = nHighSubsidyBlocks;
        consensus.nHighSubsidyFactor = nHighSubsidyFactor;
    }
};
static CDevNetParams *devNetParams;


/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.nSubsidyHalvingInterval = 150;
        consensus.nMasternodePaymentsStartBlock = 240;
        consensus.nMasternodePaymentsIncreaseBlock = 350;
        consensus.nMasternodePaymentsIncreasePeriod = 10;
        consensus.nInstantSendConfirmationsRequired = 2;
        consensus.nInstantSendKeepLock = 6;
        consensus.nBudgetPaymentsStartBlock = 1000;
        consensus.nBudgetPaymentsCycleBlocks = 50;
        consensus.nBudgetPaymentsWindowBlocks = 10;
        consensus.nSuperblockStartBlock = 1500;
        consensus.nSuperblockStartHash = uint256(); // do not check this on regtest
        consensus.nSuperblockCycle = 10;
        consensus.nGovernanceMinQuorum = 1;
        consensus.nGovernanceFilterElements = 100;
        consensus.nMasternodeMinimumConfirmations = 1;
        consensus.BIP34Height = 100000000; // BIP34 has not activated on regtest (far in the future so block v1 are not rejected in tests)
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 1351; // BIP65 activated on regtest (Used in rpc activation tests)
        consensus.BIP66Height = 1251; // BIP66 activated on regtest (Used in rpc activation tests)
        consensus.DIP0001Height = 2000;
        consensus.RemoveDependencyRewardOnDiffHeight = 2100;        
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 1
        consensus.nPowTargetTimespan = 24 * 60 * 60; // Helcon: 1 day
        consensus.nPowTargetSpacing = 2.5 * 60; // Helcon: 2.5 minutes
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nPowKGWHeight = 15200; // same as mainnet
        consensus.nPowDGWHeight = 34140; // same as mainnet
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nTimeout = 999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nTimeout = 999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0003].bit = 3;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0003].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0003].nTimeout = 999999999999ULL;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");

        pchMessageStart[0] = 0xcc;
        pchMessageStart[1] = 0xc3;
        pchMessageStart[2] = 0xb5;
        pchMessageStart[3] = 0xde;
        nDefaultPort = 28333;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1692554401, 0, 0x207fffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x29297c2b09a3cd748c26d96c7783adbd425116ff68963c886a9a42ce7c34e2f1"));
        assert(genesis.hashMerkleRoot == uint256S("0x8316be17c9135b8a13d53df7dace1bc504e8c1c7dbf35b072efc24206b1bbdcc"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fRequireRoutableExternalIP = false;
        fMineBlocksOnDemand = true;
        fAllowMultipleAddressesFromGroup = true;
        fAllowMultiplePorts = true; 

        nFulfilledRequestExpireTime = 5*60; // fulfilled requests expire in 5 minutes

        vSporkAddresses = {"mnSTcsHirwhr6tsD972Nyst4mqY99SjpUS"};
        nMinSporkKeys = 1;
        // regtest usually has no masternodes in most tests, so don't check for upgraged MNs
        fBIP9CheckMasternodesUpgraded = false;
        consensus.fLLMQAllowDummyCommitments = true;

        checkpointData = (CCheckpointData){
            boost::assign::map_list_of
            ( 0, uint256S("0x29297c2b09a3cd748c26d96c7783adbd425116ff68963c886a9a42ce7c34e2f1")),
        };

        chainTxData = ChainTxData{
            0,
            0,
            0
        };

        // Regtest Hellar addresses starts with 'r'
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        // Regtest Hellar script addresses starts with '3'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        // Regtest private keys start with '9' or 'c' (Bitcoin defaults)
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        // Regtest Hellar BIP32 pubkeys start with 'tpub' (Bitcoin defaults)
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        // Regtest Hellar BIP32 prvkeys start with 'tprv' (Bitcoin defaults)
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        // Regtest Hellar BIP44 coin type is '1' (All coin's testnet default)
        nExtCoinType = 1;

        // long living quorum params
        consensus.llmqs[Consensus::LLMQ_5_60] = llmq5_60;
        consensus.llmqs[Consensus::LLMQ_40_60] = llmq40_60;
    }

    void UpdateBIP9Parameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
    {
        consensus.vDeployments[d].nStartTime = nStartTime;
        consensus.vDeployments[d].nTimeout = nTimeout;
    }

    void UpdateBudgetParameters(int nMasternodePaymentsStartBlock, int nBudgetPaymentsStartBlock, int nSuperblockStartBlock)
    {
        consensus.nMasternodePaymentsStartBlock = nMasternodePaymentsStartBlock;
        consensus.nBudgetPaymentsStartBlock = nBudgetPaymentsStartBlock;
        consensus.nSuperblockStartBlock = nSuperblockStartBlock;
    }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = 0;

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
            return mainParams;
    else if (chain == CBaseChainParams::TESTNET)
            return testNetParams;
    else if (chain == CBaseChainParams::DEVNET) {
            assert(devNetParams);
            return *devNetParams;
    } else if (chain == CBaseChainParams::REGTEST)
            return regTestParams;
    else
        throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    if (network == CBaseChainParams::DEVNET) {
        devNetParams = (CDevNetParams*)new uint8_t[sizeof(CDevNetParams)];
        memset(devNetParams, 0, sizeof(CDevNetParams));
        new (devNetParams) CDevNetParams();
    }

    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

void UpdateRegtestBIP9Parameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
{
    regTestParams.UpdateBIP9Parameters(d, nStartTime, nTimeout);
}

void UpdateRegtestBudgetParameters(int nMasternodePaymentsStartBlock, int nBudgetPaymentsStartBlock, int nSuperblockStartBlock)
{
    regTestParams.UpdateBudgetParameters(nMasternodePaymentsStartBlock, nBudgetPaymentsStartBlock, nSuperblockStartBlock);
}

void UpdateDevnetSubsidyAndDiffParams(int nMinimumDifficultyBlocks, int nHighSubsidyBlocks, int nHighSubsidyFactor)
{
    assert(devNetParams);
    devNetParams->UpdateSubsidyAndDiffParams(nMinimumDifficultyBlocks, nHighSubsidyBlocks, nHighSubsidyFactor);
}
