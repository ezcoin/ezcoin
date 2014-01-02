// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2011-2012 Litecoin Developers
// Copyright (c) 2013 Ezcoin developers
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

    //
    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    //
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
        ;                   

    bool CheckBlock(int nHeight, const uint256& hash)
    {	
        if (fTestNet) return true; // Testnet has no checkpoints

        MapCheckpoints::const_iterator i = mapCheckpoints.find(nHeight);
        if (i == mapCheckpoints.end()) return true;
        return hash == i->second;
    }

    int GetTotalBlocksEstimate()
    {
        if (fTestNet) return 0;

        return mapCheckpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        if (fTestNet) return NULL;

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, mapCheckpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }
}
