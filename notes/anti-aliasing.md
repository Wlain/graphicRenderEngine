# 主流抗锯齿算法
* SSAA (Super-Sampling Anti-Aliasing):超采样抗锯齿：最简单，但是费资源，先吧图像银蛇大O缓存并且把它放大，
  在用超采样把放大后的图像进行采样，一般选取2个或者4个邻近像素。等于把一副模糊的大图，通过细腻化后再缩小成清晰的小图。
  常见的采样法有：
    * OGSS:顺序栅格超级采样：采样时选取2个邻近像素
    * RGSS:旋转栅格超级采样，采样时选取4个邻近像素
* MSAA (Multi Sampling Anti-Aliasing)：多重采样抗锯齿：只对Z-buffer和Stencil Buffer中的数据进行超采样抗锯齿的数据。
  可以简单理解为只对多边形的边缘进行抗锯齿。
* CSAA（Coverage Sampling Anti-Aliasing）：覆盖采样抗锯齿：是NVIDIA在G80及其衍生产品首次推向实用化的AA技术，
 也是目前 NVIDIA GeForce 8/9/G200 系列独享的 AA 技术。 CSAA 就是在 MSAA 基础上更进一步的节省显存使用量及带宽,单说CSAA
 就是将边缘多边形里需要取样的子像素坐标覆盖掉，把原像素坐标强制安置在硬件和驱动程序预先算好的坐标中.
* HRAA (High Resolution Anti-Aliasing)：高分辨率抗锯齿：出自NVIDIA公司
* CFAA (Custom Filter Anti-Aliasing)：可编程过滤抗锯齿:起源于AMD-ATI的R600家庭。简单地说 CFAA 就是扩大取样面积的 MSAA
  比方说之前的MSAA是严格选取物体边 缘像素进行缩放的，而CFAA则可以通过驱动和谐灵活地选择对影响锯齿效果较大的像素进行缩放，以较
  少的性能牺牲换取平滑效果。显卡资源占用也比较小
* MLAA (Morphological Anti-Aliasing)：形态抗锯齿:是AMD推出的完全基于CPU处理的抗锯齿解决方案。与MSAA不同，MLAA将跨
  越边缘像素的前景和背景色进行混合，用第2种颜色来填充该像素，从而更有效地改进图像边缘的变现效果。
* FXAA (Fast Approximate Anti-Aliasing)：快速近似抗锯齿:是传统 MSAA(多重采样抗锯 齿)效果的一种高性能近似。它是一种单程像
 素着色器，和MLAA一样运行于目标游戏渲染管线的后期处理阶段，但不像后者那样使用 DirectCompute，而只是单纯的后期处理着色器，
 不依赖于任何GPU计算API。
* TXAA (Temporal Anti-Aliasing)：时间性抗锯齿：将 MSAA、时间滤波以及后期处理相 结合，用于呈现更高的视觉保真度,
  TXAA 还能够对帧 之间的整个场景进行抖动采样，以减少闪烁情形，闪烁情形在技术上又称作时间性锯齿。
* MFAA (Multi-Frame Sampled Anti-Aliasing)：多帧采样抗锯齿:是NVIDIA公司根据MSAA改进出的一种抗锯齿技术。