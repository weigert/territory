-> Anthill.cpp
  -> Anthill.h
    -> geometry.cpp
      -> geometry.h
    -> memory.cpp
      -> memory.h
        -> pragma once
        -> forward/geometry.fwd.h
        -> forward/memory.fwd.h
    -> state.cpp
      -> state.h
        -> pragma once
        -> forward/world.fwd.h
        -> forward/geometry.fwd.h
        -> forward/population.fwd.h
        -> forward/bot.fwd.h
      -> world.h
        -> pragma once
        -> forward/world.fwd.h
      -> population.h
        -> pragma once
        -> forward/population.fwd.h
        -> forward/bot.fwd.h
        -> forward/world.fwd.h
      -> bot.h
        ->
    -> bot.cpp
      -> bot.h
      -> mandate.cpp
      -> world.cpp
      -> task.cpp
    -> population.cpp
      -> population.h
        -> pragma once
        -> forward/population.fwd.h
        -> forward/bot.fwd.h
        -> forward/world.fwd.h
      -> world.h
        -> pragma once
        -> forward/world.fwd.h
      -> task.h
        -> pragma once
        -> forward/world.fwd.h
    -> task.cpp
      -> bot.h
      -> population.h
      -> world.h
      -> state.h


Function Lists need to see full classes and their members!! that's why...
Fully Cyclical Dependencies:

Generic Structure:
-> Main.cpp
  -> Main.h
    -> Class1.cpp
    -> Class2.cpp
    -> Class3.cpp

-> Class.cpp
  -> Dependency1.h
  -> Dependency2.h
  -> Dependency3.h
  -> Class.h

-> Class.h
  -> Pragma Once
  -> class.fwd.h
  -> dependency1.fwd.h
  -> dependency2.fwd.h
  -> dependency3.fwd.h
