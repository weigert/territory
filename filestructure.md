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
