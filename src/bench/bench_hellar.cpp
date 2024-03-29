// Copyright (c) 2023-2024 The Hellar Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "bench.h"

#include "key.h"
#include "validation.h"
#include "util.h"

#include "bls/bls.h"

void CleanupBLSTests();
void CleanupBLSDkgTests();

int
main(int argc, char** argv)
{
    ECC_Start();
    ECCVerifyHandle verifyHandle;

    BLSInit();
    SetupEnvironment();
    fPrintToDebugLog = false; // don't want to write to debug.log file

    benchmark::BenchRunner::RunAll();

    // need to be called before global destructors kick in (PoolAllocator is needed due to many BLSSecretKeys)
    CleanupBLSDkgTests();
    CleanupBLSTests();

    ECC_Stop();
}
