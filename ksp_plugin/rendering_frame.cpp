#include "ksp_plugin/rendering_frame.hpp"

#include <utility>

#include "geometry/rotation.hpp"
#include "ksp_plugin/celestial.hpp"
#include "physics/transforms.hpp"
#include "quantities/quantities.hpp"

using principia::quantities::Angle;
using principia::quantities::ArcTan;
using principia::quantities::Time;
using principia::geometry::Bivector;
using principia::geometry::Displacement;
using principia::geometry::InnerProduct;
using principia::geometry::Rotation;
using principia::geometry::Wedge;
using principia::physics::BarycentricRotatingTransformingIterator;
using principia::physics::BodyCentredNonRotatingTransformingIterator;

namespace principia {
namespace ksp_plugin {

namespace {

// Returns an iterator for the first entry in |trajectory| with a time greater
// than or equal to |t|.
// TODO(phl): This is O(N), so we might want to expose a more efficient version.
// But then it's likely that we'll just rewrite this class anyway.
Trajectory<Barycentre>::NativeIterator LowerBound(
    Instant const& t, Trajectory<Barycentre> const& trajectory) {
  for (Trajectory<Barycentre>::NativeIterator it = trajectory.first();
       !it.at_end();
       ++it) {
    if (it.time() >= t) {
      return it;
    }
  }
  LOG(FATAL) << t << " not found in trajectory";
  return trajectory.first();
}

}  // namespace

BodyCentredNonRotatingFrame::BodyCentredNonRotatingFrame(
    Celestial<Barycentre> const& body) : body_(body) {}

std::unique_ptr<Trajectory<Barycentre>>
BodyCentredNonRotatingFrame::ApparentTrajectory(
    Trajectory<Barycentre> const& actual_trajectory) const {
  std::unique_ptr<Trajectory<Barycentre>> result =
      std::make_unique<Trajectory<Barycentre>>(actual_trajectory.body());
  // TODO(phl): Should tag the two frames differently.
  auto actual_it =
      BodyCentredNonRotatingTransformingIterator<Barycentre, Barycentre>(
          body_.prolongation(), &actual_trajectory);
  auto body_it = body_.prolongation().on_or_after(actual_it.time());
  for (; !actual_it.at_end(); ++actual_it, ++body_it) {
    // Advance over the bits of the actual trajectory that don't have a matching
    // time in the body trajectory.
    while (actual_it.time() != body_it.time()) {
      ++actual_it;
    }
    result->Append(actual_it.time(), actual_it.degrees_of_freedom());
  }
  return std::move(result);
}

BarycentricRotatingFrame::BarycentricRotatingFrame(
    Celestial<Barycentre> const& primary,
    Celestial<Barycentre> const& secondary)
    : primary_(primary),
      secondary_(secondary) {}

std::unique_ptr<Trajectory<Barycentre>>
BarycentricRotatingFrame::ApparentTrajectory(
    Trajectory<Barycentre> const& actual_trajectory) const {
  std::unique_ptr<Trajectory<Barycentre>> result =
      std::make_unique<Trajectory<Barycentre>>(actual_trajectory.body());
  // TODO(phl): Should tag the two frames differently.
  auto actual_it =
      BarycentricRotatingTransformingIterator<Barycentre, Barycentre>(
          primary_.prolongation(),
          secondary_.prolongation(),
          &actual_trajectory);
  auto primary_it = primary_.prolongation().on_or_after(actual_it.time());
  auto secondary_it = secondary_.prolongation().on_or_after(actual_it.time());
  for (; !actual_it.at_end(); ++actual_it, ++primary_it, ++secondary_it) {
    // Advance over the bits of the actual trajectory that don't have a matching
    // time in the trajectories.
    while (actual_it.time() != primary_it.time() ||
           actual_it.time() != secondary_it.time()) {
      ++actual_it;
    }
    result->Append(actual_it.time(), actual_it.degrees_of_freedom());
  }
  return std::move(result);
}

}  // namespace ksp_plugin
}  // namespace principia
