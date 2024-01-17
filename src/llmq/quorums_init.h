// Copyright (c) 2023 The Hellar Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef HEL_QUORUMS_INIT_H
#define HEL_QUORUMS_INIT_H

class CProDB;

namespace llmq
{

void InitLLMQSystem(CProDB& proDb);
void DestroyLLMQSystem();

}

#endif //HEL_QUORUMS_INIT_H
