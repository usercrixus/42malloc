# Custom Malloc Implementation

This project implements a **custom memory allocator** (`malloc`, `free`, `realloc`) with a **pool-based design**.  
It replaces the system allocator and manages memory using multiple pools depending on allocation size.  

---

## 🔹 Core Ideas
- **Pools per size class**: small/medium allocations served from pre-allocated memory regions.  
- **Large allocations**: handled separately using `mmap` (page-rounded).  
- **Free-list stack**: fast O(1) slot allocation via `free_ids` stack.  
- **Thread-safety**: global lock for correctness, with room for per-pool/per-thread optimizations.  

This is a learning project — it focuses on clarity and measurable performance, not full production features (no guard pages, no double-free protection, no tcache).

---

## ✅ Test Results

A comprehensive suite was run against this allocator:

- **Basic allocations**: ✔️ malloc/free work as expected  
- **Edge cases**: ✔️ `malloc(0)`, `free(NULL)`, large allocations  
- **Alignment**: ✔️ all allocations properly aligned  
- **Stress & fragmentation**: ✔️ passes sequential, fragmented, and interleaved tests  
- **Realloc**: ✔️ data preserved across expansions/shrinks  
- **Concurrent threads**: ✔️ safe, no corruption, 100% success  

---

## ⚡ Performance Benchmark

| Test (100k pairs)        | Time   | Throughput (pairs/s) | Ops/s (alloc+free) |
|--------------------------|--------|----------------------|--------------------|
| Sequential (fixed size)  | 0.082s | 1.22M                | 2.44M              |
| Random size              | 0.175s | 0.57M                | 1.14M              |
| Interleaved alloc/free   | 0.224s | 0.45M                | 0.89M              |

These numbers are **within the same order of magnitude as system allocators** on a single thread.

---

## 🚧 Limitations & TODO

This allocator is functional but incomplete compared to production-grade mallocs:

- ❌ **No advanced defragmentation** (freed holes are reused but not compacted).  
- ❌ **No per-thread caches (tcache)** → global lock may cause contention under heavy multithreading.  
- ❌ **No guard pages / overrun detection** → buffer overflows won’t be caught.  
- ❌ **No coalescing of adjacent free blocks** → fragmentation may grow with long runs.  
- ❌ **No `calloc` or `posix_memalign` implementations** yet.  