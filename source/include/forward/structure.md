To avoid cyclical dependency problems, we can structure our object definitions in the following way:

    ( main.cpp )
      - Include the main header file
      - Main Function
    ( main.h )
      - Include all general dependencies (e.g. stdlib)
      - Include (unordered) class .cpp files
    ( x.cpp )
      - Direct inclusion of all dependency class header files (y.h)
      - Direct inclusion of own class header file (x.h)
      - Beneath, contains member functions to its class X
    ( x.h )
      - Pragma Once, then include all dependency class forward declarations
      - Beneath, full class definition of class X
    ( x.fwd.h )
      - Single Pragma Once forward declaration of the class x
      
Example:

    main.cpp
      #include main.h
      //... main function
      
    main.h
      #include <external_dependency>
      #include x.cpp
      #include z.cpp //Unordered!
      #include y.cpp  
      
    x.cpp
      #include <y.h>
      #include <z.h>
      #include <x.h> //Include ourselves last
      //...member functions
      
    x.h
      #pragma once
      #include x.fwd.h  //Order irrelevant
      #include y.fwd.h
      #include z.fwd.h
      
      class X{  //Works even though in main.h, x.cpp was included before z.cpp
        Z z;
        Y y;
      };
