#ifndef PARAMS_H
#define PARAMS_H

#define PARAMS_N 976
#define PARAMS_NBAR 8
#define PARAMS_LOGQ 16
#define PARAMS_Q (1 << PARAMS_LOGQ)
#define PARAMS_EXTRACTED_BITS 3
#define PARAMS_STRIPE_STEP 8
#define PARAMS_PARALLEL 4
#define BYTES_SEED_A 16
#define BYTES_MU ((PARAMS_EXTRACTED_BITS * PARAMS_NBAR * PARAMS_NBAR) / 8)
#define BYTES_PKHASH CRYPTO_BYTES

// Selecting SHAKE XOF function for the KEM and noise sampling
#define shake     shake256

// CDF table
#define CDF_TABLE_DATA {5638, 15915, 23689, 28571, 31116, 32217, 32613, 32731, 32760, 32766, 32767}
#define CDF_TABLE_LEN 11

#endif
