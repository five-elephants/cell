A Tour of CELL
==============

(This tour is currently not complete.)


Functions
---------

Functionality in CELL is described as functions, e.g.:

    def plus(a : int, b : int) -> int: a + b

The body of the function is an expression, of which the result is returned when
calling the function. When nothing should be returned a semicolon ';' can be
used to absorb the result turning the expression into a statement. A compound
expression using curly braces '{' and '}' can be used to form more complex
expressions from a sequence of statements, e.g.:

    def mult_add(a : int, b : int, c : int) -> int: {
        var tmp = a * b;
        tmp + c
    }

Here, we have introduced a temporary variable 'tmp' within the function. The
last expression in a compound expression determines the value of the compound.

In general, CELL is an expression-based language. For example, you can write:

    def fac(x : int) -> int: if( x == 0 ) 1 else x * fac(x-1)

Also, functions can be overloaded by argument type.


Modules and hierarchy
---------------------

While functions allow to modularize functionality, modules provide a way to
define structural components for modeling.

    mod m: {
        var a : int
        var b : int
    }

This defines a module named 'm' with two member variables 'a' and 'b' of type
'int'. The curly braces around the module body are mandatory and items within
the module are not separated by semicolon. The member variables within a module
form the so called module frame.

The connectivity of modules is described by sockets:

    socket binop: {
        <= a : int
        <= b : int
        => y : int
    }

This defines a composite datatype 'binop' consisting of members 'a', 'b', and
'y', each of type 'int'. The arrows '<=' and '=>' define the directionality when
used as a connection. '<=' is input, '=>' is output, and '<>' would be both.

The module 'm' from before has an empty socket, which is the default if nothing
else is specified. The socket of a module can be explicitly named:

    mod adder <> binop: {
        // ... (C++ style comments by the way)
    }

Alternatively, it can be anonymously defined:

    mod adder <> socket: {
        <= a : int
        <= b : int
        => y : int
    } : {
        // module contents...
    }

The elements of its own socket are available through the implicitly defined
variable 'port' that is of type 'binop':

    def compute(): port.y = port.a + port.b;

The port is part of the module frame for its module.

Modules are instantiated within other modules using the 'inst' keyword:

    mod m: {
        inst plus : adder
    }

This creates an instance 'plus' of module 'adder'. The elements of the socket of
'plus' are available as 'plus.a' etc. within 'm'.

The motivation behind this syntax for module instantiation and definition is,
that modules should be cheap to define and use. This encourages modularization
of designs helping re-usability.
To achieve this, connectivity is abstracted into sockets. Modules implementing
the same socket can be exchanged without modification on the instantiating side.
The syntax also eliminates explicit port connection lists which are cumbersome
to type and redundant.



Discrete Event Simulation
-------------------------

Parallel functionality is executed through processes. A process is declared by
putting the 'process' keyword in front of a statement within a module:

    mod adder <> binop: {
        process: port.y = port.a + port.b;
    }

This defines a nameless function without arguments or return values. Processes
have non-blocking assignment semantics for members of the module frame, i.e.
port and member variables. This means, that all assignments to these variables
have effect only after the process has executed. Variables defined within a
process or functions called from the process have blocking assignment semantics,
since they only represent intermediate values that are not visible to other
processes.

Every time a process is executed, the simulator tracks which variables are read
to create a dynamic sensitivity list. All processes are executed once at the
start of simulation to initialize this list. Now, in every simulation cycle,
after all processes have run, assignments to module frames take effect and
modified members are detected. Processes which are sensitive to changed members
are re-run. This is repeated until a stable state or a maximum number of
iterations is reached.

The key points here are, that sensitivity lists are created automatically and
dynamically. The former makes it harder to forget signals from the list compared
to manual sensitivity lists. The latter allows for optimization when a process
is only conditionally sensitive to certain signals. Consider for example:

    process: {
        if( !en )
            port.y = 0;
        else
            port.y = compute_things(port.a, port.b, port.c, port.d);
    }

Here, most inputs are only read, when 'en' is set. As long as 'en' is not set,
the process does not need to be re-run on changes of 'a', 'b', 'c', or 'd'.


Continuous-time simulation
--------------------------

The support for continuous-time simulation is currently limited to three
keywords working similarly to process: once, periodic, and recurrent. They allow
to execute functions at specified points in time instead of depending on
sensitivity.

    mod m: {
        once(100 ns): {
            // executed when time equals 100 ns.
        }

        periodic(10 ns): {
            // executed every 10 ns.
        }

        recurrent(t): {
            // do something
            t + compute_interval()
        }
    }

The semantics of once and periodic should be clear. recurrent allows to
dynamically compute the next point of execution. It takes a single argument,
which is bound to the name 't' here, that holds the current simulation time. The
value of the expression, defined by the last line in the example, is the next
time of execution.

The intention here is to allow numerical solvers for differential equations that
have variable step sizes. This allows to have tailored solvers for particular
models.

