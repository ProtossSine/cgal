#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/Delaunay_triangulation_sphere_traits_2.h>
#include <CGAL/Projection_sphere_traits_3.h>
#include <CGAL/Triangulation_on_sphere_2.h>
#include <CGAL/Delaunay_triangulation_on_sphere_2.h>

#include <algorithm>
#include <iostream>
#include <vector>

typedef CGAL::Exact_predicates_inexact_constructions_kernel         K;
typedef K::Point_3                                                  Point_3;

typedef CGAL::Delaunay_triangulation_sphere_traits_2<K>             Gt;
typedef CGAL::Delaunay_triangulation_on_sphere_2<Gt>                DTOS;

typedef CGAL::Projection_sphere_traits_3<K>                         PGt;
typedef CGAL::Delaunay_triangulation_on_sphere_2<PGt>               PDTOS;

template <class Vertex_handle, class Face_handle>
bool has_face(const Face_handle fh,
              const Vertex_handle v0,
              const Vertex_handle v1,
              const Vertex_handle v2)
{
  bool test1, test2, test3;

  for(int i=0; i<=2; ++i)
  {
    test1 = (v0->point() == fh->vertex(i)->point());
    if(test1)
      break;
  }

  if(!test1)
    return false;

  for(int i=0; i<=2; ++i)
  {
    test2 = (v1->point() == fh->vertex(i)->point());
    if(test2)
      break;
  }

  if(!test2)
    return false;

  for(int i=0; i<=2; ++i)
  {
    test3 = (v2->point() == fh->vertex(i)->point());
    if(test3)
      break;
  }

  if(!test3)
    return false;

  return true;
}

template <class Triangul>
bool are_equal(const Triangul& triA, const Triangul& triB)
{
  typedef typename Triangul::Vertex_handle                  Vertex_handle;
  typedef typename Triangul::Face_handle                    Face_handle;
  typedef typename Triangul::All_faces_iterator             Face_iterator;

  std::cout << "First triangulation:" << std::endl;
  std::cout << "dimension: " << triA.dimension() << std::endl;
  std::cout << triA.number_of_vertices() << " nv" << std::endl;
  std::cout << triA.number_of_faces() << " nf" << std::endl;
  std::cout << triA.number_of_ghost_faces() << " gf" << std::endl;

  std::cout << "Second triangulation:" << std::endl;
  std::cout << "dimension: " << triB.dimension() << std::endl;
  std::cout << triB.number_of_vertices() << " nv" << std::endl;
  std::cout << triB.number_of_faces() << " nf" << std::endl;
  std::cout << triB.number_of_ghost_faces() << " gf" << std::endl;

  if (triA.number_of_vertices()!= triB.number_of_vertices())
    return false;
  if (triA.number_of_faces()!= triB.number_of_faces())
    return false;
  if(triA.number_of_ghost_faces()!=triB.number_of_ghost_faces())
    return false;

  bool found = true;
  for(Face_iterator fiA = triA.all_faces_begin(); fiA != triA.all_faces_end(); ++fiA)
  {
    found = false;
    for(Face_iterator fiB=triB.all_faces_begin(); fiB!=triB.all_faces_end(); ++fiB)
    {
      Face_handle fb = Face_handle(fiB);
      Face_handle fa = Face_handle(fiA);
      Vertex_handle v0 = fa->vertex(0);
      Vertex_handle v1 = fa->vertex(1);
      Vertex_handle v2 = fa->vertex(2);
      if(has_face(fb, v0, v1, v2))
      {
        found = true;
        break;
      }
    }

    assert(found);
  }

  return found;
}

// tests whether it is possible to insert points in degenerated positions
// and whether the result is uniquely defined after this.

template <typename DTOS, typename PointContainer>
void test(const double radius,
          PointContainer coplanar_points)
{
  DTOS dtos(CGAL::ORIGIN, radius);
  for(const auto& p : coplanar_points) // to avoid Hilbert sorts
  {
    std::cout << "------------> IIIIIIIIIIIIIIIIIIINSERT " << p << std::endl;
    dtos.insert(p);
  }

  assert(dtos.is_valid());

  std::cout << "Triangulation:" << std::endl;
  std::cout << "dimension: " << dtos.dimension() << std::endl;
  std::cout << dtos.number_of_vertices() << " nv" << std::endl;
  std::cout << dtos.number_of_edges() << " ne" << std::endl;
  std::cout << dtos.number_of_faces() << " nf" << std::endl;
  std::cout << dtos.number_of_ghost_faces() << " gf" << std::endl;

  return; // @tmp

  for(int i=0; i<10; ++i)
  {
    CGAL::cpp98::random_shuffle(coplanar_points.begin(), coplanar_points.end());

    DTOS dtos2(CGAL::ORIGIN, radius);
    for(const auto& p : coplanar_points) // to avoid Hilbert sorts
      dtos2.insert(p);

    assert(dtos2.is_valid());
    assert(are_equal(dtos, dtos2));

    std::cout << dtos2.number_of_vertices() << " nv and " << dtos2.number_of_faces() << " nf" << std::endl;
  }
}

int main(int, char**)
{
  using std::sqrt;

  const double radius = 100;
  const double radius2 = CGAL::square(radius);

  std::vector<Point_3> coplanar_low_dim { Point_3(0,0,radius),  Point_3(radius,0,0),  Point_3(0,radius,0)
                                    //  , Point_3(0,0,-radius)
                                        };

  // Points are coplanar and coplanar with the center of the sphere
  std::vector<Point_3> coplanar_points { Point_3(  radius/sqrt(2),   radius/sqrt(2),      0),
                                         Point_3(- radius/sqrt(2),   radius/sqrt(2),      0),
                                         Point_3(- radius/sqrt(2), - radius/sqrt(2),      0),
                                         Point_3(  radius/sqrt(2), - radius/sqrt(2),      0),
                                         Point_3(          radius,                0,      0),
                                         Point_3(               0,                0, radius) };

  std::vector<Point_3> coplanar_points_on_great_circle { Point_3(         0,          0,            radius),
                                                         Point_3( 1/sqrt(2),  1/sqrt(2), sqrt(radius2 - 1)),
                                                         Point_3(-1/sqrt(2), -1/sqrt(2), sqrt(radius2 - 1)),
                                                         Point_3(         0,          1, sqrt(radius2 - 1)),
                                                         Point_3(         1,          0, sqrt(radius2 - 1)),
                                                         Point_3(-1/sqrt(2),  1/sqrt(2), sqrt(radius2 - 1)),
                                                         Point_3( 1/sqrt(2), -1/sqrt(2), sqrt(radius2 - 1)),
                                                         Point_3(    radius,         0 ,               0) };

  std::vector<Point_3> coplanar_points_on_circle { Point_3( 1/sqrt(2),  1/sqrt(2), sqrt(radius2 - 1)),
                                                   Point_3(-1/sqrt(2), -1/sqrt(2), sqrt(radius2 - 1)),
                                                   Point_3(         0,          1, sqrt(radius2 - 1)),
                                                   Point_3(         1,          0, sqrt(radius2 - 1))/*,
                                                   Point_3(-1/sqrt(2),  1/sqrt(2), sqrt(radius2 - 1)),
                                                   Point_3( 1/sqrt(2), -1/sqrt(2), sqrt(radius2 - 1))*/ };

  // -----------------------------------------------------------------------------------------------
  std::cout << "Testing with Delaunay_triangulation_sphere_traits:" << std::endl;
  test<DTOS>(radius, coplanar_low_dim);
  test<DTOS>(radius, coplanar_points);
  test<DTOS>(radius, coplanar_points_on_great_circle);
  test<DTOS>(radius, coplanar_points_on_circle);

  // -----------------------------------------------------------------------------------------------
  std::cout << "Testing with Projection_sphere_traits:  " << std::endl;
  PGt traits(CGAL::ORIGIN, radius);
  PGt::Construct_point_on_sphere_2 to_s2 = traits.construct_point_on_sphere_2_object();

  std::vector<PGt::Point_on_sphere_2> coplanar_ppoints;
  std::vector<PGt::Point_on_sphere_2> coplanar_ppoints_on_great_circle;

  std::transform(coplanar_points.begin(), coplanar_points.end(), std::back_inserter(coplanar_ppoints), to_s2);

  test<PDTOS>(radius, coplanar_ppoints);

  // tests the convenience API (passing P3)
  test<PDTOS>(radius, coplanar_low_dim);
  test<PDTOS>(radius, coplanar_points_on_circle);
  test<PDTOS>(radius, coplanar_points_on_great_circle);
  test<PDTOS>(radius, coplanar_points_on_circle);

  std::cout << "Done" << std::endl;

  return EXIT_SUCCESS;
}
