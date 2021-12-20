#pragma once
#include <cstdint>

class pc_account_data
{
public:
   float    credit;       /* How many coin units are left on account?       */
   uint32_t credit_limit; /* In coin units (i.e. cents / oerer)             */
   uint32_t total_credit; /* Accumulated credit to date (coin units)        */
   int16_t  last4;        /* The last four digits of his credit card, or -1 */
   uint8_t  cracks;       /* Crack-attempts on CC last4                     */
   uint8_t  discount;     /* 0 - 100% discount                              */
   uint32_t flatrate;     /* The expiration date of a flat rate service     */
};
