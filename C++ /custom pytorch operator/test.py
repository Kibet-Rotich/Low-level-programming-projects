import torch
import pointnet_custom

# 1. Create a fake point cloud (10 points, 3D coordinates)
# .contiguous() ensures the memory is lined up perfectly for C++ pointers
points = torch.rand((10, 3), dtype=torch.float32).contiguous()

# 2. Pick 2 center points to draw our spheres around
centers = torch.rand((2, 3), dtype=torch.float32).contiguous()

# 3. Define our sphere radius and how many points we want to sample
radius = 0.5
max_samples = 3

print("Running Ball Query in C++...")

# 4. Execute the C++ code directly from Python!
indices = pointnet_custom.ball_query(points, centers, radius, max_samples)

print("\nOutput Indices Matrix (Shape: 2 centers x 3 samples):")
print(indices)