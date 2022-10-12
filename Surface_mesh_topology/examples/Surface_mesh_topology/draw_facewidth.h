#ifndef DRAW_FACEWIDTH_H
#define DRAW_FACEWIDTH_H

#include <iostream>

#ifdef CGAL_USE_BASIC_VIEWER

#include <CGAL/draw_face_graph_with_paths.h>
#include <CGAL/Drawing_functor.h>

template<typename ALCC>
struct Facewidth_draw_functor
{
  Facewidth_draw_functor(typename ALCC::size_type amark1, typename ALCC::size_type amark2) :
    m_vertex_mark(amark1), m_face_mark(amark2)
  {}

  template<typename LCC>
  bool colored_vertex(const LCC& alcc, typename LCC::Dart_const_handle dh) const
  { return alcc.is_marked(dh, m_vertex_mark); }

  template<typename LCC>
  CGAL::IO::Color vertex_color(const LCC& /* alcc */,
                           typename LCC::Dart_const_handle /* dh */) const
  { return CGAL::IO::Color(0, 255, 0); }

  template<typename LCC>
  bool colored_edge(const LCC& /*alcc*/, typename LCC::Dart_const_handle /*dh*/) const
  { return false; }

  template<typename LCC>
  CGAL::IO::Color edge_color(const LCC& /* alcc*/,
                         typename LCC::Dart_const_handle /* dh */) const
  { return CGAL::IO::Color(0, 0, 255); }

  template<typename LCC>
  bool colored_face(const LCC& /* alcc */,
                    typename LCC::Dart_const_handle /* dh */) const
  {return true;}

  template<typename LCC>
  CGAL::IO::Color face_color(const LCC& alcc, typename LCC::Dart_const_handle dh) const
  { return alcc.is_marked(dh, m_face_mark)?CGAL::IO::Color(255, 0, 0)
                                          :CGAL::IO::Color(211, 211, 211); }

  template<typename LCC>
  bool colored_volume(const LCC& /* alcc */,
                      typename LCC::Dart_const_handle /* dh */) const
  { return false; }

  template<typename LCC>
  bool draw_volume (const LCC& /* alcc */,
                      typename LCC::Dart_const_handle /* dh */) const
  { return false; }

  template<typename LCC>
  bool draw_face (const LCC& /* alcc */,
                      typename LCC::Dart_const_handle /* dh */) const
  { return true; }

  template<typename LCC>
  bool draw_edge (const LCC& /* alcc */,
                      typename LCC::Dart_const_handle /* dh */) const
  { return true; }

  template<typename LCC>
  bool volume_wireframe (const LCC& /* alcc */,
                      typename LCC::Dart_const_handle /* dh */) const
  { return false; }

  template<typename LCC>
  bool face_wireframe (const LCC& /* alcc */,
                      typename LCC::Dart_const_handle /* dh */) const
  { return false; }

  template<typename LCC>
  bool draw_vertex (const LCC& /* alcc */,
                      typename LCC::Dart_const_handle /* dh */) const
  { return true; }

  template<typename LCC>
  CGAL::IO::Color volume_color(const LCC& /* alcc */,
                         typename LCC::Dart_const_handle /* dh */) const {
    return CGAL::IO::Color(20, 10, 30);
  }

  bool are_edges_enabled() const { return true; }
  bool are_vertices_enabled() const { return true; }
  bool are_faces_enabled() const { return true; }

  typename ALCC::size_type m_vertex_mark, m_face_mark;
};

template<typename LCC>
void draw_facewidth(const LCC& lcc,
                    const std::vector<typename LCC::Dart_const_handle>& cycle)
{
  int nbv=0, nbf=0;
  typename LCC::size_type vertex_mark = lcc.get_new_mark();
  typename LCC::size_type face_mark = lcc.get_new_mark();
  for (std::size_t i=0; i<cycle.size(); ++i)
  {
    // Color the vertex
    if (!lcc.is_marked(cycle[i], vertex_mark))
    { lcc.template mark_cell<0>(cycle[i], vertex_mark); ++nbv; }
    // Color the face
    if (!lcc.is_marked(cycle[i], face_mark))
    { lcc.template mark_cell<2>(cycle[i], face_mark); ++nbf; }
  }

  Facewidth_draw_functor<LCC> df(vertex_mark, face_mark);
  CGAL::draw(lcc, df, "Face width");

  lcc.free_mark(vertex_mark);
  lcc.free_mark(face_mark);
}

#else  // CGAL_USE_BASIC_VIEWER

template<typename LCC>
void draw_facewidth(const LCC&,
                    const std::vector<typename LCC::Dart_const_handle>&)
{
  std::cerr<<"Impossible to draw, CGAL_USE_BASIC_VIEWER is not defined."<<std::endl;
}

#endif // CGAL_USE_BASIC_VIEWER

#endif // DRAW_FACEWIDTH_H
