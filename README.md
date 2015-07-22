# fiad_halcon

contains the ros-halcon interface and fiad halcon components.

The halcon part of a component can exist in two forms:
- For developement and debugging, one can use a hdevelop program or procedure directly. Such are called with the HDevEngine. Compilation is not needed of the halcon specific part, and changes can thereby be done without rebuilding. Visualization implementations are available for graphical feedback, the overloaded functios are taken from the examples provided by halcon and modified with some fixes. The provided interface aims to simplify this type of components.
- For effizient execution of completed functions, hdevelop can generate c++ code for the internal profgrams and procedures. calling the generated functions differs a bit from the original calling strategy. The provided examples show how to implement the node to support both interfaces in a smooth way. Since the generated programs can be assecced directly with function calls, no extra interface is needed after the code generation.

The halcon bindings offers a generic way to call both generated code 
and procedures with HDevEngine by just a define opption. 
In this way the code using the halcon does not have to care about 
which alternative is beeing used.

The halcon bridge provides method for converting halcon images 
to and from ROS sensor_msgs/Image

prefferably the two parts should be seperated later on.
Current build set as executable, but should be libs later on. 
