#pragma once

#include "limap/_limap/helpers.h"
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

#include "limap/base/camera.h"
#include "limap/base/camera_view.h"
#include "limap/util/types.h"

namespace py = pybind11;

namespace limap {

namespace undistortion {

Camera UndistortCamera(const std::string &imname_in, const Camera &camera,
                       const std::string &imname_out);
CameraView UndistortCameraView(const std::string &imname_in,
                               const CameraView &view,
                               const std::string &imname_out);

V2D UndistortPoint(const V2D &point, const Camera &distorted_camera,
                   const Camera &undistorted_camera);
std::vector<V2D> UndistortPoints(const std::vector<V2D> &points,
                                 const Camera &distorted_camera,
                                 const Camera &undistorted_camera);

} // namespace undistortion

} // namespace limap
