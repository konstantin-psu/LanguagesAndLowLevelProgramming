//------------------------------------------------------------------------- 
// Copyright (c) Portland State University.
//------------------------------------------------------------------------- 

// Jacobi method for solving a Laplace equation.  
// 
//
use BlockDist;
use Time;

// Parameters
config const epsilon = 0.001;  // convergence tolerance
config const n = 8;            // n - matrix dimension size, verbose - verbosity flag
config const verbose = 0;  // --ver verbosity [true|false] - false default
const D = {0..n-1, 0..n-1};    // 2 dimensional domain
const id = {1..n-2,1..n-2};    // inner domain
const BD = D dmapped Block(D); // distribute among locales
const innerDomain = BD[id];

// Jacobi distributed -- return the iteration count.
// 
proc jacobi(x: [BD] real, epsilon: real) { 
    var cnt = 0;			// iteration counter
    var xnew: [BD] real = x;

    do {	
        forall ij in innerDomain do { // innerDomain is distributed so job is done on appropriate locale
                                      // Will Generate lots of spam
                                      // Will Generate lots of spam
                                      // here.id is the locale that is running current iteration
                                      // x(ij).locale.id is the locale that holds the data
                                      // when x(ij+offset).locale.id != here.id message passing is required
            if (verbose) {
                writeln("here.id, x(ij).locale.id: ["+here.id+" , "+x(ij).locale.id + "," +  "]");
            }
            xnew(ij) = (x(ij+(-1,0)) + x(ij+(0,-1)) + x(ij+(1,0)) + x(ij+(0,1))) / 4.0;
        }

        const delta = max reduce abs(xnew[innerDomain] - x[innerDomain]);
        x = xnew;
        cnt+= 1;
    } while (delta > epsilon);

    writeln("");
    return cnt;
}


proc init_array(x: [BD] real) {
    x = 0.0;
    x[n-1, 0..n-1] = 1.0;         // - setting boundary values
    x[0..n-1, n-1] = 1.0;
}

// Main routine.
//
proc main(args: [] string) {
    var a: [BD] real = 0;	// mesh array

    var cnt: int = 0;
    init_array(a);
    cnt = jacobi(a, epsilon);

    writeln("Mesh size: ", n, " x ", n, ", epsilon=", epsilon);
    writeln(cnt, " -- convergence steps");
    return 0;
}
