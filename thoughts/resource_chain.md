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

## Implementation

### Tasks

There are a number of tasks that are required to allow the bots to interact with the item system.

Task::collect **FINISHED**
Destroy a block and place drop it at the location. Check for appropriate tools.

Task::take
At a specified location, attempt to take a specified item out of world and place in inventory.
This includes sub-queries if items contain inventories, so you can look inside containers in this manner.
This means looking inside placed items if they are the specified location.

Task::find
In range, attempt to find a specified item or block at a location, and put the location inside the state.

Task::search
Recursively walk around and attempt to find.

Task::retrieve
Search memory for a location where an item is known to be, then go to that location (or near) and take.

Task::convert
Take a requested conversion and perform it in a bots inventory if possible.
Uses a state to give the inputs and outputs for items.

### Functions and Updates

I need a function that generates a drop-table from breaking specific blocks in the world.

I need to update my pathfinder to also just allow for short distance walking at a time. So that bots estimate where they have to go.
Make this sufficiently large.

I need to have an item generator, that generates prototype items with metadata.

I need a function in item that generates the task with recipes for workshops and complex items, loading it from metadata.

I need a function for an item to load its sprite.

I need a harvest function that destroys a block in the world and then places it in the dropped pile.

I need a rendering option for dropped and placed items.


## Buildings and the Resource Chain

  Constructed from plans, are then built accordingly requiring resources.
  Resources are specified by a resource list, contained in the plan.
  This plan somehow needs to be able to be generated from a specification.
  Then it can be constructed. Buildings are multiblock structures.

  Houses, Commercial, Ships, Special Structures, etc.

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
