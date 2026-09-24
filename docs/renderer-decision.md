# Renderer decision

## First implementation

Use Qt 6 Widgets for the workbench and implement a native Direct3D 12 renderer on Windows first. Keep a renderer interface that can later host Vulkan and an optional bgfx adapter.

This is a sequencing decision, not a claim that one API is universally best.

## What bgfx is

bgfx is a low-level, cross-platform rendering library. It is not a UI toolkit, node editor, scene graph, or application framework. It exposes a small rendering API and maps it to several graphics backends, including Direct3D, Vulkan, Metal, OpenGL, and OpenGL ES. Its shader toolchain compiles a portable shader source into backend-specific binaries.

Useful bgfx features include a render thread, command encoders, transient buffers, frame buffers, texture and buffer handles, view ordering, multiple backends, and built-in examples. That can remove a large amount of device, swap-chain, resource binding, and shader portability code from a small team.

The cost is that bgfx defines its own resource and shader model. Advanced API-specific features, debugging workflows, vendor extensions, and unusual synchronization patterns eventually need an escape hatch or a custom backend. It is also another build and packaging dependency. bgfx is a rendering abstraction, not a replacement for the application architecture.

## bgfx and commercial software

bgfx uses the permissive BSD 2-Clause license. Its official repository maintains a public “Who is using it?” list with shipped games, tools, and visualization products. Examples include Crypt of the NecroDancer, Football Manager, MAME, Nira, HARFANG Studio, Braid: Anniversary Edition, World of Goo 2, Minecraft Vibrant Visuals, and the Metal Gear Solid 4 remaster. The list is useful evidence of backend maturity, but it is not a guarantee that every bgfx feature fits a production editor.

For this project, the relevant question is whether we want to own multiple graphics backends and shader pipelines. A small team should benchmark that maintenance cost against the constraints introduced by bgfx before committing.

## Native D3D12 and Vulkan

Native APIs provide the best control and the clearest access to GPU debugging tools. D3D12 is a strong first target for a Windows-focused application: PIX, Visual Studio graphics debugging, the Windows SDK, and vendor tooling are excellent.

The maintenance cost is often underestimated. A production renderer must handle device removal, memory budgets, descriptor allocation, resource state transitions, shader reflection, pipeline caches, synchronization, resize, HDR, capture, and driver differences. Supporting D3D12 and Vulkan means implementing and testing that work twice, even when the high-level graph is shared.

The recommended path is:

1. Build the graph, resource, timing, and export contracts independently of any API.
2. Implement D3D12 first and use PIX to establish GPU diagnostics.
3. Add Vulkan only when a second platform or deployment requirement justifies it.
4. Keep bgfx as a measured alternative for portable rendering paths or a future lightweight mode.

## Practical selection rule

- Choose native D3D12 when Windows quality, PIX diagnostics, and maximum control are the top priorities.
- Choose bgfx when shipping several graphics backends quickly is more important than API-specific control.
- Choose Vulkan directly when Linux/macOS portability and explicit control are requirements from the beginning.
