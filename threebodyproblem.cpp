#include <iostream>
#include <cmath>
#include <cpgplot.h>
#include <unistd.h>

const double G = 1.0;  // normalized gravity constant (this is only here so I could change it for testing, however, if it stays at 1.0 it could be removed from the program)

struct Body {
    double mass; // mass of the object
    double x, y; // coordinates for x and y
    double vx, vy; // velocity for x and y
    double ax, ay; // acceleration for x and y
};

// function to compute the gravitational force between two bodies
void compute(const Body& body1, const Body& body2, double& fx, double& fy) {
    double dx = body2.x - body1.x; // delta x (distance between the two bodies in x direction)
    double dy = body2.y - body1.y; // delta y (distance between the two bodies in y direction)
    double r = sqrt(dx * dx + dy * dy); // distance between the two bodies using pythagorean theorem
    double r_cubed = r * r * r; // r^3

    // gravitational force components
    fx = G * body1.mass * body2.mass * dx / r_cubed; // force in the x direction
    fy = G * body1.mass * body2.mass * dy / r_cubed; // force in the y direction
}

// function to update body positions and velocities using the leapfrog method
void iter(Body& body, const Body& other1, const Body& other2, double dt) {
    double fx1, fy1, fx2, fy2; // forces fx1 and fy1 for other1, fx2 and fy2 for other2 against body

    // compute forces between the bodies
    compute(body, other1, fx1, fy1); // compute force for x and y directions against other1
    compute(body, other2, fx2, fy2); // compute force for x and y directions against other2

    // update acceleration
    body.ax = (fx1 + fx2) / body.mass; // acceleration in x direction
    body.ay = (fy1 + fy2) / body.mass; // acceleration in y direction

    // leapfrog method: update velocity at half step
    body.vx += 0.5 * body.ax * dt; // velocity in x direction
    body.vy += 0.5 * body.ay * dt; // velocity in y direction

    // update position
    body.x += body.vx * dt; // position in x direction
    body.y += body.vy * dt; // position in y direction

    // compute forces again with updated positions
    compute(body, other1, fx1, fy1); // compute against other1
    compute(body, other2, fx2, fy2); // compute against other2

    // update acceleration again
    body.ax = (fx1 + fx2) / body.mass; // acceleration in x direction
    body.ay = (fy1 + fy2) / body.mass; // acceleration in y direction

    // leapfrog method: update velocity at full step
    body.vx += 0.5 * body.ax * dt; // velocity in x direction
    body.vy += 0.5 * body.ay * dt; // velocity in y direction
}

int main() {
    float size = 1.5; // size of the plot (default 1.5 for bumblebee example)
    double s = 0.7; // scale factor for use with circle test

    // all possible test cases used in the report

    // circle test
    // Body body1 = {1.0, 1.0, 0.0, 0.0, s * 1.0};
    // Body body2 = {1.0, -0.5, sqrt(3)/2, s * -sqrt(3)/2, s * -0.5};
    // Body body3 = {1.0, -0.5, -sqrt(3)/2, s * sqrt(3)/2, s * -0.5};

    // first chaos test
    // Body body1 = {1.0, 0.5, 0.0, 1.0, 1.0};  // random orbit that was changed to 1.01 mass for chaos test
    // Body body2 = {1.0, 1.0, 0.1, -0.3, -0.2};
    // Body body3 = {1.0, -0.5, -1.0, -0.5, 0.5};

    // Body body1 = {1.0, 0.5, 0.0, 1.0, 0.2};  // small object orbiting a smaller object affecting a third object
    // Body body2 = {1.5, 0.3, 0.1, -0.5, -0.5};
    // Body body3 = {0.5, -0.2, -1.3, -0.4, 0.5};

    // second chaos test
    // Body body1 = {1.0, 0.5, 0.0, 0.7, 0.4}; // changed the position of this object from y = 0.0 to y = 0.1
    // Body body2 = {1.0, -0.5, 0.1, 0.4, -0.6};
    // Body body3 = {1.0, 0.5, -0.9, 0.8, -0.6};

    // large mass test
    // Body body1 = {100.0, 0.0, 0.0, 0.0, 0.0}; // 0 velocity large mass in the middle
    // Body body2 = {1.0, -2.0, 0.0, 0.0, 5.0};
    // Body body3 = {1.0, 2.0, 0.0, 0.0, -5.0};

    // cases from https://arxiv.org/abs/1303.0181v1
    // double vsx = 0.51394; // II.C.2a yin-yang I
    // double vsy = 0.30474;
    // double vsx = 0.08330; // I.B.5 goggles
    // double vsy = 0.12789;
    double vsx = 0.18428; // I.A.3 bumblebee (high period)
    double vsy = 0.58719;
    // for use with above
    Body body1 = {1.0, -1.0, 0.0, vsx, vsy}; 
    Body body2 = {1.0, 1.0, 0.0, vsx, vsy};
    Body body3 = {1.0, 0.0, 0.0, -2.0 * vsx, -2.0 * vsy};

    // random test case for fast moving objects
    // Body body1 = {1.0, -1.0, -1.0, 0.01, 0.01};
    // Body body2 = {0.01, 0.5, 0.5, 1.0, -1.0};
    // Body body3 = {0.0001, 0.45, 0.45, 3.0, 0.0};

    double dt = 0.0001; // time step
    double totaltime = 100.0; // total time
    int steps = static_cast<int>(totaltime / dt); // number of steps (total time divided by time step)

    cpgbeg(0, "/XWINDOW", 1, 1); // begin a new plot
    cpgenv(-size, size, -size, size, 0, 0); // set it to the size of the plot given
    cpgask(0);
    cpglab("x", "y", "Three Body Problem"); // label the plot

    for (int step = 0; step < steps; ++step) { // iterate through the steps
        iter(body1, body2, body3, dt); // body1 against body2 and body3
        iter(body2, body1, body3, dt); // body2 against body1 and body3
        iter(body3, body1, body2, dt); // body3 against body1 and body2

        cpgsci(5); // set line to colour blue
        cpgpt1(body1.x, body1.y, 1); // plot body1 in blue
        cpgsci(2); // set line to colour red
        cpgpt1(body2.x, body2.y, 1); // plot body2 in red
        cpgsci(3); // set line to colour green
        cpgpt1(body3.x, body3.y, 1); // plot body3 in green

        // usleep(100); // uncomment this line for larger time steps
    }
    usleep(100000000); // to prevent the plot from closing immediately
    cpgend(); // end the plot

    return 0;
}