# Inventory and Resource Hierarchy System

Bots need to be given some content to interact with. Right now their world is too blank for them to do interesting things.
Before I can even ask them to start making decisions, they need to be given things to do.
Therefore, I decided to start implementing the resource hierarchy into the simulation.

I want to define a resource hierarchy and conversion system, so that bots can begin to create wealth and progress individually.

Finally, a few fun extras that are easy to implement that add more depth and variety.

## Items and Item Conversion

Bots should exploit the natural resources by creating wealth along some resource hierarchy.
In order to do this, I need to define what constitutes a "resource" and the method of conversion.

Harvesting of resources will occur via tasks, that are specific to the resource. The task will drop an object onto the ground.

Items should be customizable, with names and other flags, such as material.
Items should also be able to contain memories and tasks.

Thus, objects contain complex tasks that perform a conversion or a travel, or anything upon interaction, and bots execute it.

The question of resource conversion really depends on the level of granularity that you want in the game.
I'm going for the granularity of a craftsman performing a job - therefore something like a bow should be craftable in one go from the appropriate resources.

Various items exist, which will be simply enumerated. Their class of object will simply an item property, so that we know if certain processes apply to it.

The conversion of an object will also be a task, that takes as its input the item that it gives, and as its output the items that it returns.
Workshops will then simple be objects that are displayed in the world and can be interacted with directly through the task.

Item Flags:
(Type): ... (all items!!)
(Status): "Worn", "Placed", "Contained", "Dropped", "Broken"
(Quality): "Perfect", "Great", "Normal", "Poor", "Terrible"
(Class): Natural, Farming, Animal, Animal, Processed, Furniture, Weapon, Clothing, Workshop
(Ability): Container, Interactive, Placeable, Readable


Base Resources:
  Wood, Stones, Clay, Ores, Water, Sand, Dirt, Gravel, Salt,
  Various plant types, vines, reeds, grasses, herbs, flowers,

Farming Goods:
  Food, Fruit, Vegetables, Spices, Cotton, Grapes, Berries,

Animal Goods:
  Pelts, Meat, Milk, Feathers, Bones

Processed Goods (through workshops or complex objects):
  Paper, Quills, Plans, Rope, Planks, Fabric, Pottery, Complex Objects
  Food, Drink, Barrels, Glass, Bricks, Metal, Carpet, Maps, Books, Compass,
  Sundial, Watch, Octant, Thermometer, Lockpick, Wallet, Waterskin, Lens, etc.
  Lockbox, Clock, Candle, Lantern, Microscope, Instruments, Grammophone, etc.
  Wheelbarrow, Plow, Sled, Wagon, Navigation Wheel,

Furniture:
  Complex Objects, Containers, Beds, Carpets, Chairs, Tables, Decorative
  Lights, Stove,

Weapons:
  Saber, Axe, Pistols, Rifles, Knife,

Armor and Clothing:
  Pants, Shirts, Coats, Hats / Hoods, Capes, Gloves, Boots, Specials

Complex Objects:









## Buildings and the Resource Chain

  Constructed from plans, are then built accordingly requiring resources.
  Resources are specified by a resource list, contained in the plan.
  This plan somehow needs to be able to be generated from a specification.
  Then it can be constructed. Buildings are multiblock structures.

  Houses, Commercial, Ships, Special Structures, etc.


Workshops:
  Shipmaking, Tannery, Planning Table (Maps, Navigation, Architecture),
  Cooking Table (Butchery, Fishing, Cooking), Smithing Table (Metalworking, Machinery, ),
  Carpentry Table (Woodworking, Carving, )
  Potters Wheel, Glass Forge, Forge,
  Crafter's Table (Pottery, Sculpting, etc).
  Brewing Table (Herbalism, Brewing, Plant Processing)
  Loom and Spindle (Clothes Making and Sewing and Stuff)
  Regular tables, on which other small useful complex objects can be placed.


### Item Object Requirements

We will have an item property enum, which items can contain, giving them certain properties.

Items and their types will have to be enumerated. So we have object base types, and a bunch of flags.

They contain a task object, along with the associated state.

If they for example are a complex interactable, or if they are a weapon, or wearable (and where).

Items need to contain a stack of memories, if they have multiple interactable objects.
The task will have to search the memory of the item to see if it possible to do a certain conversion.
The memory will contain the list of possible conversions.

The item still contains a task that is the appropriate task that should be called upon interaction.





## Final
I could tell them to simply collect resources, and we would have a complex civ-controller sim.
Next, I want to tackle giving a motivation to collect resources of their own accord.

If the resource hierarchy is thus constructed, bots can begin to attempt to construct resource chains into which they
can assemble themselves. That is the goal of the next section.

Selling at market stalls. Owning one, or a store of some kind.

Leading an organization or political faction. A workshop. A guild.
Also recruiting members, and engaging in politics and stuff.

Construct buildings, even under contract. Contracting for all kinds of work.

General contract system for payments for tasks.

Mapping and data collection on nature. I.e. literal exploring.
-> What kind of data is valuable to the NPCs?
Try to harvest yourself or let people know and they pay you. It's the frontier after all.

This also means ability restrictions.

## Ability Restrictions
Ability restrictions. The simple idea that not everybody can do everything!
This forces interaction. Should probably be a core principle.
This also forces progression and specialization.

Maybe the player can only queue tasks. And attempt to solve problems.
The bot does all the work though!

Bots could have different skills. How do I count this experience, while not clashing with
the bots inherent memory structure? This is difficult.

Bots don't really know about odd magical quests or anything.
They will know about natural occurences and rumors though.
And interpersonal conflicts, which will drive most behavior.

Bots thereby need to be able to react to specific types of events.

Should there be something like machines, that can do repetitive tasks?
Like a way to process things?
This could take the form of specific workshops or something that isn't a building,
but does something specific.

Bots definitely need a system for information continuity.
This means writing memories and tasks down with specific flags, and storing them in objects.
Thus, bots can interact with the objects and learn things, read things,
accept contracts, mandate tasks, become possessed, write a will, write a diary,
write business ledgers, write delivery requests and receipts, etc. etc.

Thereby, some items (like paper) should be quite important.
But this requires some kind of crafting system from which paper can be made through
natural resources. Also ink and quills I suppose.

This means we need a crafting system, from which some resources can create other resources.
This could be skill based, to give quality to items, or to govern success or speed.
Or even the ability to do it in the first place.

To learn an ability, I guess it could just be a task. Or tasks could have a set of flags
that specify whether or not you need to have a certain ability flag to do it.
And the ability flag for a bot could just be that they remember having been told how to do it,
i.e. they either researched it or asked somebody to show them.

There could also be a natural way to do it, if the character is gifted, then maybe They
can "figure it out" and become inspired and do something.

What kind of ability flags would we want though?










Through machinery you could make small useful objects that add status information
to the adventurer (like the world-time if they have a clock),
