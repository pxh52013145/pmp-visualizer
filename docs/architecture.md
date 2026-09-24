# Visualizer architecture

The native workbench is split into four layers so UI changes do not change the
render scheduler or project model.

```text
Qt UI widgets
  Viewport / Outliner / Inspector / Timeline
              |
              v
core::WorkbenchModel -> immutable WorkbenchSnapshot
  scene placements / transforms / selection / timeline / audio analysis
              |
              +--> renderer backend (D3D12 first, Vulkan later)
              +--> IPC adapters (PMS1 spectrum stream, host commands)
```

## Extracted PMP concepts

- Scene and component placement from `modules/visualizer/scenes.ts`.
- Component transform, visibility, z ordering, selection and response scale
  from the visualizer component types.
- Workbench modes and tools from the workbench surface state.
- Timeline playhead, duration, loop and transport state from `contracts/workbench.ts`.
- Energy, bass, mid, treble, centroid, peak and beat strength from
  `AudioDataBus.ts`.
- The `PMS1` binary spectrum frame format from `audio/spectrum_stream.rs` and
  `nativeAudioSpectrumPayloadAdapter.ts`.

## Performance choices

- Audio analysis uses fixed-size storage for the initial 64-bin path. No
  per-frame vector growth occurs in the hot loop.
- Frame pacing is centralized in `FrameGovernor`; UI widgets do not schedule
  independent animation loops.
- The viewport repaints at one controlled timer cadence. The renderer layer can
  later replace that timer with a swap-chain present loop.
- Outliner structure is rebuilt only when selection, visibility, or component
  membership changes. Audio meter updates do not rebuild the tree.
- Spectrum protocol decoding validates magic, version, flags and exact payload
  length before copying data.
- UI receives a read-only snapshot rather than directly mutating service state.

## Next implementation milestones

1. Add a `RenderDevice` interface and a D3D12 device/swap-chain implementation.
2. Make the viewport a native child window owned by the renderer surface.
3. Add a graph model with undo/redo and versioned project serialization.
4. Replace demo audio generation with the authenticated local PMP stream.
5. Add a C ABI plugin SDK for audio analysis, shader nodes and exporters.
6. Add frame capture, GPU timings, device-loss recovery and offline rendering.

