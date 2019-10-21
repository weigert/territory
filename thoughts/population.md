#Mandates / Task Revamp:

**Done:**
- Can robustly execute arbitrary tasks in a hierarchical manner
- Can respond to interrupt events
- Can 3D Pathfind using A* in the world and walk around autonomously
- Can have animations associated with their tasks, which they must complete before executing the next task

**To-Do**:
- Expand task library with more basic behavior elements.
	- Tasks for: Skills/Crafts, Economy, Resources, Inventory, Fighting, Movement, Mandates, Communication, Memory, etc.
- Implement a group structure with land-ownership rights
- Add items for interaction, as well as storage of tasks and memories inside items


##Additional World-Content
Need to expand the content my bots can interact with...

##Recap:
State is an object of local metric and non-metric information
Can be measured from any bots perspective at any time
Contains quantities like position, time, object, botID, etc.

Memory is an absolute, previously observed world state.
Stored in an importance-sorted queue inside each bot
Memories that are used more often rise in importance in their minds.
Act as associations between things (locations, bots, objects, times)
Can be shared between bots to spread information
-> I know where I left my axe
-> I know where I usually sleep
-> I know where I last saw somebody
-> etc.

Tasks are single-step operations.
Tasks can contain sub-tasks.
Tasks capture ALL possible behavior of bots.
Tasks have parameters: State and Internal
-> State: An absolute, locally measured world-state
-> Internal: The bots parameters (viewDist, etc.)
Tasks can query the memory to inform sub-tasks parameters intelligently.

##Task Management Tasks
The system is robust, but bots need a wider range of possible behavior.

Bots need to be able to:
-> Prioritize Tasks
-> Interject Tasks upon events
-> Stop tasks upon events
-> Restart tasks

All of this should ideally be done using tasks, so we don't need more system overhead. To do this, bots are given a stack of tasks called "mandates". These are tasks they are "supposed" to do. Now they need to decide whether to do them, and how.

When a bots current task is empty, it is set to "decide", that takes a look at existing mandates and does some manipulations.

- Task::decide

Loop through mandates:
-> If the mandate is inappropriate, we delete it.
-> Under certain circumstances, we delegate it.
-> The most appropriate mandate is put into our current task.
  -> Important is for example time of day, or the bots hunger and stuff.
  -> Somehow we need a method to generate a score
-> If it should not be repeated, it is also removed from the list.
-> Continue

If an interrupt flag is set, we do a "listen", "think" and "decide" task all at once.

If a bot has no mandates, then it does an "interpret" task.

- Task::interpret

Interpret generates mandates from group membership / ownership.
Interpret can also generate tasks from objects!!

Here are some new task ideas:
-> Make sure they are properly parametrized, so that we avoid duplicate stuff.

##Memory Oriented Tasks
Task::dream     - Wait and Think / Listen / Sleep
Task::read      - retrieve memories from Objects
Task::write     - add memory objects to books
Task::inscribe  - Write tasks to objects
Task::interpret - Generate tasks from objects
Task::research  - High level tasks to collects lots of information on memory query

##Communication Tasks
Task::tell      - Write a memory to short-term memory and interrupt
Task::listen    - Write short term to long-term memory
Task::ask       - Mandate respond task with a query and interrupt
Task::respond   - Query state of task and tell to issuer
Task::converse  - Query Memory, construct converse mandate and interrupt (repeat until boredom)

##Mandate Management Tasks
Task::decide    - From list of possible tasks, sort them / manage and pick one
Task::request   - Give a mandate to somebody else in form of task
Task::report    - Report successful mandate execution to mandate issuer? Necessary?

##Group Management Tasks
Task::join      - Join a group in the hierarchy
Task::promote   - Move up in the hierarchy
Task::leave     - Leave a group

##Movement Tasks
Task::follow  - Move to a point a certain distance away from an individual (is also equal to avoid)

##Fighting Tasks
Task::hit     - Do a damage roll to adjacent bot
Task::attack  - Repeatedly follow and hit adjacent Bot (optionally following at distance)
Task::steal   - Rob somebody / Loot somebody? Tell them you robbed them (lol) and Set interrupt flag

##Inventory Management Tasks???
Task::seek
Task::collect
Task::take
Task::convert

Task::consume
Task::give
Task::trade
Task::store    - Place in container somehow? (i.e. blocks have inventories)
Task::retrieve - Get a certain quantity of a certain object?

##Resource Management Tasks??
-> Storage Handling
-> Hauling
-> Emptying
-> Inventory Management
-> Find / Collect Items to Inventory
-> Forage Items
-> Collect Resource from Block
  -> Requires Tools?
-> Place Somewhere
-> Clear an Area of Items / Blocks

##Economics Tasks?
-> Request Items for Money
-> Send Items to somebody remotely (messenger)
-> Tax
-> Hire a group collectively to do a task?
-> Rob
-> Balance Books
-> Pay

##Skill Tasks?
-> Craft Items
-> Write Books / Research
-> Cartography
-> Navigate
-> Transport Goods
-> Ship Building
-> Building Building
-> Trapping / Fur Hunting
-> Farming
-> Smithing


## More Systems:
-> Land Management by Groups?
-> How would warfare work?
-> How would you hire a whole group to do a specific task?
-> Because if you can delegate a single task to a large group, you get lots of potential.
  -> Example: Everyone collect wood
  -> Example: Everyone attack this guy
  ->
-> Books and Reading / Artifacts
  -> Can share information as well as mandate group creation with certain goals
    -> Example: The revolutionary communist rock (recruit everyone to kill a certain group)
    -> Tells people to find people of a certain type and mandates that they join (lmao)
-> Groups:
  -> This means founding a group, creating a founding object, giving them a purpose, leaving groups, forging relations to other groups, etc. so that they can then interpret tasks from these groups.
  -> e.g. Groups with the "Recruit" Option will try to recruit other bots. But bots will need to look at their ideals and decide if they like it or not. This means bots need to be randomly generated with stuff they like and don't like.

Books:
-> Bots can read memories from other bots about states, written on objects
-> i.e. objects store memories, which are unchangeable
-> bots can also record their memories in objects
-> this means they can refer to them later if necessary
-> This can also simply be instructions
-> You can create a memory virus by telling bots to tell other bots stuff
-> What if a memory contains a task handle and a state
-> Then you can share a memory with a task handle and tell somebody to do it
-> What if instead of always storing what we're doing in our memory, we write stuff down in diaries?
-> Parse memories so books become readable for human eyes (i.e. we read what bots think / know)
-> Parse Tasks so bot can describe actions / responsibilities

Higher Level:
-> A bot should be able to randomly decide to walk to a place they read about in a book.
-> Or look for someone they heard about in association with something and ask them
-> This requires high-level tasks with recursive structures
  -> Research

Problems:
How does the bot have "experience"?
It will only have experience if it tracks its effect on the world I surmise.
What if State additionally stores a task name?
Bots only have experience about the world state, but can't predict what will happen. They just do.
