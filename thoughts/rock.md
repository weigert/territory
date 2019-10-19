Proposed Pipeline:
  -> Generate Blank Chunks

  -> Generate Base Height-Map
    -> Using Tectonics

  -> Add Geological Deposits based on elemental distribution maps in 3D!
    -> Generate Perlin Maps for all Elements, then cluster to common minerals and rare stuff too!
    -> Scale the perlin maps by the relative abundance of elements on earth for instance
    -> Mineral Types Need: Brittleness, Solubility, Porosity,
    -> This is still deterministic based on the perlin map!

  -> Generate a Volcanism Map Randomly, with varying degrees of intensity in certain spots
    -> Underground temperature field comes from volcanism
    -> From this is derived a pressure field, which moves groundwater around
    -> Groundwater is empty at first, but is filled later

  -> Generate large bodies of water
    -> Randomly pick minima on the map and grow them up
    -> Sufficiently large volumes (i.e. > critical mass) are then filled
    -> If it reaches the edge, it is flood-filled upto "sea-level" with salt-water

  -> Alternatingly Perform:
    -> Weather Cycle
    -> Vegetation Cycle
    -> Erosion Cycle

  -> Weather Cycle (@ chunk-resolution):
    -> Compute Wind-Speed Map
      -> Slope of Land affects wind Speed
      -> Plant Coverage hinders winds
      -> Heavy wind dropping / rising in P / T very rapidly causes tornadoes?
    -> Compute Humidity Map
      -> Humidity is moved around by wind and diffuses
      -> Bodies of Water add humidity in dependency of temp
      -> Plants remove humidity from the air
      -> Rain removes humidity from the air in dependency of temp
      -> Snow removes humidity from the air
      -> Evaporation lowers the water level of lakes
    -> Compute Temperature Map
      -> Air temperature is moved around by wind and diffuses
      -> Sunlight adds Temperature
      -> Plants block sunlight
      -> Higher elevation has less temperature
      -> Evaporation from water sources cools the temp
      -> Temperature / sunlight will melt snow and ice
    -> Compute Clouds
      -> Temperature and Humidity give cloud-coverage
      -> Critical Values then give rain vs. snow in some areas
      -> Heavy rain causes lightning in hot areas
      -> Clouds reflect sunlight

  -> Vegation Cycle:
    -> Plants have a feature vector for their desired weather values (wind / temp / humidity / elevation / sunlight, etc.)
    -> Plants also have a vector of how they affect those values in turn (wind-blockage, sunlight blockage, soil retention)
    -> Depending on the predictor vectors in a chunk, we can sample from a distribution of plants given by distance to the plant prototypes
    -> Place the plants in appropriate locations near other plants of the same type on subsequent cycles

  -> Erosion / Sedimenation Cycle
    -> Water Erosion:
      -> Rain spawns particles that move down the map at the steepest slope
      -> Particles are eroded based on their solubility
      -> Particles have a certain mass and certain amount of soil the can carry based on mass / speed
      -> Deposited soil becomes dirt / mud
      -> Steeper Slopes / Faster Speeds means more sediment capacity
      -> Particles have inertia based on mass, which moves them along a direction
      -> When particles (nearly) stop and they still have volume, they spawn an amount of water
      -> Particles lose mass over time as they move over porous rock / soil

      How does this make rivers as blocks though?
        -> Track particle frequency of paths and the most frequent areas become rivers based on some width?
        -> Particles inside rivers lose momentum much less?
        -> Something like that (i.e. if they hit a body of water, they are propelled forward / lose inertia much less!!)

    -> Glacial Erosion:
      -> Snow deposits where it falls, lots of snow compacts to ice
      -> Ice needs to move downhill and away from peaks of course, so you get nice exposed ridges!
      -> Sunlight will melt snow and ice and spawn particles too
      -> Ice will also move downhill slowly, leading to erosion based on rock brittleness
      -> Large rock amounts will be broken off and placed into the ice
      -> As the ice melts in lower / hotter regions, these large rocks are placed at the mouth of the glacier

    -> Wind Erosion:
      -> Based on wind-speeds particles are removed and made into a fine sand.
      -> Sand and loose particles are moved around appropriately
      -> Wind carry-capacity is dependent on the speed, as it drops, particles leave the wind.

    -> Ground Water Cycle
      -> Water from the particles of sedimentation will move into porous ground and add to the water amount
      -> High pressure regions will move to low-pressure regions, based on rock porosity
      -> Temperature increases the pressure
      -> Underground pressure is given by the volcanism
      -> Temperature /pressure above a critical amount will lead to geysering / dissolution of rock
      -> Also leads to springs when water is moved to certain regions!

    -> Erosion Cycle / Water / Soil Movement
      -> Water also seeps into the ground when rain falls on flat areas that are porous
      -> This leads to a larger amount of groundwater
      -> Water underground flows to more porous areas
      -> When pressure is above a certain value, you can get geyser formation
      -> Otherwise groundwater can be moved upwards to natural springs
      -> Once lakes are overfilled, they can spill over
      -> Rivers and streams are created by tracking a drainage map over certain areas and growing it!
      -> Fast winds can move loose soil and erode the sides of rock-faces it runs past.
      -> Depending on wind-speed model will incentivize different erosion types
      -> Glaciers move downhill and actively grind the mountain, brittle rocks will be moved by the glacier
      -> Glaciers melt from sunlight and form strong rivers, they move large rocks down the mountain too


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
