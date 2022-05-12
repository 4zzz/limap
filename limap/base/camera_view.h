#ifndef LIMAP_BASE_IMAGE_H
#define LIMAP_BASE_IMAGE_H

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>
#include <cmath>
#include <fstream>

namespace py = pybind11;

#include "util/types.h"
#include "_limap/helpers.h"

#include "base/camera.h"

namespace limap {

class CameraImage {
public:
    CameraImage() {}
    CameraImage(const int& input_cam_id, const CameraPose& input_pose, const std::string& image_name = "none"): cam_id(input_cam_id), pose(input_pose), image_name_(image_name) {}
    CameraImage(const Camera& input_cam, const CameraPose& input_pose, const std::string& image_name = "none"): cam_id(input_cam.CameraId()), pose(input_pose), image_name_(image_name) {}
    CameraImage(py::dict dict);

    int cam_id;
    CameraPose pose;
    
    py::dict as_dict() const;
    M3D R() const { return pose.R(); }
    V3D T() const { return pose.T(); }

    void SetCameraId(const int input_cam_id) { cam_id = input_cam_id; }
    void SetImageName(const std::string& image_name) { image_name_ = image_name; }
    std::string image_name() const { return image_name_; }

private:
    std::string image_name_;
};

class CameraView: public CameraImage {
public:
    CameraView() {}
    CameraView(const Camera& input_cam, const CameraPose& input_pose, const std::string& image_name = "none"): CameraImage(input_cam, input_pose, image_name), cam(input_cam) {}
    CameraView(py::dict dict);

    Camera cam;
    py::array_t<uint8_t> read_image(const bool set_gray) const;
    
    py::dict as_dict() const;
    M3D K() const { return cam.K(); }
    M3D K_inv() const { return cam.K_inv(); }
    int h() const { return cam.h(); }
    int w() const { return cam.w(); }

    V2D projection(const V3D& p3d) const;
    V3D ray_direction(const V2D& p2d) const;
};

// used for optimization
class MinimalPinholeCamera {
public:
    MinimalPinholeCamera() {}
    MinimalPinholeCamera(const CameraView& view);
    CameraView GetCameraView() const;

    V4D kvec; // [f1, f2, c1, c2]
    V4D qvec;
    V3D tvec;
    int height, width;
};

// interchanging between CameraView and MinimalPinholeCamera
MinimalPinholeCamera cam2minimalcam(const CameraView& view);

CameraView minimalcam2cam(const MinimalPinholeCamera& camera);

} // namespace limap

#endif

