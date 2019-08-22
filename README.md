# territory
3D rendered proc-gen world test


/*
ToDo:
- Ditch the Octree
	-> World Should Contain Chunks ("Pages") in an Unordered Map!!!!\
	-> Virtual Memory just means I load and save chunks into a fixed subset like I'm doing now, basically not having all loaded at once / only a fixed amount at once.
	-> Constant Access Time for All Chunks in the thing
	-> Chunks then consist of Run-Length-Encoded Interval Trees??
	-> Or a more efficient Octree???
	-> Not Sure!!!!

When Storing a chunk inside an array, you still need to iterate over everything to get the desired effects (meshing, etc.) but random access to elements is very fast (for pathfinding). I need fast random access.
Interval tree sounds like it makes sense because of log(n) access.
Seems to make sense!

This should hopefully speed everything up, including drawing and stuff due to getting rid of the Octree restriction.
I could try run-length-encoding for now.

Apparently, std::vectors are slow.
Also, maybe we can avoid using std::sort.

But once we have done these other speed improvements, we can maybe look at some benchmarking and see where we can improve. I definitely want LARGE worldsizes with a LARGE renderdistance. This is important for playability! Right now the bottlenecks are TOO NOTICEABLE.

Replace binary logarithms with bit shifts. Totally unnecessary to use math.
Maybe we can eliminate math entirely.

Subsequently, I won't have LOD functions anymore, but I will be much faster.

Finally, I need to multithread my saving and loading. Right now these basic performance issues are a major bottleneck, particularly for path-finding and stuff, where random access is incredibly important.

Hash Map / Unordered Map is the only "Constant Access Time" Data Structure. That's why we should really be using it for storing our chunks inside. Additionally, because it doesn't matter what order they are drawn in, it should be nice and quick.

But the individual chunks...

I think the individual chunks in virtual memory can simply be (flat) 3D arrays in a hash map. That should allow super flexibility LIVE, and if we multithread the loading and conversion from some more compressed format, we should see significant performance improvements.

If we truly use multithreading, I could keep the octree and simply convert back and forth between flat 3D arrays and the octree.

I can use bitwise operators in the octree to raise efficiency.
Additionally, the octree "data" can contain bytes, not ints, which are 64 bits vs. 8 bits -> 8x smaller.

I definitely also need better meshing, but with the fast neighbor access I should see significant performance improvements.

I just initialize the Array to: "byte array[10000] = 0;"
With a byte, I will have 256 different options for blocks, which should be sufficient.
and read the non-sparse elements back out. This should be a very efficient storage method.

Finally, multithread saving and loading.

This is difficult.

- Octree Try Majority function for map-rendering / LOD functions
-> Higher Efficiency in chunk rendering, by combining vertices and occluded blocks!
-> Higher efficiency in chunk loading, by not reloading everything all the time!
-> Somehow merge chunk files into region files
-> Set the player's sprite to sit in top of the chunk (i.e. player movement)
-> Add a SUB-LOD so that we can have more than single block detail
-> Proper Implementation of LOD so we can zoom out and view the chunk map
- Keep walk orientation after rotating world
- Walking and rotations need to be continuous actions so that you can have animation support
-> "Drop Shadow" effect for the character sprites
-> See if it is possible to draw an outline for the mesh somehow also in post-processing
-> Player proper movement constraints

Renderer:
- World Fog
- Bloom (Lighting Blocks)
- Blur (Characters, DOF)
- Grain (Surfaces)
- Particles
- Outlines and Picking (e.g. characters)

//3.5 MB for a HIGH-DETAIL (10*16)^2 * 10 World

World Generator:
Features:
	-> Geology: Ore Deposits, Stone Types, Solubility / Brittleness, etc / Soil Quality
	-> Maps: Height, Humidity, Temperature, Volcanism, Drainage, Geology
	-> Rock: Mountains / Volcanoes / Boulders / Caves / Canyons / Geysers / etc.
	-> Water: Rivers / Streams / (Hot) Springs / Deltas / Swamps / Islands / Waterfalls
	-> Biomes: Deserts, Forests, Rainforests, Plains
	-> Dynamic Erosion and Flooding
	-> Dynamic Weather System (Clouds, Rain, Snow, Hail, Temp, Wind, Lightning, Sunlight)
	-> Vegetation for soil / water retention, enrichment, climate dependency, cooling
	-> Dynamic Range of Vegetation, where each plant is a point in some space, then sample near where the state is find nearest plant, place it

DONE:
- Keep rotation when loading new chunks
- Shadow Mapping!!!!
- Save World File Structure / File Folder Structure
- World generates and saves to file
- World loads chunks around a player
- Arbitrary world sizes!
-> PLAYER POSITION AND CHUNK LOADING
- Chunks have a position in chunk-space
- Some chunks are loaded, some are not, and which ones are depends on the player position
- If chunks are loaded, they are simply rendered at appropriate position relative to the player
- World can save and load chunks into an array
- Chunks are rendered as single objects
- Chunks have normal maps and we can compute simple shading
- Colors for chunk faces are loaded dynamically
- Setup Basic Shadow Mapping and Normal Mapping / Ambient Lighting, etc.
- Chunks should construct a VBO, so that we render chunks, not individual cubes.
- Pick the block color based on the ID value in the chunk data.
*/
