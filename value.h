#ifndef _VALUE_H
#define _VALUE_H

#include "amount.h"
#include "balance.h"

#include <exception>

namespace ledger {

class value_error : public std::exception {
  std::string reason;
 public:
  value_error(const std::string& _reason) throw() : reason(_reason) {}
  virtual ~value_error() throw() {}

  virtual const char* what() const throw() {
    return reason.c_str();
  }
};

// The following type is a polymorphous value type used solely for
// performance reasons.  The alternative is to compute value
// expressions (valexpr.cc) in terms of the largest data type,
// balance_t. This was found to be prohibitively expensive, especially
// when large logic chains were involved, since many temporary
// allocations would occur for every operator.  With value_t, and the
// fact that logic chains only need boolean values to continue, no
// memory allocations need to take place at all.

class value_t
{
 public:
  char data[sizeof(balance_pair_t)];

  enum type_t {
    BOOLEAN,
    INTEGER,
    AMOUNT,
    BALANCE,
    BALANCE_PAIR
  } type;

  value_t() {
    *((long *) data) = 0;
    type = INTEGER;
  }

  value_t(const value_t& value) : type(INTEGER) {
    *this = value;
  }
  value_t(const bool value) {
    *((bool *) data) = value;
    type = BOOLEAN;
  }
  value_t(const long value) {
    *((long *) data) = value;
    type = INTEGER;
  }
  value_t(const unsigned long value) {
    new((amount_t *) data) amount_t(value);
    type = AMOUNT;
  }
  value_t(const double value) {
    new((amount_t *) data) amount_t(value);
    type = AMOUNT;
  }
  value_t(const std::string& value) {
    new((amount_t *) data) amount_t(value);
    type = AMOUNT;
  }
  value_t(const char * value) {
    new((amount_t *) data) amount_t(value);
    type = AMOUNT;
  }
  value_t(const amount_t& value) {
    new((amount_t *)data) amount_t(value);
    type = AMOUNT;
  }
  value_t(const balance_t& value) : type(INTEGER) {
    *this = value;
  }
  value_t(const balance_pair_t& value) : type(INTEGER) {
    *this = value;
  }

  ~value_t() {
    destroy();
  }

  void destroy();

  value_t& operator=(const value_t& value);
  value_t& operator=(const bool value) {
    if ((bool *) data != &value) {
      destroy();
      *((bool *) data) = value;
      type = BOOLEAN;
    }
    return *this;
  }
  value_t& operator=(const long value) {
    if ((long *) data != &value) {
      destroy();
      *((long *) data) = value;
      type = INTEGER;
    }
    return *this;
  }
  value_t& operator=(const unsigned long value) {
    return *this = amount_t(value);
  }
  value_t& operator=(const double value) {
    return *this = amount_t(value);
  }
  value_t& operator=(const std::string& value) {
    return *this = amount_t(value);
  }
  value_t& operator=(const char * value) {
    return *this = amount_t(value);
  }
  value_t& operator=(const amount_t& value) {
    if (type == AMOUNT &&
	(amount_t *) data == &value)
      return *this;
    
    destroy();
    new((amount_t *)data) amount_t(value);
    type = AMOUNT;

    return *this;
  }
  value_t& operator=(const balance_t& value) {
    if (type == BALANCE &&
	(balance_t *) data == &value)
      return *this;
    
    if (value.amounts.size() == 1) {
      return *this = (*value.amounts.begin()).second;
    } else {
      destroy();
      new((balance_t *)data) balance_t(value);
      type = BALANCE;
      return *this;
    }
  }
  value_t& operator=(const balance_pair_t& value) {
    if (type == BALANCE_PAIR &&
	(balance_pair_t *) data == &value)
      return *this;
    
    if (! value.cost) {
      return *this = value.quantity;
    } else {
      destroy();
      new((balance_pair_t *)data) balance_pair_t(value);
      type = BALANCE_PAIR;
      return *this;
    }
  }

  value_t& operator+=(const value_t& value);
  value_t& operator-=(const value_t& value);
  value_t& operator*=(const value_t& value);
  value_t& operator/=(const value_t& value);

  template <typename T>
  value_t& operator+=(const T& value) {
    return *this += value_t(value);
  }
  template <typename T>
  value_t& operator-=(const T& value) {
    return *this -= value_t(value);
  }
  template <typename T>
  value_t& operator*=(const T& value) {
    return *this *= value_t(value);
  }
  template <typename T>
  value_t& operator/=(const T& value) {
    return *this /= value_t(value);
  }

  value_t operator+(const value_t& value) {
    value_t temp(*this);
    temp += value;
    return temp;
  }
  value_t operator-(const value_t& value) {
    value_t temp(*this);
    temp -= value;
    return temp;
  }
  value_t operator*(const value_t& value) {
    value_t temp(*this);
    temp *= value;
    return temp;
  }
  value_t operator/(const value_t& value) {
    value_t temp(*this);
    temp /= value;
    return temp;
  }

  template <typename T>
  value_t operator+(const T& value) {
    return *this + value_t(value);
  }
  template <typename T>
  value_t operator-(const T& value) {
    return *this - value_t(value);
  }
  template <typename T>
  value_t operator*(const T& value) {
    return *this * value_t(value);
  }
  template <typename T>
  value_t operator/(const T& value) {
    return *this / value_t(value);
  }

  bool operator<(const value_t& value);
  bool operator<=(const value_t& value);
  bool operator>(const value_t& value);
  bool operator>=(const value_t& value);
  bool operator==(const value_t& value);
  bool operator!=(const value_t& value) {
    return ! (*this == value);
  }

  template <typename T>
  bool operator<(const T& value) {
    return *this < value_t(value);
  }
  template <typename T>
  bool operator<=(const T& value) {
    return *this <= value_t(value);
  }
  template <typename T>
  bool operator>(const T& value) {
    return *this > value_t(value);
  }
  template <typename T>
  bool operator>=(const T& value) {
    return *this >= value_t(value);
  }
  template <typename T>
  bool operator==(const T& value) {
    return *this == value_t(value);
  }
  template <typename T>
  bool operator!=(const T& value) {
    return ! (*this == value);
  }

  template <typename T>
  operator T() const;

  void negate();
  value_t negated() const {
    value_t temp = *this;
    temp.negate();
    return temp;
  }
  value_t operator-() const {
    return negated();
  }

  void     abs();
  void     cast(type_t cast_type);
  value_t  cost() const;
  value_t& add(const amount_t& amount, const amount_t * cost = NULL);

  value_t value(const std::time_t moment) const {
    switch (type) {
    case BOOLEAN:
    case INTEGER:
      return *this;
    case AMOUNT:
      return ((amount_t *) data)->value(moment);
    case BALANCE:
      return ((balance_t *) data)->value(moment);
    case BALANCE_PAIR:
      return ((balance_pair_t *) data)->quantity.value(moment);
    }
  }

  void round() {
    switch (type) {
    case BOOLEAN:
    case INTEGER:
      break;
    case AMOUNT: {
      amount_t& amount = *((amount_t *) data);
      if (amount.commodity())
	amount = amount.round(amount.commodity().precision);
      break;
    }
    case BALANCE:
      ((balance_t *) data)->round();
      break;
    case BALANCE_PAIR:
      ((balance_pair_t *) data)->round();
      break;
    }
  }
};

#define DEF_VALUE_AUX_OP(OP)					\
  inline value_t operator OP(const balance_pair_t& value,	\
			     const value_t& obj) {		\
    return value_t(value) OP obj;				\
  }								\
  inline value_t operator OP(const balance_t& value,		\
			     const value_t& obj) {		\
    return value_t(value) OP obj;				\
  }								\
  inline value_t operator OP(const amount_t& value,		\
			     const value_t& obj) {		\
    return value_t(value) OP obj;				\
  }								\
  template <typename T>						\
  inline value_t operator OP(T value, const value_t& obj) {	\
    return value_t(value) OP obj;				\
  }

DEF_VALUE_AUX_OP(+)
DEF_VALUE_AUX_OP(-)
DEF_VALUE_AUX_OP(*)
DEF_VALUE_AUX_OP(/)

DEF_VALUE_AUX_OP(<)
DEF_VALUE_AUX_OP(<=)
DEF_VALUE_AUX_OP(>)
DEF_VALUE_AUX_OP(>=)
DEF_VALUE_AUX_OP(==)
DEF_VALUE_AUX_OP(!=)

template <typename T>
value_t::operator T() const
{
  switch (type) {
  case BOOLEAN:
    return *((bool *) data);
  case INTEGER:
    return *((long *) data);
  case AMOUNT:
    return *((amount_t *) data);
  case BALANCE:
    return *((balance_t *) data);
  case BALANCE_PAIR:
    return *((balance_pair_t *) data);

  default:
    assert(0);
    break;
  }
  assert(0);
  return 0;
}

template <> value_t::operator long() const;
template <> value_t::operator double() const;

inline value_t abs(const value_t& value) {
  value_t temp(value);
  temp.abs();
  return temp;
}

inline std::ostream& operator<<(std::ostream& out, const value_t& value) {
  switch (value.type) {
  case value_t::BOOLEAN:
    out << *((bool *) value.data);
    break;
  case value_t::INTEGER:
    out << *((long *) value.data);
    break;
  case value_t::AMOUNT:
    out << *((amount_t *) value.data);
    break;
  case value_t::BALANCE:
    out << *((balance_t *) value.data);
    break;
  case value_t::BALANCE_PAIR:
    out << *((balance_pair_t *) value.data);
    break;

  default:
    assert(0);
    break;
  }
  return out;
}

} // namespace ledger

#endif // _VALUE_H