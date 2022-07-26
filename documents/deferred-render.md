# Rendering Path

## 正向渲染 (Forward Rendering)
应用程序阶段-> 顶点着色->图元操作->光栅化->片元着色->输出合并（片元合并） ->帧缓存
    
## 延迟着色（Deferred Rendering）：
应用程序阶段-> 顶点着色->图元操作->光栅化->片元着色 ->geometry pass through ->输出合并（片元合并）-> G-buffer -> shading着色 ->帧缓存

* 优点：
  * 复杂度仅 O(n+m)
  * 只渲染可见的像素，节省计算量
  * 用更少的 shader
  * 对后处理支持良好
  * 在大量光源的场景优势尤其明显
* 缺点：
  * 内存开销较大（带宽）
  * 读写G-buffer的内存带宽用量是性能瓶颈
  * 对透明物体的渲染存在问题。在这点上需要结合正向渲染进行渲染
  * 对多重采样抗锯齿(MultiSampling Anti-Aliasing, MSAA)的支持不友好，主要因为需开启MRT

## 延迟光照 (Light Pre-Pass / Deferred Lighting)

## 分块延迟渲染(Tile-Based Deferred Rendering):TBDR

