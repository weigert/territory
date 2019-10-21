A number of engine improvements need to be made, and there are options I haven't explored yet.



Greedy meshing has been implemented without a bunch of fucky for-loops.



I would like asynchronous chunk loading and saving. So that can simply be done in the background, while other stuff is happening on the important threads.

Play around with better data storage formats.
I already have a homebrew octree class, maybe I can use that. It is pretty sparse.
Alternatively RLE would probably be faster to generate the model from.

- Add run-length-encoding for flat-array
- Multithread Saving / Loading / Population Updates / Rendering
- Possibly convert to octree for saving, after multithreading
- Reduce flat array size by using bytes?
- Reimplement LOD for flat-array

Renderer:
- Bloom (Lighting Blocks)
- Particles
- Texture Shadows (i.e. sprites)

- Texture Outlining and Picking of Sprites (bots and items)


- Better Input Handling for simultaneous and multi-tick inputs

**Done:**
- Generation of voxel maps based on chunk data
- Full 3D rendering of scene
- 2D Camera-Facing Sprites with animation support
- Normal Shading, Shadow Mapping and Transparency
- Full Dear ImGui Control Panel for simulation
- Renderdistance based viewing of entities and chunks
- FPS Calculator

**To-Do:**
- Drop-Shadow for sprites
- Sprite and Block Outlines (-> picking)
- Shader Effects: Fog, Bloom, Blur, Grain, Particles




and: I want to see if I can get the DOF shader to slightly make the map that we can see see-through in a visually appealing way, so that we can still see stuff behind a certain area.

This is more difficult. Maybe the camera should just be angled up more. lmao.

I'll have to see. Right now this visibility issue is a problem, but might even not be in the future. Who knows.

Maybe only if it is shaded and behind the mid-point (i.e. the depth value at wherever the camera is?)

I'll have to see what works well.
But first I need better meshing so everything is simply FASTER
and I don't have these rendering bottlenecks.

The renderer needs to be made FASTER.
Work on this!


Also still need: Bloom effects,

Remove the chunk border drawing. That looks pretty sick then.
