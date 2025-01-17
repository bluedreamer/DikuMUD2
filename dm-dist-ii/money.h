#pragma once

#include "money_type.h"
#include "structs.h"
#include "utils.h"

using amount_t = int32_t;

/* Give a number of coins (of a specific type) to unit
 * (type == -1 means money_to_unit with local_currency(unit)
 *  Used for database backwards compatibility...)
 */
void coins_to_unit(unit_data *, amount_t amt, int type);

/* Use this if an amount needs to be physically transfered.
 * Money is created/deleted if either unit is NULL.
 *
 * Impossible amounts are converted automagically
 */
void money_transfer(unit_data *from, unit_data *to, amount_t amt, currency_t currency);

/*  Counts up what amount of a given currency a unit holds recursively in
 *  inventory.
 *  Use ANY_CURRENCY as currency-type to count up ALL money...
 */
auto unit_holds_total(unit_data *u, currency_t currency) -> amount_t;

/*  Counts up what amount of a given currency char holds in inventory.
 *  Use ANY_CURRENCY as currency-type to count up ALL money...
 */
auto char_holds_amount(unit_data *ch, currency_t currency) -> amount_t;

/* Can char afford amt in currency?
 *
 * Impossible amounts are converted automagically
 */
auto char_can_afford(unit_data *ch, amount_t amt, currency_t currency) -> bool;

/* Does unit contain any money of type?
 */
auto unit_has_money_type(unit_data *unit, uint8_t type) -> unit_data *;

/* Split `money' into two objects.  Return object with amount `amt'
 */
auto split_money(unit_data *money, amount_t amt) -> unit_data *;

/* Set all the values on money correctly according to amount - return money
 * In general: DON'T use, as the db handles this correctly...
 */
auto set_money(unit_data *money, amount_t amt) -> unit_data *;

/* Check to see if UNIT_IN(money) contains any money of same type, and
 * if so, merge the piles
 */
void pile_money(unit_data *money);

/*  Round amount down/up to nearest `types' number of coins
 */
auto money_round(bool up, amount_t amt, currency_t currency, int types) -> amount_t;

#define money_round_up(a, c, t)   (money_round(TRUE, (a), (c), (t)))
#define money_round_down(a, c, t) (money_round(FALSE, (a), (c), (t)))

/* Local currency of unit, or DEF_CURRENCY if not defined.
 */
auto local_currency(unit_data *unit) -> currency_t;

/* Print out representation of money-object with the amount amt .
 * (amt == 0 means all)
 */
auto obj_money_string(unit_data *obj, amount_t amt) -> char *;

/* Print out optimal representation of amt in currency
 *
 * Impossible amounts are converted automagically
 */
auto money_string(amount_t amt, currency_t currency, bool verbose) -> const char *;

/* How many `coins' of given money-object can char carry, resp. unit contain
 *   (Naturally the amount of money is an upper bound)
 */
auto char_can_carry_amount(unit_data *ch, unit_data *money) -> amount_t;
auto unit_can_hold_amount(unit_data *unit, unit_data *money) -> amount_t;

#define money_pluralis_type(type) (money_types[(type)].strings[money_types[(type)].pl_idx])
#define money_pluralis(unit)      (money_pluralis_type(MONEY_TYPE(unit)))

#define money_singularis_type(type) (money_types[(type)].strings[0])
#define money_singularis(unit)      (money_singularis_type(MONEY_TYPE(unit)))

#define money_from_unit(unit, amt, currency) (money_transfer((unit), NULL, (amt), (currency)))

#define money_to_unit(unit, amt, currency) (money_transfer(NULL, (unit), (amt), (currency)))

#define IS_MONEY(unit) (IS_OBJ(unit) && OBJ_TYPE(unit) == ITEM_MONEY)

//#define MONEY_AMOUNT(unit) (OBJ_PRICE(unit)) // MS2020: was  ((amount_t) OBJ_PRICE(unit))
inline auto MONEY_AMOUNT(unit_data *unit) -> uint32_t &
{
   return OBJ_PRICE(unit);
}

/* Index into money-array */
#define MONEY_TYPE(obj) (OBJ_VALUE((obj), 0))

#define MONEY_CURRENCY(obj) (money_types[MONEY_TYPE(obj)].currency)

#define MONEY_RELATIVE(obj) (money_types[MONEY_TYPE(obj)].relative_value)

#define MONEY_MIN_VALUE(obj) (money_types[MONEY_TYPE(obj)].min_value)

#define MONEY_WEIGHT(obj) (money_types[MONEY_TYPE(obj)].coins_per_weight)

#define MONEY_VALUE(obj) (MONEY_AMOUNT(obj) * MONEY_RELATIVE(obj))

#define MONEY_WEIGHT_SUM(obj1, obj2) ((MONEY_AMOUNT(obj1) + MONEY_AMOUNT(obj2)) / MONEY_WEIGHT(obj1))
