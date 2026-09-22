import torch
import time
import pointnet_custom

def ball_query_python_loop(points, centers, radius, max_samples):
    """
    1. Pure Python Loop
    Identical logic to the C++ version, but executed line-by-line in Python.
    """
    N = points.shape[0]
    M = centers.shape[0]
    group_indices = torch.zeros((M, max_samples), dtype=torch.int32)
    radius_squared = radius ** 2
    
    for i in range(M):
        sample_count = 0
        for j in range(N):
            # Slow tensor math for every single point inside a Python loop
            dist_sq = torch.sum((centers[i] - points[j]) ** 2)
            if dist_sq < radius_squared:
                group_indices[i, sample_count] = j
                sample_count += 1
                if sample_count == max_samples:
                    break
                    
        # Duplicate the first point to pad the array if needed
        if 0 < sample_count < max_samples:
            group_indices[i, sample_count:] = group_indices[i, 0]
            
    return group_indices

def ball_query_pytorch_vectorized(points, centers, radius, max_samples):
    """
    2. PyTorch Native Vectorized
    Uses PyTorch's underlying backend via matrix math (cdist).
    """
    # Computes a massive [M, N] distance matrix instantly
    dist_sq = torch.cdist(centers, points).pow(2)
    
    M = centers.shape[0]
    group_indices = torch.zeros((M, max_samples), dtype=torch.int32)
    
    for i in range(M):
        valid_idx = torch.nonzero(dist_sq[i] < (radius ** 2)).squeeze(-1)
        count = min(len(valid_idx), max_samples)
        
        if count > 0:
            group_indices[i, :count] = valid_idx[:count].int()
            group_indices[i, count:] = valid_idx[0].int()
            
    return group_indices

# --- Benchmarking Setup ---
# 10,000 points and 500 centers simulates a small CAD model segment
N, M = 10000, 500
radius, max_samples = 0.5, 16

points = torch.rand((N, 3), dtype=torch.float32).contiguous()
centers = torch.rand((M, 3), dtype=torch.float32).contiguous()

print(f"Benchmarking Ball Query (Points: {N}, Centers: {M})...\n")

# 1. Pure Python Loop
start = time.perf_counter()
# We only run the Python loop ONCE because it is heavily bottlenecked
out_py = ball_query_python_loop(points, centers, radius, max_samples)
py_time = time.perf_counter() - start
print(f"1. Pure Python Loop:       {py_time:.4f} seconds")

# 2. PyTorch Vectorized
start = time.perf_counter()
for _ in range(10): # Run 10 times and average for accuracy
    out_vec = ball_query_pytorch_vectorized(points, centers, radius, max_samples)
vec_time = (time.perf_counter() - start) / 10
print(f"2. PyTorch Vectorized:     {vec_time:.4f} seconds")

# 3. Custom C++ Extension
start = time.perf_counter()
for _ in range(10):
    out_cpp = pointnet_custom.ball_query(points, centers, radius, max_samples)
cpp_time = (time.perf_counter() - start) / 10
print(f"3. Custom C++ Extension:   {cpp_time:.4f} seconds")

print(f"\nResult: C++ is {py_time / cpp_time:.0f}x faster than the Python Loop!")