#include <torch/extension.h>
#include <vector>

torch::Tensor ball_query(torch::Tensor points, torch::Tensor centers, float radius, int max_samples) {
    // Get the dimensions: N points, M centers
    int N = points.size(0);
    int M = centers.size(0);

    // Create an output PyTorch tensor filled with zeros. Shape: [M, max_samples]
    auto group_indices = torch::zeros({M, max_samples}, torch::kInt32);

    // Extract raw C++ pointers from the PyTorch tensors for maximum speed
    float* points_data = points.data_ptr<float>();
    float* centers_data = centers.data_ptr<float>();
    int* idx_data = group_indices.data_ptr<int>();

    // We use radius squared so we don't have to compute a slow square root later
    float radius_squared = radius * radius;

    // Loop 1: Go through every center point
    for (int i = 0; i < M; ++i) {
        float cx = centers_data[i * 3 + 0];
        float cy = centers_data[i * 3 + 1];
        float cz = centers_data[i * 3 + 2];

        int sample_count = 0;

        // Loop 2: Check every single point in the entire cloud against this center
        for (int j = 0; j < N; ++j) {
            float px = points_data[j * 3 + 0];
            float py = points_data[j * 3 + 1];
            float pz = points_data[j * 3 + 2];

            // Calculate squared distance (a^2 + b^2 + c^2)
            float dist_sq = (px - cx) * (px - cx) +
                            (py - cy) * (py - cy) +
                            (pz - cz) * (pz - cz);

            // If the point is inside our sphere
            if (dist_sq < radius_squared) {
                // Save the point's index (j) into our output tensor
                idx_data[i * max_samples + sample_count] = j;
                sample_count++;

                // Stop if we hit our maximum allowed points for this sphere
                if (sample_count == max_samples) {
                    break;
                }
            }
        }

        // If a sphere has some points, but didn't reach max_samples, 
        // duplicate the first found point to fill out the matrix (Standard PointNet++ trick)
        if (sample_count > 0 && sample_count < max_samples) {
            int first_idx = idx_data[i * max_samples];
            for (int j = sample_count; j < max_samples; ++j) {
                idx_data[i * max_samples + j] = first_idx;
            }
        }
    }

    return group_indices;
}