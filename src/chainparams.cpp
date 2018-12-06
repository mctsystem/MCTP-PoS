// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Copyright (c) 2014-2017 The MCT Core developers
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

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=00000ffd590b14, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=e0028e, nTime=1390095618, nBits=1e0ffff0, nNonce=28917698, vtx=1)
 *   CTransaction(hash=e0028e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d01044c5957697265642030392f4a616e2f3230313420546865204772616e64204578706572696d656e7420476f6573204c6976653a204f76657273746f636b2e636f6d204973204e6f7720416363657074696e6720426974636f696e73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0xA9037BAC7050C479B121CF)
 *   vMerkleTree: e0028e
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
        const char* pszTimestamp = "27/05/2018 New Trading Platform MCT being launched next week";
        const CScript genesisOutputScript = CScript() << ParseHex("04178eb2d72ac4245e281189d947d319ff67b268e3d0b2d72931fe3327236d98347044ac68e739de5443b7df8ddfd362353090ded4f7fa2395d37b630bd6119cbc") << OP_CHECKSIG;
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
            consensus.nSubsidyHalvingInterval = 210000; 
            consensus.nMasternodePaymentsStartBlock = 1440; 
            consensus.nMasternodePaymentsIncreaseBlock = 150; 
            consensus.nMasternodePaymentsIncreasePeriod = 720*7; 
            consensus.nInstantSendKeepLock = 24;
            consensus.nBudgetPaymentsStartBlock = 328008; // actual historical value
            consensus.nBudgetPaymentsCycleBlocks = 43200; // ~60*24*30
            consensus.nBudgetPaymentsWindowBlocks = 100;
            //consensus.nBudgetProposalEstablishingTime = 60*60*24;
            consensus.nSuperblockStartBlock = 296600; // one year and 1 month from now
            consensus.nSuperblockCycle = 43200; // ~60*24*30
            consensus.nGovernanceMinQuorum = 10;
            consensus.nGovernanceFilterElements = 20000;
            consensus.nMasternodeMinimumConfirmations = 15;
            consensus.nMajorityEnforceBlockUpgrade = 750;
            consensus.nMajorityRejectBlockOutdated = 950;
            consensus.nMajorityWindow = 1000;
            consensus.BIP34Height = 1;
            consensus.BIP34Hash = uint256S("0x000008beee6fa1e2212145f7ccfa10d4f1dfe265e8e1f4b21dd4c196c639cff8");
            consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); //diff adjustments
            consensus.nPowTargetTimespan = 24 * 60 * 60; // MCT: 1 day
            consensus.nPowTargetSpacing = 1 * 60; // MCT: 60 seconds
            consensus.fPowAllowMinDifficultyBlocks = false;//Mindiff blocks are needed, but apparently they break consensus
            consensus.fPowNoRetargeting = false;
            consensus.nRuleChangeActivationThreshold = 1916; // 95% of 2016
            consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
            consensus.nLastPoWBlock = 269000;
            // Stake information
            consensus.nPosTargetSpacing = 1 * 60; // PoSW: 1 minute
            consensus.nPosTargetTimespan = 60 * 40;
            consensus.nStakeMinAge = 60 * 60;
            consensus.nStakeMaxAge = 60 * 60 * 24; // one day
            consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
            consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
            consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

            // Deployment of BIP68, BIP112, and BIP113.
            consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
            consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1527453712; // 23/Mar/2018
            consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1530144000; // 28/Jun/2018

            // Deployment of DIP0001
            consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].bit = 1;
            consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nStartTime = 1527453712; // 23/Feb/2018
            consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nTimeout = 1539561600; // 15/Oct/2018
            consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nWindowSize = 4032;
            consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nThreshold = 3226; // 80% of 4032
            consensus.nMinimumChainWork = uint256S("0x000000000000000000000000000000000000000000000000"); // 0
            consensus.defaultAssumeValid = uint256S("0x000000000000000000000000000000000000000000000000"); // 0
            pchMessageStart[0] = 0xfd;
            pchMessageStart[1] = 0xe9;
            pchMessageStart[2] = 0x9c;
            pchMessageStart[3] = 0xb1;
            vAlertPubKey = ParseHex("04fdd84c016631fd15b83201356693291cd1d3aa3fa2cb43523920fd0419e289349ab685ef81eeba333a4a87f01e15f134f336d688b6db4f842ca38a04bd8d536a");
            nDefaultPort = 9551;
            //nMaxTipAge = 6 * 60 * 60; // ~144 blocks behind -> 2 x fork detection time, was 24 * 60 * 60 in bitcoin
            //nDelayGetHeadersTime = 24 * 60 * 60;
            nPruneAfterHeight = 999999999;

            genesis = CreateGenesisBlock(1527453712, 4953922, 0x1e0ffff0, 1, 50 * COIN);
            consensus.hashGenesisBlock = genesis.GetHash();
        
            assert(consensus.hashGenesisBlock == uint256S("0x000003470aebda8e4c312b725f3d712d73d29bd66870237a592e234528ede2a8"));
            assert(genesis.hashMerkleRoot == uint256S("0xc807b5315065804037ea051fbe34b77b5ee9ba4e499524c2c37fe0bc37b8ff34"));

            vSeeds.push_back(CDNSSeedData("mct.plus", "seeds.mct.plus"));
            
            // MCT addresses start with                                     'M'
            base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,50);
            // MCT script addresses start with                              'C'
            base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,28);
            // MCT private keys start with                                  'T'
            base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,65);
            // MCT BIP32 pubkeys start with 'xpub' (Bitcoin defaults)
            base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
            // MCT BIP32 prvkeys start with 'xprv' (Bitcoin defaults)
            base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();

            // MCT BIP44 coin type is '5'
            nExtCoinType = 5;

            vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

            fMiningRequiresPeers = true;
            fDefaultConsistencyChecks = false;
            fRequireStandard = true;
            fMineBlocksOnDemand = true;
            //fTestnetToBeDeprecatedFieldRPC = false;

            nPoolMaxTransactions = 3;
            nFulfilledRequestExpireTime = 60*60; // fulfilled requests expire in 1 hour
            strSporkAddress = "PAvya6xSBRb755Uhe2aXBFsjuyi68hTP8u";

            checkpointData = (CCheckpointData) {
                boost::assign::map_list_of
                ( 0, uint256S("0x000003470aebda8e4c312b725f3d712d73d29bd66870237a592e234528ede2a8")),
            
            };
        chainTxData = ChainTxData{
            1527926979, // * UNIX timestamp of last checkpoint block
            256128,    // * total number of transactions between genesis and last checkpoint
                    //   (the tx=... number in the SetBestChain debug.log lines)
            21000        // * estimated number of transactions per day after checkpoint

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
            consensus.nSubsidyHalvingInterval = 1569325056;
            consensus.nMasternodePaymentsStartBlock = 15; // not true, but it's ok as long as it's less then nMasternodePaymentsIncreaseBlock
            consensus.nMasternodePaymentsIncreaseBlock = 1569325056;
            consensus.nMasternodePaymentsIncreasePeriod = 1569325056;
            consensus.nInstantSendConfirmationsRequired = 2;
            consensus.nInstantSendKeepLock = 6;
            consensus.nBudgetPaymentsStartBlock = 46;
            consensus.nBudgetPaymentsCycleBlocks = 24;
            consensus.nBudgetPaymentsWindowBlocks = 10;
            consensus.nSuperblockStartBlock = 3050; // NOTE: Should satisfy nSuperblockStartBlock > nBudgetPaymentsStartBlock
            // consensus.nSuperblockStartHash = uint256S("000001af046f4ed575a48b919ed28be8a40c6a78df8d7830fbbfd07ec17a1fee");
            consensus.nSuperblockCycle = 24; // Superblocks can be issued hourly on testnet
            consensus.nGovernanceMinQuorum = 1;
            consensus.nGovernanceFilterElements = 500;
            consensus.nMasternodeMinimumConfirmations = 1;
            consensus.BIP34Height = 76;
            consensus.BIP34Hash = uint256S("0x000008ebb1db2598e897d17275285767717c6acfeac4c73def49fbea1ddcbcb6");
            consensus.BIP65Height = 2431; // 0000039cf01242c7f921dcb4806a5994bc003b48c1973ae0c89b67809c2bb2ab
            consensus.BIP66Height = 2075; // 0000002acdd29a14583540cb72e1c5cc83783560e38fa7081495d474fe1671f7
            consensus.DIP0001Height = 5500;
            consensus.powLimit = uint256S("00000fffff000000000000000000000000000000000000000000000000000000");
            consensus.nPowTargetTimespan = 60 * 60 * 24; // mct: 1 day
            consensus.nPowTargetSpacing = 2 * 60; // mct: 2 minutes
            consensus.fPowAllowMinDifficultyBlocks = true;
            consensus.fPowNoRetargeting = false;
            consensus.nPowKGWHeight = 4001; // nPowKGWHeight >= nPowDGWHeight means "no KGW"
            consensus.nPowDGWHeight = 4001;

            // Stake info
            consensus.nPosTargetSpacing = 2 * 60; // PoSW: 2 minutes
            consensus.nPosTargetTimespan = 60 * 40;
            consensus.nStakeMinAge = 60; //one minute
            consensus.nStakeMaxAge = 60 * 60 * 24; // one day
            consensus.nLastPoWBlock = 650;
            consensus.nPoSDiffAdjustRange = 1;
            // highest difficulty | 0x1e0ffff0 (?)
            // smallest difficulty | 0x008000
            consensus.nWSTargetDiff = 0x1e0ffff0; // Genesis Difficulty

            consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
            consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
            consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
            consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
            consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

            // Deployment of BIP68, BIP112, and BIP113.
            consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
            consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1506556800; // September 28th, 2017
            consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1538092800; // September 28th, 2018

            // Deployment of DIP0001
            consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].bit = 1;
            consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nStartTime = 1505692800; // Sep 18th, 2017
            consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nTimeout = 1537228800; // Sep 18th, 2018
            consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nWindowSize = 100;
            consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nThreshold = 50; // 50% of 100

            // Deployment of BIP147
            consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].bit = 2;
            consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nStartTime = 1517792400; // Feb 5th, 2018
            consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nTimeout = 1549328400; // Feb 5th, 2019
            consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nWindowSize = 100;
            consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nThreshold = 50; // 50% of 100

            // The best chain should have at least this much work.
            consensus.nMinimumChainWork = uint256S("0x"); // 37900
            // By default assume that the signatures in ancestors of this block are valid.
            consensus.defaultAssumeValid = uint256S("0x"); // 37900


            pchMessageStart[0] = 0xce;
            pchMessageStart[1] = 0xe2;
            pchMessageStart[2] = 0xca;
            pchMessageStart[3] = 0xff;
            vAlertPubKey = ParseHex("040389fde3d4a65b7123096b8f185cd5bc6e90c1d526b0ff36ed3b5b91c6877f3f38ad2201ebaf8389125913828ba8cf2f28634ce57033f423ee71840bc4ce5720");
            nDefaultPort = 21430;
            nPruneAfterHeight = 1000;

            genesis = CreateGenesisBlock(1527453745, 3864711579, 0x1e0ffff0, 1, 50 * COIN);
            consensus.hashGenesisBlock = genesis.GetHash();
            assert(consensus.hashGenesisBlock == uint256S("0x00000e8b5870751168de398f5dfc190108a121cab434004ef84ddf3bcfbcf3df"));
            assert(genesis.hashMerkleRoot == uint256S("0xc807b5315065804037ea051fbe34b77b5ee9ba4e499524c2c37fe0bc37b8ff34"));



            vSeeds.push_back(CDNSSeedData("testnetseed.mctcentral.org", "testnetseed.mctcentral.org"));
            vSeeds.push_back(CDNSSeedData("testnetseed2.mctcentral.org", "testnetseed2.mctcentral.org"));


            // Testnet mct addresses start with 'y'
            base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,140);
            // Testnet mct script addresses start with '8' or '9'
            base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,19);
            // Testnet private keys start with '9' or 'c' (Bitcoin defaults)
            base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
            // Testnet mct BIP32 pubkeys start with 'tpub' (Bitcoin defaults)
            base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
            // Testnet mct BIP32 prvkeys start with 'tprv' (Bitcoin defaults)
            base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

            // Testnet mct BIP44 coin type is '1' (All coin's testnet default)
            nExtCoinType = 1;

            fMiningRequiresPeers = true;
            fDefaultConsistencyChecks = false;
            fRequireStandard = false;
            fMineBlocksOnDemand = false;
            fAllowMultipleAddressesFromGroup = false;
            fAllowMultiplePorts = false;

            nPoolMaxTransactions = 3;
            nFulfilledRequestExpireTime = 5*60; // fulfilled requests expire in 5 minutes


            strSporkAddress = "yMCScEFCuhFGQL8aBS8UPXnKriFtjMVWra";

            checkpointData = (CCheckpointData) {
                    boost::assign::map_list_of
                            (   0, uint256S("0x"))


            };
            chainTxData = ChainTxData{
                    0, // * UNIX timestamp of last checkpoint block
                    0,       // * total number of transactions between genesis and last checkpoint
                    //   (the tx=... number in the SetBestChain debug.log lines)
                    0         // * estimated number of transactions per day after checkpoint

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
            consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 1
            consensus.nPowTargetTimespan = 24 * 60 * 60; // MCT: 1 day
            consensus.nPowTargetSpacing = 2.5 * 60; // MCT: 2.5 minutes
            consensus.fPowAllowMinDifficultyBlocks = true;
            consensus.fPowNoRetargeting = false;
            consensus.nPowKGWHeight = 4001; // nPowKGWHeight >= nPowDGWHeight means "no KGW"
            consensus.nPowDGWHeight = 4001;


            // Stake information

            consensus.nPosTargetSpacing = 2 * 60; // PoSW: 1 minutes
            consensus.nPosTargetTimespan = 60 * 40;
            consensus.nStakeMinAge = 60 * 60;
            consensus.nStakeMaxAge = 60 * 60 * 24; // one day
            consensus.nLastPoWBlock = 180675;



            consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
            consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
            consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
            consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
            consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

            // Deployment of BIP68, BIP112, and BIP113.
            consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
            consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1506556800; // September 28th, 2017
            consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1538092800; // September 28th, 2018

            // Deployment of DIP0001
            consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].bit = 1;
            consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nStartTime = 1505692800; // Sep 18th, 2017
            consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nTimeout = 1537228800; // Sep 18th, 2018
            consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nWindowSize = 100;
            consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nThreshold = 50; // 50% of 100

            // Deployment of BIP147
            consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].bit = 2;
            consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nStartTime = 1517792400; // Feb 5th, 2018
            consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nTimeout = 1549328400; // Feb 5th, 2019
            consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nWindowSize = 100;
            consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nThreshold = 50; // 50% of 100

            // The best chain should have at least this much work.
            consensus.nMinimumChainWork = uint256S("0x000000000000000000000000000000000000000000000000000000000000000");

            // By default assume that the signatures in ancestors of this block are valid.
            consensus.defaultAssumeValid = uint256S("0x000000000000000000000000000000000000000000000000000000000000000");

            pchMessageStart[0] = 0xe2;
            pchMessageStart[1] = 0xca;
            pchMessageStart[2] = 0xff;
            pchMessageStart[3] = 0xce;
            vAlertPubKey = ParseHex("04517d8a699cb43d3938d7b24faaff7cda448ca4ea267723ba614784de661949bf632d6304316b244646dea079735b9a6fc4af804efb4752075b9fe2245e14e412");
            nDefaultPort = 19999;
            nPruneAfterHeight = 1000;

            genesis = CreateGenesisBlock(1527453768, 1096453, 0x207fffff, 1, 50 * COIN);
            consensus.hashGenesisBlock = genesis.GetHash();
            assert(consensus.hashGenesisBlock == uint256S("0x1c8c77a13f08cf4232420d9bb042e4a436ef69ddd309ae3c73d8bb1bb7c2a3b5"));
            assert(genesis.hashMerkleRoot == uint256S("0xc807b5315065804037ea051fbe34b77b5ee9ba4e499524c2c37fe0bc37b8ff34"));

            devnetGenesis = FindDevNetGenesisBlock(consensus, genesis, 50 * COIN);
            consensus.hashDevnetGenesisBlock = devnetGenesis.GetHash();

            vFixedSeeds.clear();
            vSeeds.clear();
            //vSeeds.push_back(CDNSSeedData("mctevo.org",  "devnet-seed.mctevo.org"));

            // Testnet MCT addresses start with 'y'
            base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,140);
            // Testnet MCT script addresses start with '8' or '9'
            base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,19);
            // Testnet private keys start with '9' or 'c' (Bitcoin defaults)
            base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
            // Testnet MCT BIP32 pubkeys start with 'tpub' (Bitcoin defaults)
            base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
            // Testnet MCT BIP32 prvkeys start with 'tprv' (Bitcoin defaults)
            base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

            // Testnet MCT BIP44 coin type is '1' (All coin's testnet default)
            nExtCoinType = 1;

            fMiningRequiresPeers = true;
            fDefaultConsistencyChecks = false;
            fRequireStandard = false;
            fMineBlocksOnDemand = false;
            fAllowMultipleAddressesFromGroup = true;
            fAllowMultiplePorts = true;

            nPoolMaxTransactions = 3;
            nFulfilledRequestExpireTime = 5*60; // fulfilled requests expire in 5 minutes

            strSporkAddress = "yjPtiKh2uwk3bDutTEA2q9mCtXyiZRWn55";

            checkpointData = (CCheckpointData) {
                    boost::assign::map_list_of
                            (      0, uint256S("0x1c8c77a13f08cf4232420d9bb042e4a436ef69ddd309ae3c73d8bb1bb7c2a3b5"))
                            (      1, devnetGenesis.GetHash())
            };

            chainTxData = ChainTxData{
                    devnetGenesis.GetBlockTime(), // * UNIX timestamp of devnet genesis block
                    2,                            // * we only have 2 coinbase transactions when a devnet is started up
                    0.01                          // * estimated number of transactions per second
            };
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
            consensus.nBudgetPaymentsStartBlock = 25;
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
            consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
            consensus.nPowTargetTimespan = 24 * 60 * 60; // mct: 1 day
            consensus.nPowTargetSpacing = 120; // mct: 2.5 minutes
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

            // Stake info
            consensus.nPosTargetSpacing = 30; // PoSW: 1 minutes
            consensus.nPosTargetTimespan = 60 * 40;
            consensus.nStakeMinAge = 60;
            consensus.nStakeMaxAge = 60 * 60 * 24; // one day
            consensus.nLastPoWBlock = 25;
            // highest difficulty | 0x1e0ffff0 (?)
            // smallest difficulty | 0x008000
            consensus.nWSTargetDiff = 0x1e0ffff0; // Genesis Difficulty

            // The best chain should have at least this much work.
            consensus.nMinimumChainWork = uint256S("0x00");

            // By default assume that the signatures in ancestors of this block are valid.
            consensus.defaultAssumeValid = uint256S("0x00");

            pchMessageStart[0] = 0xfc;
            pchMessageStart[1] = 0xc1;
            pchMessageStart[2] = 0xb7;
            pchMessageStart[3] = 0xdc;
            nDefaultPort = 19994;
            nPruneAfterHeight = 1000;

            genesis = CreateGenesisBlock(1527453768, 1096453, 0x207fffff, 1, 50 * COIN);
            consensus.hashGenesisBlock = genesis.GetHash();
            assert(consensus.hashGenesisBlock == uint256S("0x1c8c77a13f08cf4232420d9bb042e4a436ef69ddd309ae3c73d8bb1bb7c2a3b5"));
            assert(genesis.hashMerkleRoot == uint256S("0xc807b5315065804037ea051fbe34b77b5ee9ba4e499524c2c37fe0bc37b8ff34"));


            vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
            vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

            fMiningRequiresPeers = false;
            fDefaultConsistencyChecks = true;
            fRequireStandard = false;
            fMineBlocksOnDemand = true;
            fAllowMultipleAddressesFromGroup = true;
            fAllowMultiplePorts = true;

            nFulfilledRequestExpireTime = 5*60; // fulfilled requests expire in 5 minutes

            // privKey: cP4EKFyJsHT39LDqgdcB43Y3YXjNyjb5Fuas1GQSeAtjnZWmZEQK
            strSporkAddress = "yj949n1UH6fDhw6HtVE5VMj2iSTaSWBMcW";

            checkpointData = (CCheckpointData){
                    boost::assign::map_list_of
                            ( 0, uint256S("0x1c8c77a13f08cf4232420d9bb042e4a436ef69ddd309ae3c73d8bb1bb7c2a3b5"))
            };

            chainTxData = ChainTxData{
                    0,
                    0,
                    0
            };

            base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,140);
            // Regtest mct script addresses start with '8' or '9'
            base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,19);
            // Regtest private keys start with '9' or 'c' (Bitcoin defaults)
            base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
            // Regtest mct BIP32 pubkeys start with 'tpub' (Bitcoin defaults)
            base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
            // Regtest mct BIP32 prvkeys start with 'tprv' (Bitcoin defaults)
            base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

            // Regtest mct BIP44 coin type is '1' (All coin's testnet default)
            nExtCoinType = 1;
    }

    void UpdateBIP9Parameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
    {
            consensus.vDeployments[d].nStartTime = nStartTime;
            consensus.vDeployments[d].nTimeout = nTimeout;
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
                devNetParams = new CDevNetParams();
        }

        SelectBaseParams(network);
        pCurrentParams = &Params(network);
}

void UpdateRegtestBIP9Parameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
{
        regTestParams.UpdateBIP9Parameters(d, nStartTime, nTimeout);
}
