// Copyright (c) 2007-2020  INRIA (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
//
// $URL$
// $Id$
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-Commercial
//
// Author(s)     : Jackson Campolattaro, Cédric Portaneri, Tong Zhao

#ifndef CGAL_ORTHTREE_TRAVERSAL_ITERATOR_H
#define CGAL_ORTHTREE_TRAVERSAL_ITERATOR_H

#include <CGAL/license/Orthtree.h>

#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <boost/iterator/iterator_facade.hpp>

/// \cond SKIP_IN_MANUAL

namespace CGAL {

/*!
 * \ingroup PkgOrthtreeClasses
 *
 * \brief
 *
 * \todo
 *
 * \tparam Value
 */
template <class Tree>
class Traversal_iterator
  : public boost::iterator_facade<Traversal_iterator<Tree>, const typename Tree::Node, boost::forward_traversal_tag> {
public:

  /// \name Types
  /// @{

  /*!
   * \brief
   *
   * \todo
   */
  typedef std::function<boost::optional<std::size_t>(const Tree&, std::size_t)> Traversal_function;

  typedef typename Tree::Node_index Node_index;

  /// @}

public:

  /// \name Creation
  /// @{

  /*!
   * \brief Default constructor, creates an end sentinel
   *
   * \todo
   */
  Traversal_iterator() : m_next() {}

  /*!
   * \brief
   *
   * \todo
   *
   * \param tree
   * \param first
   * \param next
   */
  Traversal_iterator(const Tree& tree, Node_index first, const Traversal_function& next) :
    m_tree(&tree), m_index(first), m_next(next) {}

  /// @}

private:

  friend class boost::iterator_core_access;

  bool equal(Traversal_iterator<Tree> const& other) const {
    return m_index == other.m_index;
  }

  void increment() {
    // invoking increment on the sentinel is undefined behavior
    m_index = m_next(*m_tree, m_index.get());
  }

  const typename Tree::Node& dereference() const {
    return (*m_tree)[m_index.get()];
  }

private:

  Traversal_function m_next;

  boost::optional<std::size_t> m_index = {};
  const Tree* m_tree = nullptr;

};

template <class Tree>
class Index_traversal_iterator : public boost::iterator_facade<
  Index_traversal_iterator<Tree>,
  const typename Tree::Node_index,
  boost::forward_traversal_tag
> {
public:

  /// \name Types
  /// @{

  /*!
   * \brief
   *
   * \todo
   */
  typedef std::function<boost::optional<std::size_t>(const Tree&, std::size_t)> Traversal_function;

  typedef typename Tree::Node_index Node_index;

  /// @}

public:

  /// \name Creation
  /// @{

  /*!
   * \brief Default constructor, creates an end sentinel
   *
   * \todo
   */
  Index_traversal_iterator() : m_next() {}

  /*!
   * \brief
   *
   * \todo
   *
   * \param tree
   * \param first
   * \param next
   */
  Index_traversal_iterator(const Tree& tree, Node_index first, const Traversal_function& next) :
    m_tree(&tree), m_index(first), m_next(next) {}

  /// @}

private:

  friend class boost::iterator_core_access;

  bool equal(Index_traversal_iterator<Tree> const& other) const {
    return m_index == other.m_index;
  }

  void increment() {
    // invoking increment on the sentinel is undefined behavior
    m_index = m_next(*m_tree, m_index.get());
  }

  Node_index dereference() const {
    return m_index.get();
  }

private:

  Traversal_function m_next;

  boost::optional<std::size_t> m_index = {};
  const Tree* m_tree = nullptr;

};

}

/// \endcond

#endif //CGAL_ORTHTREE_TRAVERSAL_ITERATOR_H
