#World Generator V2

I want three main cycles that operate on different time-scales to generate my world. They also each have a number of general "large scale effects" whose properties I want to capture.

-> Geological Cycle
  -> Tectonics, Volcanics
-> Climate Cycle
  -> Weather, Water Movement, Erosion
-> Vegetation Cycle
  -> Plant Growth, Soil / Water Retention

These work together to generate desired world shapes.

1. Use Geology to Generate Height Map
2. Generate Climate Maps from it
3. Generate Vegetation Maps

## Geological Cycle

-> Do an erosion cycle using the downfall map somehow
-> for example, just know where rivers pour down and carve there as well as move sediment

- Maps: Height, Humidity, Temperature, Volcanism, Drainage, Geology
- Water: Rivers / Streams / (Hot) Springs / Deltas / Swamps / Islands / Waterfalls
- Biomes: Deserts, Forests, Rainforests, Plains, etc.
- Dynamic Erosion and Flooding
- Dynamic Weather System at chunk resolution(Clouds, Rain, Snow, Hail, Temp, Wind, Lightning, Sunlight)
- Vegetation for soil / water retention, enrichment, climate dependency, cooling
- Dynamic Range of Vegetation, where each plant is a point in some space, then sample near where the state is find nearest plant, place it

## Geological Cycle
My approach is to take a number of general properties about how plate tectonics works, wrap it into a time-dependent differential equation system on a grid and let it evolve in time.

**Basic Principles:**
1. Crust of Earth Consists of Distinct Plates, floating on the Mantle Magma
2. Convective forces in the mantle magma lead to plate movement
  - Convective forces are dictates by differences in temperature / pressure in magma.
3. Plates meet at "faults" and can have varying relative movement.
  - Convergent: Plates move towards each other, leading to "subduction".
  - Divergent: Leads to rifts and rift valleys
  - Transform: The move past each other
4. Plates have varying densities and thicknesses
  - Ocean plates are more dense, more thin
  - Land plates are less dense, more thick
5. More dense plates will be forced under another plate, and are forced under more easily than less dense plates
6. Melting and Cooling of Magma will change the magma density and add / remove crust material
  - Melting material increases volcanism / decreases density
  - Cooling material decreases volcanism / increases density
7. Some spots are hotter than others, with higher "volcanic activity" (hot spots).
8. Very high volcanism and thin crust can lead to eruptions and the like..

**Approach**
Volcanism Map:
1. Generate an initial "Volcanism" map from a 2D perlin noise field.
2. Derive a flow-field from this volcanism maps gradient in 2D.

1. Generate voronoi-cell grid for plates, with plate density given by cell value
2. Seed ocean plates below some density threshold, and land plates above some density threshold.
3. Give them an initial "height"

1. Pass over the voronoi cells to compute movement for each plate.
2. At the places where cells overlap, compute a probable change to the landscape using some function.
3. Add Noise

1. Where Material is added, we alter the volcanism map with feedback
2. Where material is removed, we do the same thing

3. This affects the height map overall

**Simulation Assumptions**
1. Plates float at the same "level" on the mantle
2. Density is given by a voronoi cell system
3. Temperature and Pressure System is summarized by a density map ("volcanism")
  - Convective forces act from low-density (heating / high volcanism) to high-density (cooling / low volcanism) regions
4. Convective forces are generated via a volcanism gradient
5. The surface height is controlled by the addition and removal of material from the crust through the volcanism.












Missing Aspects:

To place specific types of minerals, my impression is that petrology is very complex and not easily modelled by a "reduced system".

Rock types are classified by:
- Igneous
- Sedimentary
- Metamorphic

In certain regions, igneous rock will be expelled
Erosion will lead to addition of sedimentary rock
Metamorphic rock will be generated at specific volcanism zones





Required Features:
-> Tectonics
  -> Mountains, Canyons, Valleys,
-> Volcanism
  -> Volcanoes, Springs, Geysers,
-> Groundwater
  -> Caves, Cenotes, Underground Rivers, Drainage, Temperature and Humidity
-> Geology
  -> Rock Types, Boulders and Rocks, Mineral and Metal deposits
-> Erosion
  -> Glaciers, Rain, Wind, Sedimentation and Transport, Boulder Movement,
-> Flooding
  ->






  We also need groundwater effects.
  We also need volcanism effects.
  Possibly underground temperature and pressure.



## Climate Cycle



Climate Cycle:
-> Temperature, Humidity, Downfall, Wind, Sun generation
-> Also responsible for generating downfall and wind maps for erosion purposes
-> Also can generate maps of

Vegetation Cycle:
-> Killing and Birthing Plants of various types at appropriate positions


## Vegetation Cycle

#Rock Cycle

Three Main Rock Types:
- Sedimentary (From Layering and Compression)
- Metamorphic (From heat reformed sedimentary)
- Igneous (From Cooled Magma)
  - Extrusive (magma outside of earth)
  - Intrusive (magma inside earth)

Weathering and Erosion always leads to sediment and that becomes sedimented.
Heat / Pressure leads to changing to Metamorphic Rocks
Melting leads to magma, solidification to igneous rock

Igneous can move to metamorphic too.

Magma under high pressure will move up and cause volcanism.

Rivers and wind transport the sediment somehow.

Sedimentation lots under oceans.

Does this need to be coupled to the water cycle?

Not yet I think. I think water cycle / drainage can be computed on a separate cycle.
