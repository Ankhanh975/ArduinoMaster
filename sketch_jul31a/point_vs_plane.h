#include <math.h>

// I should probably explain that (0, 0, 0) is the reference coordanies to the real world which is the point that is the end of the axis of servo_0
struct Point
{
    float x = 0;
    float y = 0;
    float z = 0;
};
struct Plane
{
    float A = 0;
    float B = 0;
    float C = 0;
    float D = 0;
};

int sign(float number)
{
    return (number > 0) ? 1 : -1;
}

void getPosistion(float angle1, float angle2, float angle3, float angle4, Point &Servo0_up, Point &Servo0_down,
                  Point &Servo1_up, Point &Servo1_down,
                  Point &Servo2_up, Point &Servo2_down,
                  Point &Servo3_up, Point &Servo3_down)
{
    float x_const = 4.46;
    float y_const = 6.60;
    float z_const = 2.85;

    Servo1_up.y = -y_const;
    Servo2_up.x = x_const;
    Servo3_up.x = x_const;
    Servo3_up.y = -y_const;

    float _angle1 = abs(90 - angle1);
    float _angle2 = abs(90 - angle2);
    float _angle3 = abs(90 - angle3);
    float _angle4 = abs(90 - angle4);

    float alpha1 = radians(_angle1);
    float alpha2 = radians(_angle2);
    float alpha3 = radians(_angle3);
    float alpha4 = radians(_angle4);

    // Calculate the pos in 3d of Servo0
    Servo0_down.x = Servo0_up.x + z_const * sin(alpha1) * sign(_angle1 - 90);
    Servo0_down.y = Servo0_up.y;
    Servo0_down.z = Servo0_up.z - z_const * cos(alpha1);

    // Calculate the pos in 3d of Servo1
    Servo1_down.x = Servo1_up.x + z_const * sin(alpha2) * sign(_angle2 - 90);
    Servo1_down.y = Servo1_up.y;
    Servo1_down.z = Servo1_up.z - z_const * cos(alpha2);

    // Calculate the pos in 3d of Servo2
    Servo2_down.x = Servo2_up.x + z_const * sin(alpha3) * sign(_angle3 - 90);
    Servo2_down.y = Servo2_up.y;
    Servo2_down.z = Servo2_up.z - z_const * cos(alpha3);

    // Calculate the pos in 3d of Servo3
    Servo3_down.x = Servo3_up.x + z_const * sin(alpha4) * sign(_angle4 - 90);
    Servo3_down.y = Servo3_up.y;
    Servo3_down.z = Servo3_up.z - z_const * cos(alpha4);
}

void calculatePlaneEquation(Point P1, Point P2, Point P3, Plane &plane)
{
    // Compute vectors v1 and v2
    float v1x = P2.x - P1.x;
    float v1y = P2.y - P1.y;
    float v1z = P2.z - P1.z;

    float v2x = P3.x - P1.x;
    float v2y = P3.y - P1.y;
    float v2z = P3.z - P1.z;

    // Compute the cross product to get the normal vector (A, B, C)
    plane.A = v1y * v2z - v1z * v2y;
    plane.B = v1z * v2x - v1x * v2z;
    plane.C = v1x * v2y - v1y * v2x;

    // Compute D using the normal vector and point P1
    plane.D = -(plane.A * P1.x + plane.B * P1.y + plane.C * P1.z);
}

float distancePointToPlane(Plane plane, Point point)
{
    // Calculate the numerator of the distance formula
    float numerator = abs(plane.A * point.x + plane.B * point.y + plane.C * point.z + plane.D);

    // Calculate the denominator of the distance formula
    float denominator = sqrt(plane.A * plane.A + plane.B * plane.B + plane.C * plane.C);

    // Calculate the distance
    float distance = numerator / denominator;

    return distance;
}

float evaluatePlane(float A, float B, float C, float D, float x, float y, float z)
{
    return A * x + B * y + C * z + D;
}

bool isSameSideOfPlane(float A, float B, float C, float D, float x1, float y1, float z1, float x2, float y2, float z2)
{
    // Function to check if two points are on the same side of the plane
    // Function to evaluate the plane equation for a given point

    float eval1 = evaluatePlane(A, B, C, D, x1, y1, z1);
    float eval2 = evaluatePlane(A, B, C, D, x2, y2, z2);
    return (eval1 * eval2) > 0;
}

int main_calculation(float angle1, float angle2, float angle3, float angle4, float &d)
{
    //  115 120 60 115
    Point Servo0_up;
    Point Servo0_down;
    Point Servo1_up;
    Point Servo1_down;
    Point Servo2_up;
    Point Servo2_down;
    Point Servo3_up;
    Point Servo3_down;
    getPosistion(angle1, angle2, angle3, angle4, Servo0_up, Servo0_down, Servo1_up, Servo1_down, Servo2_up, Servo2_down, Servo3_up, Servo3_down);

    Plane plane1;
    calculatePlaneEquation(Servo0_down, Servo1_down, Servo2_down, plane1);
    bool sameSide1 = isSameSideOfPlane(plane1.A, plane1.B, plane1.C, plane1.D, 0, 0, 10, Servo3_down.x, Servo3_down.y, Servo3_down.z);
    if (sameSide1)
    {
    }

    Plane plane2;
    calculatePlaneEquation(Servo0_down, Servo1_down, Servo3_down, plane2);
    bool sameSide2 = isSameSideOfPlane(plane2.A, plane2.B, plane2.C, plane2.D, 0, 0, 10, Servo2_down.x, Servo2_down.y, Servo2_down.z);
    if (sameSide2)
    {
    }

    Plane plane3;
    calculatePlaneEquation(Servo0_down, Servo2_down, Servo3_down, plane3);
    bool sameSide3 = isSameSideOfPlane(plane3.A, plane3.B, plane3.C, plane3.D, 0, 0, 10, Servo1_down.x, Servo1_down.y, Servo1_down.z);
    if (sameSide3)
    {
    }

    Plane plane4;
    calculatePlaneEquation(Servo1_down, Servo2_down, Servo3_down, plane4);
    bool sameSide4 = isSameSideOfPlane(plane4.A, plane4.B, plane4.C, plane4.D, 0, 0, 10, Servo0_down.x, Servo0_down.y, Servo0_down.z);
    if (sameSide4)
    {
    }

    // if angle of the legs is < 90 degree, the robot will fall and land on the front legs
    // (yeah, my robot can run forward and backward equally well,
    // but let's just say 1 side is the front and 1 side is the back)
    // else it'll fall and land on the back legs
    // if the robot is standing on the front legs, the back legs will be in the air
    // if the robot is standing on the back legs, the front legs will be in the air
    Plane choosenPlane;
    Point choosenPoint;
    if (sameSide1 == true || sameSide3 == true)
    {
        if (angle2 < 82 && angle3 < 82)
        {
            sameSide4 = false;
            choosenPoint = Servo0_down;
            choosenPlane = plane4;
        }
        else if (angle2 > 98 && angle3 > 98)
        {
            sameSide1 = false;
            choosenPoint = Servo3_down;
            choosenPlane = plane1;
        }
        else
        {
            sameSide4 = false;
            sameSide1 = false;
        }
    }
    else
    {
        if (angle4 < 82 && angle1 < 82)
        {
            sameSide2 = false;
            choosenPoint = Servo2_down;
            choosenPlane = plane2;
        }
        else if (angle4 > 98 && angle1 > 98)
        {
            sameSide3 = false;
            choosenPoint = Servo1_down;
            choosenPlane = plane3;
        }
        else
        {
            sameSide2 = false;
            sameSide3 = false;
        }
    }

    if (sameSide1 == true)
    {
        d = distancePointToPlane(choosenPlane, choosenPoint);
        return 0;
    }
    else if (sameSide2 == true)
    {
        d = distancePointToPlane(choosenPlane, choosenPoint);
        return 1;
    }
    else if (sameSide3 == true)
    {
        d = distancePointToPlane(choosenPlane, choosenPoint);
        return 2;
    }
    else if (sameSide4 == true)
    {
        d = distancePointToPlane(choosenPlane, choosenPoint);
        return 3;
    }
}