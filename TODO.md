- Buffer Object Pooling!!! this could be major.


- Buffer Object Pooling
- Chunk Pooling

- Loading and Saving of Chunks needs to be a member of chunk,
so we can defien derived classes.

- Meshing works well - I need better rendering with less driver overhead
  - Quad pooling?
  - Model Merging?
  - glMultiDrawElements?

- Compression and LOD
  - Z-Order RLE?

- Occlusion Culling?
  - Don't draw quads / voxels which are not visible?
  - Don't mesh quads / voxels which are not visible?






Options:
- Indexed Multi Model, This is slow because of multiple VAO switches.
- Rendering a single model is the fastest, but constructing is expensive and costs lots of memory.
- I need a way to slice out chunk data from the model dynamically and quickly.



Two Cases:
- Rendering Slow because of many glDraw calls, but easy to manipulate data.
- Data meshing is slow because of slicing out of one big array.
- Also slicing positions, colors and normals invalidates the indices array.

- So I can just slice out the indices, but the positions will stay.
- Is there an alternative way of doing this?

Slicing indices out of a big array is very slow.

Creating individual models does not make sense.

I do an indexed render of the individual quads.



Options: Merged Instanced Model
Individual, Unmerged Models


There is a lot of driver overhead currently for how the individual chunks are rendered.
This needs to intelligently bundled somehow. The question is how.

Vertex pool which is overwritten somehow?
Or like a quad-pool model?



# Quad-Pooling:

Create a single big buffer which contains the quad information. The format is always the same.

Every chunk just needs to track pointers to its own quads.

when the model is deleted, it needs to remove its quads from the quad pool.

When a new quad is created by a model, it can just request a quad and then it will thus be rendered.
