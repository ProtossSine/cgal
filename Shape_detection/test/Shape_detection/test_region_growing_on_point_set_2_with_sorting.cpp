// STL includes.
#include <string>
#include <vector>
#include <utility>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <cassert>

// CGAL includes.
#include <CGAL/assertions.h>
#include <CGAL/property_map.h>
#include <CGAL/Simple_cartesian.h>

#include <CGAL/Shape_detection/Region_growing/Region_growing.h>
#include <CGAL/Shape_detection/Region_growing/Region_growing_on_point_set.h>
#include <CGAL/Shape_detection/Region_growing/internal/free_functions.h>

namespace SD = CGAL::Shape_detection;
using Kernel = CGAL::Simple_cartesian<double>;

using FT       = typename Kernel::FT;
using Point_2  = typename Kernel::Point_2;
using Vector_2 = typename Kernel::Vector_2;

using Point_with_normal = std::pair<Point_2, Vector_2>;
using Input_range       = std::vector<Point_with_normal>;
using Point_map         = CGAL::First_of_pair_property_map<Point_with_normal>;
using Normal_map        = CGAL::Second_of_pair_property_map<Point_with_normal>;

using Neighbor_query = SD::Point_set::Sphere_neighbor_query<Kernel, Input_range, Point_map>;
using Line_region    = SD::Point_set::Least_squares_line_fit_region<Kernel, Input_range, Point_map, Normal_map>;
using Line_sorting   = SD::Point_set::Least_squares_line_fit_sorting<Kernel, Input_range, Neighbor_query, Point_map>;
using Circle_region  = SD::Point_set::Least_squares_circle_fit_region<Kernel, Input_range, Point_map, Normal_map>;
using Circle_sorting = SD::Point_set::Least_squares_circle_fit_sorting<Kernel, Input_range, Neighbor_query, Point_map>;

template<typename Region_type, typename Sorting>
bool test(int argc, char** argv, const std::string name, const std::size_t minr, const std::size_t maxr) {
  using Region_growing = SD::Region_growing<Input_range, Neighbor_query, Region_type, typename Sorting::Seed_map>;

  // Default parameter values.
  const std::size_t k               = 12;
  const FT          max_distance    = FT(45) / FT(10);
  const FT          max_angle       = FT(45);
  const std::size_t min_region_size = 5;

  // Load data.
  std::ifstream in(argc > 1 ? argv[1] : "data/point_set_2.xyz");
  CGAL::IO::set_ascii_mode(in);
  assert(in);

  FT a, b, c, d, e, f;
  Input_range input_range;
  while (in >> a >> b >> c >> d >> e >> f) {
    input_range.push_back(
      std::make_pair(Point_2(a, b), Vector_2(d, e)));
  }
  in.close();
  assert(input_range.size() == 3634);

  // Create parameter classes.
  Neighbor_query neighbor_query(
    input_range, CGAL::parameters::k_neighbors(k));
  Region_type region_type(
    input_range,
    CGAL::parameters::
    maximum_distance(max_distance).
    maximum_angle(max_angle).
    minimum_region_size(min_region_size));

  // Sort indices.
  Sorting sorting(
    input_range, neighbor_query,
    CGAL::parameters::all_default());
  sorting.sort();

  // Run region growing.
  Region_growing region_growing(
    input_range, neighbor_query, region_type, sorting.seed_map());

  std::vector< std::vector<std::size_t> > regions;
  region_growing.detect(std::back_inserter(regions));
  region_growing.clear();

  std::cout << "- num regions " + name + ": " << regions.size() << std::endl;
  assert(regions.size() >= minr && regions.size() <= maxr);

  // Test free functions and stability.
  for (std::size_t k = 0; k < 3; ++k) {
    regions.clear();
    SD::internal::region_growing_lines(
      input_range, std::back_inserter(regions),
      CGAL::parameters::
      maximum_distance(max_distance).
      maximum_angle(max_angle).
      minimum_region_size(min_region_size));
    assert(regions.size() == 62);
  }

  const bool success = true;
  std::cout << "rg_" + name + "_sortpoints2, sc_test_success: " << success << std::endl;
  return success;
}

int main(int argc, char *argv[]) {

  return ((
    test<Line_region, Line_sorting>(argc, argv, "lines", 62, 66) && // test lines
    test<Circle_region, Circle_sorting>(argc, argv, "circles", 62, 66) // test circles
  ) ? EXIT_SUCCESS : EXIT_FAILURE );
}
