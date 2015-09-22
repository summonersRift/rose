Kernel from LoopTrees
=====================

This module generates SPMD kernels from nested loops.
These kernels are generated to be executed with different SPMD programming models.
KLT's runtime enables to execute the kernels.
Sequential versions of the kernel can be executed on the host.
Parallel kernels are generated for:
 - a pThread runtime
 - OpenCL
 - CUDA
In addition to executing the kernels, KLT's runtime take care of data movement.

