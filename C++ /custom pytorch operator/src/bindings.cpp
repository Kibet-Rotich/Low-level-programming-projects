#include <torch/extension.h>

// Tell C++ this function exists (forward declaration)
torch::Tensor ball_query(torch::Tensor points, torch::Tensor centers, float radius, int max_samples);

// Bind it to Python
PYBIND11_MODULE(TORCH_EXTENSION_NAME, m) {
    m.def("ball_query", &ball_query, "PointNet++ Ball Query (CPU)");
}