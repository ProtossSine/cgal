// Copyright (c) 2018  GeometryFactory Sarl (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
//
// $URL$
// $Id$
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-Commercial
//
//
// Author(s)     : Mael Rouxel-Labbé

#ifndef CGAL_SURFACE_MESH_IO_OFF_H
#define CGAL_SURFACE_MESH_IO_OFF_H

#include <CGAL/license/Surface_mesh.h>

#include <CGAL/Surface_mesh/Surface_mesh_fwd.h>

#include <CGAL/boost/graph/IO/OFF.h>
#include <CGAL/boost/graph/Named_function_parameters.h>
#include <CGAL/IO/Color.h>
#include <CGAL/Kernel_traits.h>

#include <iostream>
#include <tuple>

#ifdef DOXYGEN_RUNNING
#define CGAL_BGL_NP_TEMPLATE_PARAMETERS NamedParameters
#define CGAL_BGL_NP_CLASS NamedParameters
#endif

namespace CGAL {

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// Read

namespace internal {
namespace IO {

template<typename PolygonMesh, typename K,
         typename NamedParameters = Named_function_parameters<bool, internal_np::all_default_t> >
class GetVertexNormalMap
{
  typedef typename PolygonMesh::template Property_map<typename PolygonMesh::Vertex_index,
                                                      typename K::Vector_3>                     DefaultMap;
  typedef DefaultMap                                                                            DefaultMap_const;

public:
  typedef typename internal_np::Lookup_named_param_def<internal_np::vertex_normal_map_t,
                                                       NamedParameters, DefaultMap>::type       type;
  typedef typename internal_np::Lookup_named_param_def<internal_np::vertex_normal_map_t,
                                                       NamedParameters, DefaultMap_const>::type const_type;
};

template<typename PolygonMesh,
         typename NamedParameters = Named_function_parameters<bool, internal_np::all_default_t> >
class GetVertexColorMap
{
  typedef typename PolygonMesh::template Property_map<typename PolygonMesh::Vertex_index,
                                                      CGAL::Color>                              DefaultMap;
  typedef DefaultMap                                                                            DefaultMap_const;
public:
  typedef typename internal_np::Lookup_named_param_def<internal_np::vertex_color_map_t,
                                                       NamedParameters, DefaultMap>::type        type;
  typedef typename internal_np::Lookup_named_param_def<internal_np::vertex_color_map_t,
                                                       NamedParameters, DefaultMap_const>::type  const_type;
};

template<typename PolygonMesh, typename K,
         typename NamedParameters = Named_function_parameters<bool, internal_np::all_default_t> >
class GetVertexTextureMap
{
  typedef typename PolygonMesh::template Property_map<typename PolygonMesh::Vertex_index,
                                                      typename K::Point_2>                      DefaultMap;
  typedef DefaultMap                                                                            DefaultMap_const;

public:
  typedef typename internal_np::Lookup_named_param_def<internal_np::vertex_texture_map_t,
                                                       NamedParameters, DefaultMap>::type       type;
  typedef typename internal_np::Lookup_named_param_def<internal_np::vertex_texture_map_t,
                                                       NamedParameters, DefaultMap_const>::type const_type;
};

template<typename PolygonMesh,
         typename NamedParameters = Named_function_parameters<bool, internal_np::all_default_t> >
class GetFaceColorMap
{
  typedef typename PolygonMesh::template Property_map<typename PolygonMesh::Face_index,
                                                      CGAL::Color>                              DefaultMap;
  typedef DefaultMap                                                                            DefaultMap_const;

public:
  typedef typename internal_np::Lookup_named_param_def<internal_np::face_color_map_t,
                                                       NamedParameters, DefaultMap>::type       type;
  typedef typename internal_np::Lookup_named_param_def<internal_np::face_color_map_t,
                                                       NamedParameters, DefaultMap_const>::type const_type;
};

template <typename Point, typename CGAL_BGL_NP_TEMPLATE_PARAMETERS>
bool read_OFF_with_or_without_fcolors(std::istream& is,
                                      Surface_mesh<Point>& sm,
                                      const CGAL::File_scanner_OFF& scanner,
                                      const CGAL_BGL_NP_CLASS& np)
{
  typedef Surface_mesh<Point>                                                             Mesh;
  typedef typename Mesh::Face_index                                                       Face_index;
  typedef CGAL::Color                                                                     Color;

  typedef typename CGAL::internal::IO::GetFaceColorMap<Mesh, CGAL_BGL_NP_CLASS>::type     FCM;

  using parameters::choose_parameter;
  using parameters::is_default_parameter;
  using parameters::get_parameter;

  typename Mesh::template Property_map<Face_index, Color> fcm;

  bool is_fcm_requested = !(is_default_parameter(get_parameter(np, internal_np::face_color_map)));
  if(!is_fcm_requested && scanner.has_colors())
  {
    bool created;
    std::tie(fcm, created) = sm.template add_property_map<Face_index, Color>("f:color", Color(0,0,0));
    CGAL_assertion(created);
    is_fcm_requested = true;
  }

  if(is_fcm_requested)
  {
    FCM fcolors = choose_parameter(get_parameter(np, internal_np::face_color_map), fcm);
    return CGAL::IO::internal::read_OFF_BGL(is, sm, np.face_color_map(fcolors));
  }
  else
  {
    return CGAL::IO::internal::read_OFF_BGL(is, sm, np);
  }
}

template <typename Point, typename CGAL_BGL_NP_TEMPLATE_PARAMETERS>
bool read_OFF_with_or_without_vtextures(std::istream& is,
                                        Surface_mesh<Point>& sm,
                                        const CGAL::File_scanner_OFF& scanner,
                                        const CGAL_BGL_NP_CLASS& np)
{
  typedef Surface_mesh<Point>                                                                Mesh;
  typedef typename Mesh::Vertex_index                                                        Vertex_index;

  typedef typename GetK<Surface_mesh<Point>, CGAL_BGL_NP_CLASS>::Kernel                      K;
  typedef typename K::Point_2                                                                Texture;
  typedef typename CGAL::internal::IO::GetVertexTextureMap<Mesh, K, CGAL_BGL_NP_CLASS>::type VTM;

  using parameters::choose_parameter;
  using parameters::is_default_parameter;
  using parameters::get_parameter;

  typename Mesh::template Property_map<Vertex_index, Texture> vtm;

  bool is_vtm_requested = !(is_default_parameter(get_parameter(np, internal_np::vertex_texture_map)));
  if(!is_vtm_requested && scanner.has_textures())
  {
    bool created;
    std::tie(vtm, created) = sm.template add_property_map<Vertex_index, Texture>("v:texcoord");
    CGAL_assertion(created);
    is_vtm_requested = true;
  }

  if(is_vtm_requested)
  {
    VTM vtextures = choose_parameter(get_parameter(np, internal_np::vertex_texture_map), vtm);
    return read_OFF_with_or_without_fcolors(is, sm, scanner, np.vertex_texture_map(vtextures));
  }
  else
  {
    return read_OFF_with_or_without_fcolors(is, sm, scanner, np);
  }
}

template <typename Point, typename CGAL_BGL_NP_TEMPLATE_PARAMETERS>
bool read_OFF_with_or_without_vcolors(std::istream& is,
                                      Surface_mesh<Point>& sm,
                                      const CGAL::File_scanner_OFF& scanner,
                                      const CGAL_BGL_NP_CLASS& np)
{
  typedef Surface_mesh<Point>                                                            Mesh;
  typedef typename Mesh::Vertex_index                                                    Vertex_index;

  typedef CGAL::Color                                                                    Color;
  typedef typename CGAL::internal::IO::GetVertexColorMap<Mesh, CGAL_BGL_NP_CLASS>::type  VCM;

  using parameters::choose_parameter;
  using parameters::is_default_parameter;
  using parameters::get_parameter;

  typename Mesh::template Property_map<Vertex_index, Color> vcm;

  bool is_vcm_requested = !(is_default_parameter(get_parameter(np, internal_np::vertex_color_map)));
  if(!is_vcm_requested && scanner.has_colors())
  {
    bool created;
    std::tie(vcm, created) = sm.template add_property_map<Vertex_index, Color>("v:color", Color(0,0,0));
    CGAL_assertion(created);
    is_vcm_requested = true;
  }

  if(is_vcm_requested)
  {
    VCM vcolors = choose_parameter(get_parameter(np, internal_np::vertex_color_map), vcm);
    return read_OFF_with_or_without_vtextures(is, sm, scanner, np.vertex_color_map(vcolors));
  }
  else
  {
    return read_OFF_with_or_without_vtextures(is, sm, scanner, np);
  }
}

template <typename Point, typename CGAL_BGL_NP_TEMPLATE_PARAMETERS>
bool read_OFF_with_or_without_vnormals(std::istream& is,
                                       Surface_mesh<Point>& sm,
                                       const CGAL::File_scanner_OFF& scanner,
                                       const CGAL_BGL_NP_CLASS& np)
{
  typedef Surface_mesh<Point>                                                                Mesh;
  typedef typename Mesh::Vertex_index                                                        Vertex_index;

  typedef typename GetK<Surface_mesh<Point>, CGAL_BGL_NP_CLASS>::Kernel                      K;
  typedef typename K::Vector_3                                                               Normal;
  typedef typename CGAL::internal::IO::GetVertexNormalMap<Mesh, K, CGAL_BGL_NP_CLASS>::type  VNM;

  using parameters::choose_parameter;
  using parameters::is_default_parameter;
  using parameters::get_parameter;

  typename Mesh::template Property_map<Vertex_index, Normal> vnm;

  bool is_vnm_requested = !(is_default_parameter(get_parameter(np, internal_np::vertex_normal_map)));
  if(!is_vnm_requested && scanner.has_normals())
  {
    bool created;
    std::tie(vnm, created) = sm.template add_property_map<Vertex_index, Normal>("v:normal");
    CGAL_assertion(created);
    is_vnm_requested = true;
  }

  if(is_vnm_requested)
  {
    VNM vnormals = choose_parameter(get_parameter(np, internal_np::vertex_normal_map), vnm);
    return read_OFF_with_or_without_vcolors(is, sm, scanner, np.vertex_normal_map(vnormals));
  }
  else
  {
    return read_OFF_with_or_without_vcolors(is, sm, scanner, np);
  }
}

} // namespace IO
} // namespace internal

/// \ingroup PkgSurfaceMeshIOFuncOFF
///
/// \brief extracts the surface mesh from an input stream in the \ref IOStreamOFF
///        and appends it to the surface mesh `sm`.
///
/// This function reads the point property as well as vertex normals, vertex and face colors,
/// and texture vertex coordinates. Those properties are stored in internal property maps
/// named "v:normal", "v:color", "f:color", and `"v:texcoord"`, respectively.
/// If property maps are passed through named parameters (see below),
/// then they are used instead of the internal ones.
///
/// Ignores comment lines which start with a hash, and lines with whitespace.
///
///\attention The graph `g` is not cleared, and the data from the stream is added.
///
/// \tparam Point The type of the \em point property of a vertex. There is no requirement on `P`,
///               besides being default constructible and assignable.
///               In typical use cases it will be a 2D or 3D point type.
/// \tparam NamedParameters a sequence of \ref bgl_namedparameters "Named Parameters"
///
/// \param is the input stream
/// \param sm the surface mesh to be constructed
/// \param np optional \ref bgl_namedparameters "Named Parameters" described below
///
/// \cgalNamedParamsBegin
///   \cgalParamNBegin{vertex_point_map}
///     \cgalParamDescription{a property map associating points to the vertices of `sm`}
///     \cgalParamType{a class model of `ReadWritePropertyMap` with `Surface_mesh::Vertex_index`
///                    as key type and `%Point_3` as value type}
///     \cgalParamDefault{`boost::get(CGAL::vertex_point, g)`}
///     \cgalParamExtra{If this parameter is omitted, an internal property map for `CGAL::vertex_point_t`
///                     must be available in `sm`.}
///   \cgalParamNEnd
///
///   \cgalParamNBegin{vertex_normal_map}
///     \cgalParamDescription{a property map associating normals to the vertices of `sm`}
///     \cgalParamType{a class model of `ReadWritePropertyMap` with `Surface_mesh::Vertex_index`
///                    as key type and `%Vector_3` as value type}
///     \cgalParamDefault{vertex normals will be output using the internal property map, if it exists}
///   \cgalParamNEnd
///
///   \cgalParamNBegin{vertex_color_map}
///     \cgalParamDescription{a property map associating colors to the vertices of `sm`}
///     \cgalParamType{a class model of `ReadWritePropertyMap` with `Surface_mesh::Vertex_index`
///                    as key type and `CGAL::Color` as value type}
///     \cgalParamDefault{vertex colors will be output using the internal property map, if it exists}
///   \cgalParamNEnd
///
///   \cgalParamNBegin{vertex_texture_map}
///     \cgalParamDescription{a property map associating textures to the vertices of `sm`}
///     \cgalParamType{a class model of `ReadWritePropertyMap` with `Surface_mesh::Vertex_index`
///                    as key type and `%Point_2` as value type}
///     \cgalParamDefault{vertex textures will be output using the internal property map, if it exists}
///   \cgalParamNEnd
///
///   \cgalParamNBegin{face_color_map}
///     \cgalParamDescription{a property map associating colors to the faces of `sm`}
///     \cgalParamType{a class model of `ReadWritePropertyMap` with `Surface_mesh::Face_index`
///                    as key type and `CGAL::Color` as value type}
///     \cgalParamDefault{face colors will be output using the internal property map, if it exists}
///   \cgalParamNEnd
/// \cgalNamedParamsEnd
///
/// \pre The data in the stream must represent a two-manifold. If this is not the case
///      the `failbit` of `is` is set and the mesh cleared.
///
template <typename Point,
          typename CGAL_BGL_NP_TEMPLATE_PARAMETERS>
bool read_OFF(std::istream& is,
              Surface_mesh<Point>& sm,
              const CGAL_BGL_NP_CLASS& np)
{
  typedef Surface_mesh<Point>                                               Mesh;

  using parameters::choose_parameter;
  using parameters::get_parameter;
  using parameters::is_default_parameter;

  typedef typename CGAL::GetVertexPointMap<Mesh, CGAL_BGL_NP_CLASS>::type   VPM;

  VPM vpm = choose_parameter(get_parameter(np, internal_np::vertex_point),
                             get_property_map(CGAL::vertex_point, sm));

  std::streampos pos = is.tellg();
  CGAL::File_scanner_OFF scanner(is, false);
  is.seekg(pos);

  bool res = internal::IO::read_OFF_with_or_without_vnormals(is, sm, scanner, np);
  if(!res)
    sm.clear();

  return res;
}

#ifndef CGAL_NO_DEPRECATED_CODE

/*!
  \ingroup PkgSurfaceMeshIOFuncDeprecated
  \deprecated This function is deprecated since \cgal 5.2, `CGAL::read_OFF(std::istream&, const Surface_mesh<Point>&)` should be used instead.
*/
template <typename Point, typename CGAL_BGL_NP_TEMPLATE_PARAMETERS>
CGAL_DEPRECATED bool read_off(std::istream& is, Surface_mesh<Point>& sm, const CGAL_BGL_NP_CLASS& np)
{
  return read_OFF(is, sm, np);
}

/*!
  \ingroup PkgSurfaceMeshIOFuncDeprecated
  \deprecated This function is deprecated since \cgal 5.2, `CGAL::read_OFF(std::istream&, const Surface_mesh<Point>&)` should be used instead.
*/
template <typename Point>
CGAL_DEPRECATED bool read_off(std::istream& is, Surface_mesh<Point>& sm)
{
  return read_OFF(is, sm, parameters::all_default());
}

/*!
  \ingroup PkgSurfaceMeshIOFuncDeprecated
  \deprecated This function is deprecated since \cgal 5.2, `CGAL::read_OFF(std::istream&, const Surface_mesh<Point>&)` should be used instead.
*/
template <typename Point>
CGAL_DEPRECATED bool read_off(Surface_mesh<Point>& sm, const std::string& filename)
{
  return read_OFF(filename, sm, parameters::all_default());
}

#endif // CGAL_NO_DEPRECATED_CODE

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// Write

namespace IO {
namespace internal {

template <typename Point, typename CGAL_BGL_NP_TEMPLATE_PARAMETERS>
bool write_OFF_with_or_without_fcolors(std::ostream& os,
                                       const Surface_mesh<Point>& sm,
                                       const CGAL_BGL_NP_CLASS& np)
{
  typedef Surface_mesh<Point>                                            Mesh;
  typedef typename Mesh::Face_index                                      Face_index;
  typedef CGAL::Color                                                    Color;

  using parameters::choose_parameter;
  using parameters::is_default_parameter;
  using parameters::get_parameter;

  const bool has_fcolors = !(is_default_parameter(get_parameter(np, internal_np::face_color_map)));

  typename Mesh::template Property_map<Face_index, Color> fcolors;
  bool has_internal_fcolors;
  std::tie(fcolors, has_internal_fcolors) = sm.template property_map<Face_index, CGAL::Color>("f:color");

  if(!has_fcolors && has_internal_fcolors && !std::distance(fcolors.begin(), fcolors.end()) > 0)
    return write_OFF_BGL(os, sm, np.face_color_map(fcolors));
  else
    return write_OFF_BGL(os, sm, np);
}

template <typename Point, typename CGAL_BGL_NP_TEMPLATE_PARAMETERS>
bool write_OFF_with_or_without_vtextures(std::ostream& os,
                                         const Surface_mesh<Point>& sm,
                                         const CGAL_BGL_NP_CLASS& np)
{
  typedef Surface_mesh<Point>                                            Mesh;
  typedef typename Mesh::Vertex_index                                    Vertex_index;

  typedef typename GetK<Surface_mesh<Point>, CGAL_BGL_NP_CLASS>::Kernel  K;
  typedef typename K::Point_2                                            Texture;

  using parameters::choose_parameter;
  using parameters::is_default_parameter;
  using parameters::get_parameter;

  const bool has_vtextures = !(is_default_parameter(get_parameter(np, internal_np::vertex_texture_map)));

  typename Mesh::template Property_map<Vertex_index, Texture> vtextures;
  bool has_internal_vtextures;
  std::tie(vtextures, has_internal_vtextures) = sm.template property_map<Vertex_index, Texture>("v:texcoord");

  if(!has_vtextures && has_internal_vtextures && !std::distance(vtextures.begin(), vtextures.end()) > 0)
    return write_OFF_with_or_without_fcolors(os, sm, np.vertex_texture_map(vtextures));
  else
    return write_OFF_with_or_without_fcolors(os, sm, np);
}

template <typename Point, typename CGAL_BGL_NP_TEMPLATE_PARAMETERS>
bool write_OFF_with_or_without_vcolors(std::ostream& os,
                                       const Surface_mesh<Point>& sm,
                                       const CGAL_BGL_NP_CLASS& np)
{
  typedef Surface_mesh<Point>                                            Mesh;
  typedef typename Mesh::Vertex_index                                    Vertex_index;
  typedef CGAL::Color                                                    Color;

  using parameters::choose_parameter;
  using parameters::is_default_parameter;
  using parameters::get_parameter;

  const bool has_vcolors = !(is_default_parameter(get_parameter(np, internal_np::vertex_color_map)));

  typename Mesh::template Property_map<Vertex_index, Color> vcolors;
  bool has_internal_vcolors;
  std::tie(vcolors, has_internal_vcolors) = sm.template property_map<Vertex_index, CGAL::Color>("v:color");

  if(!has_vcolors && has_internal_vcolors && !std::distance(vcolors.begin(), vcolors.end()) > 0)
    return write_OFF_with_or_without_vtextures(os, sm, np.vertex_color_map(vcolors));
  else
    return write_OFF_with_or_without_vtextures(os, sm, np);
}

template <typename Point, typename CGAL_BGL_NP_TEMPLATE_PARAMETERS>
bool write_OFF_with_or_without_vnormals(std::ostream& os,
                                        const Surface_mesh<Point>& sm,
                                        const CGAL_BGL_NP_CLASS& np)
{
  typedef Surface_mesh<Point>                                            Mesh;
  typedef typename Mesh::Vertex_index                                    Vertex_index;

  typedef typename GetK<Surface_mesh<Point>, CGAL_BGL_NP_CLASS>::Kernel  K;
  typedef typename K::Vector_3                                           Normal;

  using parameters::choose_parameter;
  using parameters::is_default_parameter;
  using parameters::get_parameter;

  const bool has_vnormals = !(is_default_parameter(get_parameter(np, internal_np::vertex_normal_map)));

  typename Mesh::template Property_map<Vertex_index, Normal> vnormals;
  bool has_internal_vnormals;
  std::tie(vnormals, has_internal_vnormals) = sm.template property_map<Vertex_index, Normal>("v:normal");

  if(!has_vnormals && has_internal_vnormals && !std::distance(vnormals.begin(), vnormals.end()) > 0)
    return write_OFF_with_or_without_vcolors(os, sm, np.vertex_normal_map(vnormals));
  else
    return write_OFF_with_or_without_vcolors(os, sm, np);
}

} // namespace internal
} // namespace IO

/// \ingroup PkgSurfaceMeshIOFuncOFF
///
/// \brief writes the surface mesh `sm` in the output stream, using the \ref IOStreamOFF.
///
/// This overload of \link PkgBGLIOFct `write_OFF(std::ostream&, const Graph&)` \endlink will also output
/// the following property maps internal to the surface mesh, if they exist and if they are not
/// already present in the named parameters:
///
/// - vertex:normal map (property map named "v:normal" in the surface mesh)
/// - vertex:color map (property map named "v:color" in the surface mesh)
/// - vertex:texture map (property map named "v:texcoord" in the surface mesh)
/// - face:color map (property map named "f:color" in the surface mesh)
///
/// \tparam Point The type of the \em point property of a vertex. There is no requirement on `P`,
///               besides being default constructible and assignable.
///               In typical use cases it will be a 2D or 3D point type.
/// \tparam NamedParameters a sequence of \ref bgl_namedparameters "Named Parameters"
///
/// \param os the output stream
/// \param sm the surface mesh to be output
/// \param np optional \ref bgl_namedparameters "Named Parameters" described below
///
/// \cgalNamedParamsBegin
///   \cgalParamNBegin{vertex_point_map}
///     \cgalParamDescription{a property map associating points to the vertices of `sm`}
///     \cgalParamType{a class model of `ReadablePropertyMap` with `Surface_mesh::Vertex_index`
///                    as key type and `%Point_3` as value type}
///     \cgalParamDefault{`boost::get(CGAL::vertex_point, g)`}
///     \cgalParamExtra{If this parameter is omitted, an internal property map for `CGAL::vertex_point_t`
///                     must be available in `sm`.}
///   \cgalParamNEnd
///
///   \cgalParamNBegin{vertex_normal_map}
///     \cgalParamDescription{a property map associating normals to the vertices of `sm`}
///     \cgalParamType{a class model of `ReadablePropertyMap` with `Surface_mesh::Vertex_index`
///                    as key type and `%Vector_3` as value type}
///     \cgalParamDefault{vertex normals will be output using the internal property map, if it exists}
///   \cgalParamNEnd
///
///   \cgalParamNBegin{vertex_color_map}
///     \cgalParamDescription{a property map associating colors to the vertices of `sm`}
///     \cgalParamType{a class model of `ReadablePropertyMap` with `Surface_mesh::Vertex_index`
///                    as key type and `CGAL::Color` as value type}
///     \cgalParamDefault{vertex colors will be output using the internal property map, if it exists}
///   \cgalParamNEnd
///
///   \cgalParamNBegin{vertex_texture_map}
///     \cgalParamDescription{a property map associating textures to the vertices of `sm`}
///     \cgalParamType{a class model of `ReadablePropertyMap` with `Surface_mesh::Vertex_index`
///                    as key type and `%Point_2` as value type}
///     \cgalParamDefault{vertex textures will be output using the internal property map, if it exists}
///   \cgalParamNEnd
///
///   \cgalParamNBegin{face_color_map}
///     \cgalParamDescription{a property map associating colors to the faces of `sm`}
///     \cgalParamType{a class model of `ReadablePropertyMap` with `Surface_mesh::Face_index`
///                    as key type and `CGAL::Color` as value type}
///     \cgalParamDefault{face colors will be output using the internal property map, if it exists}
///   \cgalParamNEnd
///
///   \cgalParamNBegin{stream_precision}
///     \cgalParamDescription{a parameter used to set the precision (i.e. how many digits are generated) of the output stream}
///     \cgalParamType{int}
///     \cgalParamDefault{`6`}
///   \cgalParamNEnd
/// \cgalNamedParamsEnd
///
/// \returns `true` if writing was successful, `false` otherwise.
///
template <typename Point, typename CGAL_BGL_NP_TEMPLATE_PARAMETERS>
bool write_OFF(std::ostream& os,
               const Surface_mesh<Point>& sm,
               const CGAL_BGL_NP_CLASS& np)
{
  using parameters::choose_parameter;
  using parameters::is_default_parameter;
  using parameters::get_parameter;

  const bool has_vpoints = !(is_default_parameter(get_parameter(np, internal_np::vertex_point)));
  if(has_vpoints)
    return IO::internal::write_OFF_with_or_without_vnormals(os, sm, np);

  return IO::internal::write_OFF_with_or_without_vnormals(os, sm, np.vertex_point_map(get_const_property_map(CGAL::vertex_point, sm)));
}

#ifndef CGAL_NO_DEPRECATED_CODE

/*!
  \ingroup PkgSurfaceMeshIOFuncDeprecated
  \deprecated This function is deprecated since \cgal 5.2, `CGAL::write_OFF(std::ostream&, const Surface_mesh<Point>&)` should be used instead.
*/
template <typename Point, typename CGAL_BGL_NP_TEMPLATE_PARAMETERS>
CGAL_DEPRECATED bool write_off(std::ostream& os, const Surface_mesh<Point>& sm, const CGAL_BGL_NP_CLASS& np)
{
  return write_OFF(os, sm, np);
}

/*!
  \ingroup PkgSurfaceMeshIOFuncDeprecated
  \deprecated This function is deprecated since \cgal 5.2, `CGAL::write_OFF(std::ostream&, const Surface_mesh<Point>&)` should be used instead.
*/
template <typename Point>
CGAL_DEPRECATED bool write_off(std::ostream& os, const Surface_mesh<Point>& sm)
{
  return write_OFF(os, sm, parameters::all_default());
}

/*!
  \ingroup PkgSurfaceMeshIOFuncDeprecated
  \deprecated This function is deprecated since \cgal 5.2, `CGAL::write_OFF(std::ostream&, const Surface_mesh<Point>&)` should be used instead.
*/
template <typename Point>
CGAL_DEPRECATED bool write_off(const Surface_mesh<Point>& sm, const std::string& filename)
{
  return write_OFF(filename, sm, parameters::all_default());
}

#endif // CGAL_NO_DEPRECATED_CODE

} // namespace CGAL

#endif // CGAL_SURFACE_MESH_IO_OFF_H
