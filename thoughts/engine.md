**To-Do**
Data:
- Add run-length-encoding for flat-array
- Multithread Saving / Loading / Population Updates / Rendering
- Possibly convert to octree for saving, after multithreading
- Reduce flat array size by using bytes?

Shader Effects:
- Bloom (Lighting Sources)
- Particles
- Texture Shadows (i.e. sprites)
- Texture Outlining and Picking of Sprites (bots and items)
- See-through walls or outlining of obscured objects (i.e. look into buildings / forests)

Input Handling:
- Better Input Handling for simultaneous and multi-tick inputs

Meshing and Rendering:
- Reimplement LOD for flat-array
- Change from a color VBO to a texture index VBO and texture uniform
- Add support for differently shaped blocks (i.e. separate models)
  - Grasses and Plants (i.e. crossed planes = 4 surfaces) (or simply move the planes to 0 -> 6 planes!)
  - Slopes, Steps, Slabs, Pillars, Beams
  - Small Objects (Buckets, Lamps, )
- Add support for different textures on different faces
