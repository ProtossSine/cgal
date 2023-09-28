// Copyright (c) 2018  INRIA Sophia-Antipolis (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
//
// $URL$
// $Id$
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-Commercial
//
// Author(s)     : Guillaume Damiand <guillaume.damiand@liris.cnrs.fr>
//                 Mostafa Ashraf <mostaphaashraf1996@gmail.com>

#ifndef CGAL_DRAW_T3_H
#define CGAL_DRAW_T3_H

#include <CGAL/license/Triangulation_3.h>
#include <CGAL/Qt/Basic_viewer_qt.h>
#include <CGAL/Graphic_storage.h>
#include <CGAL/Graphics_scene_options.h>
#include <CGAL/Random.h>
#include <CGAL/Triangulation_3.h>

namespace CGAL {

namespace draw_function_for_t3
{

template <typename BufferType=float, class T3, class GSOptions>
void compute_face(typename T3::Finite_facets_iterator fh,
                  const GSOptions& gs_options,
                  CGAL::Graphic_storage<BufferType>& graphic_storage, const T3 *t3)
{
  if(!gs_options.draw_face(*t3, fh))
  { return; }

  if(gs_options.colored_face(*t3, fh))
  { graphic_storage.face_begin(gs_options.face_color(*t3, fh)); }
  else
  { graphic_storage.face_begin(); }

  graphic_storage.add_point_in_face(fh->first->vertex((fh->second + 1) % 4)->
                                   point());
  graphic_storage.add_point_in_face(fh->first->vertex((fh->second + 2) % 4)->
                                   point());
  graphic_storage.add_point_in_face(fh->first->vertex((fh->second + 3) % 4)->
                                   point());

  graphic_storage.face_end();
}

template <typename BufferType=float, class T3, class GSOptions>
void compute_edge(typename T3::Finite_edges_iterator eh,
                  CGAL::Graphic_storage<BufferType>& graphic_storage,
                  const GSOptions& gs_options, const T3* t3)
{
  if(!gs_options.draw_edge(*t3, eh))
  { return; }

  if(gs_options.colored_edge(*t3, eh))
  {
    graphic_storage.add_segment(eh->first->vertex(eh->second)->point(),
                               eh->first->vertex(eh->third)->point(),
                               gs_options.edge_color(*t3, eh));
  }
  else {
    graphic_storage.add_segment(eh->first->vertex(eh->second)->point(),
                               eh->first->vertex(eh->third)->point());
  }
}

template <typename BufferType=float, class T3, class GSOptions>
void compute_vertex(typename T3::Vertex_handle vh,
                    CGAL::Graphic_storage<BufferType>& graphic_storage,
                    const GSOptions& gs_options, const T3* t3)
{
  if(!gs_options.draw_vertex(*t3, vh))
  { return; }

  if(gs_options.colored_vertex(*t3, vh))
  {
    graphic_storage.add_point(vh->point(), gs_options.vertex_color(*t3, vh));
  }
  else
  { graphic_storage.add_point(vh->point()); }
}

template <typename BufferType=float, class T3, class GSOptions>
void compute_elements(const T3* t3,
                      CGAL::Graphic_storage<BufferType>& graphic_storage,
                      const GSOptions& gs_options)
{
  if (gs_options.are_faces_enabled())
  {
    for (typename T3::Finite_facets_iterator it=t3->finite_facets_begin();
         it!=t3->finite_facets_end(); ++it)
    { compute_face(it, gs_options, graphic_storage, t3); }
  }

  if (gs_options.are_edges_enabled())
  {
    for (typename T3::Finite_edges_iterator it=t3->finite_edges_begin();
         it!=t3->finite_edges_end(); ++it)
    { compute_edge(it, graphic_storage,gs_options, t3); }
  }

  if (gs_options.are_vertices_enabled())
  {
    for (typename T3::Finite_vertices_iterator it=t3->finite_vertices_begin();
         it!=t3->finite_vertices_end(); ++it)
    { compute_vertex(it, graphic_storage, gs_options, t3); }
  }
}

} // namespace draw_function_for_t3

#define CGAL_T3_TYPE CGAL::Triangulation_3<Gt, Tds, Lock_data_structure>

template <class Gt, class Tds, class Lock_data_structure,
          typename BufferType=float, class GSOptions>
void add_in_graphic_storage(const CGAL_T3_TYPE& at3,
                           CGAL::Graphic_storage<BufferType>& graphic_storage,
                           const GSOptions& gs_options)
{
  draw_function_for_t3::compute_elements(&at3, graphic_storage, gs_options);
}

template <class Gt, class Tds, class Lock_data_structure,
          typename BufferType=float>
void add_in_graphic_storage(const CGAL_T3_TYPE& at3,
                           CGAL::Graphic_storage<BufferType>& graphic_storage)
{
  CGAL::Graphics_scene_options<CGAL_T3_TYPE,
                       typename CGAL_T3_TYPE::Vertex_handle,
                       typename CGAL_T3_TYPE::Finite_edges_iterator,
                       typename CGAL_T3_TYPE::Finite_facets_iterator>
    gs_options;

  gs_options.colored_face =
    [](const CGAL_T3_TYPE &, const typename CGAL_T3_TYPE::Finite_facets_iterator) -> bool
    { return true; };

  gs_options.face_color =
    [](const CGAL_T3_TYPE &at3, const typename CGAL_T3_TYPE::Finite_facets_iterator fh) -> CGAL::IO::Color
    {
      if (fh==at3.finite_facets_end())         // use to get the mono color
        return CGAL::IO::Color(100, 125, 200); // R G B between 0-255

      CGAL::Random random((unsigned int)((std::size_t)(&*(fh->first)) +
                                         (std::size_t)(fh->second)));

      return get_random_color(random);
    };

  add_in_graphic_storage(at3, graphic_storage, gs_options);
}

#ifdef CGAL_USE_BASIC_VIEWER

// Specialization of draw function.
template<class Gt, class Tds, class Lock_data_structure, class GSOptions>
void draw(const CGAL_T3_TYPE &at3, const GSOptions &gs_options,
          const char *title="T3 Basic Viewer")
{
  CGAL::Graphic_storage<float> buffer;
  add_in_graphic_storage(at3, buffer, gs_options);
  draw_graphic_storage(buffer, title);
}

template <class Gt, class Tds, class Lock_data_structure>
void draw(const CGAL_T3_TYPE &at3, const char *title="T3 Basic Viewer")
{
  CGAL::Graphic_storage<float> buffer;
  add_in_graphic_storage(at3, buffer);
  draw_graphic_storage(buffer, title);
}

#endif // CGAL_USE_BASIC_VIEWER

#undef CGAL_T3_TYPE

} // End namespace CGAL

#endif // CGAL_DRAW_T3_H
