// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "main.h"
#include "uint256.h"

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    // How many times we expect transactions after the last checkpoint to
    // be slower. This number is a compromise, as it can't be accurate for
    // every system. When reindexing from a fast disk with a slow CPU, it
    // can be up to 20, while when downloading from a slow network with a
    // fast multicore CPU, it won't be much higher than 1.
    static const double fSigcheckVerificationFactor = 5.0;

    struct CCheckpointData {
        const MapCheckpoints *mapCheckpoints;
        int64 nTimeLastCheckpoint;
        int64 nTransactionsLastCheckpoint;
        double fTransactionsPerDay;
    };

    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        (     0, uint256("0x61e2df0bf8cc0a43960d7d2fcfc605935f0848a6ba53ab8d46b98ab4463adc3e"))
        (  1000, uint256("0x9acf0e40c53c8bd88bb292e6a9efc7e54ebdb305a12462cd7172b30e37eafecb"))
        (  2500, uint256("0xd8b7d76354b1308de398073a5dfabb0f9149faf86c5442012f0e57c97fa1e3ca"))
        (  4000, uint256("0xe83ee0f0d4e28e43a4d956e550521bc270db5ffc78d861df3db91d81a35cc1b6"))
        (  5000, uint256("0xe3fe3e80b95fdd46299f7e8536176518d242851d495bba2ce1921b20f87a4def"))
		( 10000, uint256("0xcff48972378a3f838e26de2a9386cc2eb8060f7e99d52d9bf3c642afc7d351c5"))
		( 20000, uint256("0xed164fb6b78f9f24e616bcdf61c4b716320bcf0216c4f47d2706ff8e233ac1e3"))
		( 30001, uint256("0x7b444c82c92a10f253ff41c8ab2376935c97edd4e308c48bb76c9476cbf77884"))
		( 44161, uint256("0x8a547e3a271034c8db2a948bc190466d7c7611e3983af6ea147e54f67cbea8d9"))
		( 65001, uint256("0x481b140ca7b02cd2f3c31f3507b2cea77c30b610de87d1d26d62aa98d1c9cfdb"))
		(170002, uint256("0xe0ba190af0736b23c0a8ca15003daf76fd900f5e9936c35a5db2557f674f7d2f"))
		(259472, uint256("0x57997222d73086b043837db343cee6940b2e8882c68a21ffbdd789d6d7f926fe"))
		(279022, uint256("0xd427274166f0b579e61a22921de6d853e8e461541e5f296bdf1eca8fe9e6319e"))
		(303303, uint256("0x906293053f8dd6117d2f6c086db511ce6db031528ff1fa4c3adb5eca8ea112cf"))
		(334467, uint256("0x34ce42c5168573a8b1a864edabe447645c876d90c62a2e44164ec842ea2a5006"))
		(360004, uint256("0xd1308a833690c10a4c3cd75a7e919cd4e27dad25953b09658de17658a6b5a8f8"))
        ;                   
    static const CCheckpointData data = {
        &mapCheckpoints,
        1402691751, // * UNIX timestamp of last checkpoint block
        4404988,    // * total number of transactions between genesis and last checkpoint
                    //   (the tx=... number in the SetBestChain debug.log lines)
        10000.0     // * estimated number of transactions per day after checkpoint
    };

    static MapCheckpoints mapCheckpointsTestnet =
        boost::assign::map_list_of
        (   546, uint256("0xa0fea99a6897f531600c8ae53367b126824fd6a847b2b2b73817a95b8e27e602"))
        ;
    static const CCheckpointData dataTestnet = {
        &mapCheckpointsTestnet,
        1365458829,
        547,
        576
    };

    const CCheckpointData &Checkpoints() {
        if (fTestNet)
            return dataTestnet;
        else
            return data;
    }

    bool CheckBlock(int nHeight, const uint256& hash)
    {
        if (!GetBoolArg("-checkpoints", true))
            return true;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    // Guess how far we are in the verification process at the given block index
    double GuessVerificationProgress(CBlockIndex *pindex) {
        if (pindex==NULL)
            return 0.0;

        int64 nNow = time(NULL);

        double fWorkBefore = 0.0; // Amount of work done before pindex
        double fWorkAfter = 0.0;  // Amount of work left after pindex (estimated)
        // Work is defined as: 1.0 per transaction before the last checkoint, and
        // fSigcheckVerificationFactor per transaction after.

        const CCheckpointData &data = Checkpoints();

        if (pindex->nChainTx <= data.nTransactionsLastCheckpoint) {
            double nCheapBefore = pindex->nChainTx;
            double nCheapAfter = data.nTransactionsLastCheckpoint - pindex->nChainTx;
            double nExpensiveAfter = (nNow - data.nTimeLastCheckpoint)/86400.0*data.fTransactionsPerDay;
            fWorkBefore = nCheapBefore;
            fWorkAfter = nCheapAfter + nExpensiveAfter*fSigcheckVerificationFactor;
        } else {
            double nCheapBefore = data.nTransactionsLastCheckpoint;
            double nExpensiveBefore = pindex->nChainTx - data.nTransactionsLastCheckpoint;
            double nExpensiveAfter = (nNow - pindex->nTime)/86400.0*data.fTransactionsPerDay;
            fWorkBefore = nCheapBefore + nExpensiveBefore*fSigcheckVerificationFactor;
            fWorkAfter = nExpensiveAfter*fSigcheckVerificationFactor;
        }

        return fWorkBefore / (fWorkBefore + fWorkAfter);
    }

    int GetTotalBlocksEstimate()
    {
        if (!GetBoolArg("-checkpoints", true))
            return 0;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        if (!GetBoolArg("-checkpoints", true))
            return NULL;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }
}
