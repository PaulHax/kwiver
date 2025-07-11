// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// Interface to the KLV set template class.

#ifndef KWIVER_VITAL_KLV_KLV_SET_H_
#define KWIVER_VITAL_KLV_KLV_SET_H_

#include <arrows/klv/klv_data_format.h>
#include <arrows/klv/klv_tag_traits.h>
#include <arrows/klv/klv_value.h>
#include <arrows/klv/kwiver_algo_klv_export.h>

#include <vital/range/iterator_range.h>

#include <map>

namespace kwiver {

namespace arrows {

namespace klv {

// ----------------------------------------------------------------------------
/// Base class for KLV universal and local sets.
///
/// This class abstracts sets into a map-like structure where some keys may have
/// multiple values, but with utility methods that recognize that most KLV keys
/// do not. There is no guarantee as to the order of the key-value pairs when
/// iterating. Notably, this class does not maintain the order of insertion, as,
/// with the single exception of the SDCC-FLP item, order in KLV sets is decreed
/// to not matter. Any constructs such as SDCC-FLP which are order-sensitive or
/// have other constraints (e.g. tags which may not appear more than once) must
/// maintain their invariants at a higher level than this data structure.
template < class Key >
class KWIVER_ALGO_KLV_EXPORT klv_set
{
public:
  using container = std::multimap< Key, klv_value >;
  using iterator = typename container::iterator;
  using const_iterator = typename container::const_iterator;
  using value_type = typename container::value_type;
  using range = kwiver::vital::range::iterator_range< iterator >;
  using const_range = kwiver::vital::range::iterator_range< const_iterator >;

  klv_set();

  klv_set( container const& items );

  klv_set( std::initializer_list< value_type > const& items );

  iterator
  begin();

  const_iterator
  begin() const;

  const_iterator
  cbegin() const;

  iterator
  end();

  const_iterator
  end() const;

  const_iterator
  cend() const;

  bool
  empty() const;

  size_t
  size() const;

  size_t
  count( Key const& key ) const;

  bool
  has( Key const& key ) const;

  void
  add( Key const& key, klv_value const& datum );

  iterator
  erase( const_iterator it );

  void
  erase( Key const& key );

  void
  clear();

  /// Return single entry corresponding to \p key, or end iterator on failure.
  iterator
  find( Key const& key );

  /// \copydoc iterator find( Key const& key )
  const_iterator
  find( Key const& key ) const;

  /// Return single value corresponding to \p key.
  ///
  /// \throws out_of_range If no \p key entry is present.
  /// \throws logic_error If more than one \p key entry is present.
  klv_value&
  at( Key const& key );

  /// \copydoc klv_value& at( Key const& key )
  klv_value const&
  at( Key const& key ) const;

  /// Return the range of entries corresponding to \p key.
  ///
  /// \note Order of entries returned is not defined.
  range
  all_at( Key const& key );

  /// \copydoc range all_at( Key const& key )
  const_range
  all_at( Key const& key ) const;

  /// Returns iterators to all entries, sorted by key, then by value.
  std::vector< const_iterator >
  fully_sorted() const;

  // Sort by key, then value.
  static bool
  value_compare( const_iterator lhs, const_iterator rhs );

private:
  std::multimap< Key, klv_value > m_items;
};

// ----------------------------------------------------------------------------
template < class Key >
KWIVER_ALGO_KLV_EXPORT
bool
operator==( klv_set< Key > const& lhs, klv_set< Key > const& rhs );

// ----------------------------------------------------------------------------
template < class Key >
KWIVER_ALGO_KLV_EXPORT
bool
operator<( klv_set< Key > const& lhs, klv_set< Key > const& rhs );

// ----------------------------------------------------------------------------
template < class Key >
KWIVER_ALGO_KLV_EXPORT
std::ostream&
operator<<( std::ostream& os, klv_set< Key > const& rhs );

// ----------------------------------------------------------------------------
/// Interprets data as a local or universal set.
template < class Key >
class KWIVER_ALGO_KLV_EXPORT klv_set_format
  : public klv_data_format_< klv_set< Key > >
{
public:
  explicit
  klv_set_format( klv_tag_traits_lookup const& traits );

  virtual
  ~klv_set_format();

  klv_tag_traits_lookup const&
  traits() const;

  klv_set< Key >
  read_typed( klv_read_iter_t& data, size_t length ) const override;

  void
  write_typed(
    klv_set< Key > const& klv,
    klv_write_iter_t& data, size_t length ) const override;

  size_t
  length_of_typed( klv_set< Key > const& value ) const override;

  std::ostream&
  print_typed( std::ostream& os, klv_set< Key > const& value ) const;

protected:
  // Print warnings if tags appear too few or too many times in the given set.
  void
  check_tag_counts( klv_set< Key > const& klv ) const;

  virtual void
  check_set( klv_set< Key > const& klv ) const;

private:
  klv_tag_traits_lookup const& m_traits;
};

// ----------------------------------------------------------------------------
/// KLV local set. Key-value pairs of a format defined by a standard.
using klv_local_set = klv_set< klv_lds_key >;

// ----------------------------------------------------------------------------
/// Interprets data as a KLV local set.
using klv_local_set_format = klv_set_format< klv_lds_key >;

// ----------------------------------------------------------------------------
/// KLV universal set. Key-value pairs of a format defined by a standard.
using klv_universal_set = klv_set< klv_uds_key >;

// ----------------------------------------------------------------------------
/// Interprets data as a KLV universal set.
using klv_universal_set_format = klv_set_format< klv_uds_key >;

} // namespace klv

} // namespace arrows

} // namespace kwiver

#endif
