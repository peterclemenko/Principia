﻿#include "testing_utilities/componentwise.hpp"

#include <limits>

#include "geometry/grassmann.hpp"
#include "geometry/pair.hpp"
#include "geometry/point.hpp"
#include "geometry/r3_element.hpp"
#include "glog/logging.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "quantities/named_quantities.hpp"
#include "quantities/quantities.hpp"
#include "quantities/si.hpp"
#include "testing_utilities/almost_equals.hpp"
#include "testing_utilities/vanishes_before.hpp"

using principia::geometry::Bivector;
using principia::geometry::Pair;
using principia::geometry::Point;
using principia::geometry::R3Element;
using principia::geometry::Vector;
using principia::quantities::Action;
using principia::quantities::Winding;
using principia::quantities::Length;
using principia::si::Metre;
using testing::Eq;
using testing::Not;

namespace principia {
namespace testing_utilities {

struct World;

class ComponentwiseTest : public testing::Test {};

TEST_F(ComponentwiseTest, R3Element) {
  R3Element<double> r({1.0 + 1.0E-12, 1.0E-10, 3.5});
  EXPECT_THAT(r, Componentwise(AlmostEquals(1.0, 4504),
                               VanishesBefore(1.0, 450360),
                               Eq(3.5)));
  EXPECT_THAT(r, Componentwise(AlmostEquals(1.0, 4504),
                               VanishesBefore(1.0, 450360),
                               Not(Eq(2.5))));
  EXPECT_THAT(r, Not(Componentwise(AlmostEquals(1.0, 4504),
                                   VanishesBefore(1.0, 450360),
                                   Eq(2.5))));
}

TEST_F(ComponentwiseTest, Grassmann) {
  Vector<Length, World> v({(1.0 + 1.0E-12) * Metre,
                            1.0E-10 * Metre,
                            3.5 * Metre});
  EXPECT_THAT(v, Componentwise(AlmostEquals(1.0 * Metre, 4504),
                               VanishesBefore(1.0 * Metre, 450360),
                               Eq(3.5 * Metre)));
  Bivector<Length, World> b({(1.0 + 1.0E-12) * Metre,
                              1.0E-10 * Metre,
                              3.5 * Metre});
  EXPECT_THAT(b, Componentwise(AlmostEquals(1.0 * Metre, 4504),
                               VanishesBefore(1.0 * Metre, 450360),
                               Eq(3.5 * Metre)));
}

TEST_F(ComponentwiseTest, Pair) {
  using PV = Pair<Point<Vector<Action, World>>, Vector<Winding, World>>;
  PV pv(Point<Vector<Action, World>>(
            Vector<Action, World>({(1.0 + 1.0E-12) * SIUnit<Action>(),
                                    1.0E-10 *  SIUnit<Action>(),
                                    3.5 *  SIUnit<Action>()})),
        Vector<Winding, World>({(1.0 + 1.0E-12) * SIUnit<Winding>(),
                                (2.0 + 1.0E-10) *  SIUnit<Winding>(),
                                 3.5 *  SIUnit<Winding>()}));
  EXPECT_THAT(pv, Componentwise(
                      Componentwise(
                          AlmostEquals(1.0 * SIUnit<Action>(), 4504),
                          VanishesBefore(1.0 * SIUnit<Action>(), 450360),
                          Eq(3.5 * SIUnit<Action>())),
                      AlmostEquals(
                          Vector<Winding, World>({1.0 * SIUnit<Winding>(),
                                                  2.0 *  SIUnit<Winding>(),
                                                  3.5 *  SIUnit<Winding>()}),
                          3)));
}

}  // namespace testing_utilities
}  // namespace principia
