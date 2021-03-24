- Buffer Object Pooling!!! this could be major.


- Buffer Object / Chunk Pooling
- Model Combination / Merging for less draw calls
- Z-Order RLE?

- Occlusion Culling?
  - Don't draw quads / voxels which are not visible?
  - Don't mesh quads / voxels which are not visible?

- Loading and Saving of Chunks needs to be a member of chunk,
so we can defien derived classes.

Two Cases:
- Rendering Slow because of many glDraw calls, but easy to manipulate data.
- Data meshing is slow because of slicing out of one big array.
- Also slicing positions, colors and normals invalidates the indices array.

- So I can just slice out the indices, but the positions will stay.
- Is there an alternative way of doing this?

Creating individual models does not make sense.

I do an indexed render of the individual quads.



Options: Merged Instanced Model
Individual, Unmerged Models
