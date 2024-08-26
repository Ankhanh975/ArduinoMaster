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
    return (number > 0) ? 1 : 0;
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

void main_calculation(float angle1, float angle2, float angle3, float angle4)
{

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
    Serial.print("Servo3_down is: ");
    Serial.println(sameSide1);
    if (sameSide1)
    {
        Serial.print("The point Servo3_down is on the same side of the plane as the origin (0, 0, 10) with the following coordinates: ");
    }

    Plane plane2;
    calculatePlaneEquation(Servo0_down, Servo1_down, Servo3_down, plane2);
    bool sameSide2 = isSameSideOfPlane(plane2.A, plane2.B, plane2.C, plane2.D, 0, 0, 10, Servo2_down.x, Servo2_down.y, Servo2_down.z);
    Serial.print("Servo2_down is: ");
    Serial.println(sameSide2);
    if (sameSide2)
    {
        Serial.print("The point Servo2_down is on the same side of the plane as the origin (0, 0, 10) with the following coordinates: ");
    }

    Plane plane3;
    calculatePlaneEquation(Servo0_down, Servo2_down, Servo3_down, plane3);
    bool sameSide3 = isSameSideOfPlane(plane3.A, plane3.B, plane3.C, plane3.D, 0, 0, 10, Servo1_down.x, Servo1_down.y, Servo1_down.z);
    Serial.print("Servo1_down is: ");
    Serial.println(sameSide3);
    if (sameSide3)
    {
        Serial.print("The point Servo1_down is on the same side of the plane as the origin (0, 0, 10) with the following coordinates: ");
    }

    Plane plane4;
    calculatePlaneEquation(Servo1_down, Servo2_down, Servo3_down, plane4);
    bool sameSide4 = isSameSideOfPlane(plane4.A, plane4.B, plane4.C, plane4.D, 0, 0, 10, Servo0_down.x, Servo0_down.y, Servo0_down.z);
    Serial.print("Servo0_down is: ");
    Serial.println(sameSide4);
    if (sameSide4)
    {
        Serial.print("The point Servo0_down is on the same side of the plane as the origin (0, 0, 10) with the following coordinates: ");
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
        Serial.print("The robot is standing on the front legs with the following coordinates: ");
        if (angle1 < 90)
        {
            Serial.print("The robot will fall and land on the front legs with the following coordinates: ");
            sameSide4 = false;
            choosenPoint = Servo0_down;
            choosenPlane = plane4;
        }
        else
        {
            sameSide1 = false;
            choosenPoint = Servo3_down;
            choosenPlane = plane1;
        }
    }
    else
    {
        Serial.print("The robot will fall and land on the back legs with the following coordinates: ");
        if (angle2 < 90)
        {
            sameSide2 = false;
            choosenPoint = Servo2_down;
            choosenPlane = plane2;
        }
        else
        {
            sameSide3 = false;
            choosenPoint = Servo1_down;
            choosenPlane = plane3;
        }
    }
    float d = distancePointToPlane(choosenPlane, choosenPoint);
    Serial.print("The distance to the choosen point is: ");
    Serial.println(d);
    Serial.print("The chosen point is point number: ");
    if (sameSide1 == true)
    {
        Serial.println("Servo0");
    }
    else if (sameSide2 == true)
    {
        Serial.println("Servo1");
    }
    else if (sameSide3 == true)
    {
        Serial.println("Servo2");
    }
    else
    {
        Serial.println("Servo3");
    }
}

// void setup()
// {
//     // Initialize serial communication at 9600 bits per second
//     Serial.begin(9600);
//     float angle1 = 0;
//     float angle2 = 80;
//     float angle3 = 80;
//     float angle4 = 45;

//     main_calculation(angle1, angle2, angle3, angle4);
// }
// void loop()
// {
//     delay(1000);
// }
// // #include <math.h>

// // struct Point
// // {
// //     float x;
// //     float y;
// //     float z;
// // };

// // int sign(float number)
// // {
// //     if (number > 0)
// //     {
// //         return 1;
// //     }
// //     else if (number < 0)
// //     {
// //         return -1;
// //     }
// //     else
// //     {
// //         return 0;
// //     }
// // }

// // void isClearFromGround(float angle1, float angle2, float angle3, float angle4, float &Angle1Dist, float &Angle2Dist, float &Angle3Dist, float &Angle4Dist)
// // {
// //     const float x = 4.46;
// //     const float y = 6.60;
// //     const float z = 1.47;

// //     Point Servo0 = {0, 0, 0};
// //     Point Servo1 = {0, -y, 0};
// //     Point Servo2 = {x, 0, 0};
// //     Point Servo3 = {x, -y, 0};

// //     float _angle1 = abs(90 - angle1);
// //     float _angle2 = abs(90 - angle2);
// //     float _angle3 = abs(90 - angle3);
// //     float _angle4 = abs(90 - angle4);

// //     float alpha1 = radians(_angle1);
// //     float alpha2 = radians(_angle2);
// //     float alpha3 = radians(_angle3);
// //     float alpha4 = radians(_angle4);

// //     // Calculate the pos in 3d of Servo0
// //     float x1 = z * sin(alpha1) * sign(_angle1 - 90);
// //     float z1 = z * cos(alpha1);

// //     Servo0.x = Servo0.x + x1;
// //     Servo0.z = Servo0.z - z1;

// //     // Calculate the pos in 3d of Servo1
// //     float x2 = z * sin(alpha2) * sign(_angle2 - 90);
// //     float z2 = z * cos(alpha2);

// //     Servo1.x = Servo1.x + x2;
// //     Servo1.z = Servo1.z - z2;

// //     // Calculate the pos in 3d of Servo2
// //     float x3 = z * sin(alpha3) * sign(_angle3 - 90);
// //     float z3 = z * cos(alpha3);

// //     Servo2.x = Servo2.x + x3;
// //     Servo2.z = Servo2.z - z3;

// //     // Calculate the pos in 3d of Servo3
// //     float x4 = z * sin(alpha4) * sign(_angle4 - 90);
// //     float z4 = z * cos(alpha4);

// //     Servo3.x = Servo3.x + x4;
// //     Servo3.z = Servo3.z - z4;

// //     // Calculate the plane equation coefficients
// //     float A, B, C, D;
// //     calculatePlaneEquation(Servo1, Servo2, Servo3, A, B, C, D);
// //     Angle1Dist = distancePointToPlane(A, B, C, D, Servo0.x, Servo0.y, Servo0.z);

// //     calculatePlaneEquation(Servo0, Servo2, Servo3, A, B, C, D);
// //     Angle2Dist = distancePointToPlane(A, B, C, D, Servo1.x, Servo1.y, Servo1.z);

// //     calculatePlaneEquation(Servo0, Servo1, Servo3, A, B, C, D);
// //     Angle3Dist = distancePointToPlane(A, B, C, D, Servo2.x, Servo2.y, Servo2.z);

// //     calculatePlaneEquation(Servo0, Servo1, Servo2, A, B, C, D);
// //     Angle4Dist = distancePointToPlane(A, B, C, D, Servo3.x, Servo3.y, Servo3.z);
// // }
// // void calculatePlaneEquation(Point P1, Point P2, Point P3, float &A, float &B, float &C, float &D)
// // {
// //     // Compute vectors v1 and v2
// //     float v1x = P2.x - P1.x;
// //     float v1y = P2.y - P1.y;
// //     float v1z = P2.z - P1.z;

// //     float v2x = P3.x - P1.x;
// //     float v2y = P3.y - P1.y;
// //     float v2z = P3.z - P1.z;

// //     // Compute the cross product to get the normal vector (A, B, C)
// //     A = v1y * v2z - v1z * v2y;
// //     B = v1z * v2x - v1x * v2z;
// //     C = v1x * v2y - v1y * v2x;

// //     // Compute D using the normal vector and point P1
// //     D = -(A * P1.x + B * P1.y + C * P1.z);
// // }

// // float distancePointToPlane(float A, float B, float C, float D, float x1, float y1, float z1)
// // {
// //     // Calculate the numerator of the distance formula
// //     float numerator = abs(A * x1 + B * y1 + C * z1 + D);

// //     // Calculate the denominator of the distance formula
// //     float denominator = sqrt(A * A + B * B + C * C);

// //     // Calculate the distance
// //     float distance = numerator / denominator;

// //     return distance;
// // }
// // void setup()
// // {
// //     // Initialize serial communication at 9600 bits per second
// //     Serial.begin(9600);

// //     Serial.println("Starting test...");
// //     float angle1 = 0   ;
// //     float angle2 = 80 ;
// //     float angle3 = 80;
// //     float angle4 = 90;
// //     float Angle1Dist, Angle2Dist, Angle3Dist, Angle4Dist;
// //     isClearFromGround(angle1, angle2, angle3, angle4, Angle1Dist, Angle2Dist, Angle3Dist, Angle4Dist);
// //     Serial.print("Angle1Dist: ");
// //     Serial.println(Angle1Dist);
// //     Serial.print("Angle2Dist: ");
// //     Serial.println(Angle2Dist);
// //     Serial.print("Angle3Dist: ");
// //     Serial.println(Angle3Dist);
// //     Serial.print("Angle4Dist: ");
// //     Serial.println(Angle4Dist);

// // }
// #include <math.h>

// struct Point
// {
//     float x = 0;
//     float y = 0;
//     float z = 0;
// };
// int sign(float number)
// {
//     return (number > 0) ? 1 : 0;
// }

// void getPosistion(float angle1, float angle2, float angle3, float angle4, Point &Servo0_up, Point &Servo0_down,
//                   &Point &Servo1_up, &Point Servo1_down,
//                   &Point Servo2_up, &Point Servo2_down,
//                   &Point &Servo3_up, &Point Servo3_down)
// {
//     float x_const = 4.46;
//     float y_const = 6.60;
//     float z_const = 1.47;

//     Servo1_up.y = -y_const;
//     Servo2_up.x = x_const;
//     Servo3_up.x = x_const;
//     Servo3_up.y = -y_const;

//     float _angle1 = abs(90 - angle1);
//     float _angle2 = abs(90 - angle2);
//     float _angle3 = abs(90 - angle3);
//     float _angle4 = abs(90 - angle4);

//     float alpha1 = radians(_angle1);
//     float alpha2 = radians(_angle2);
//     float alpha3 = radians(_angle3);
//     float alpha4 = radians(_angle4);

//     // Calculate the pos in 3d of Servo0
//     Servo0_up.x = Servo0_down.x + z_const * sin(alpha1) * sign(_angle1 - 90);
//     Servo0_up.y = Servo0_down.y;
//     Servo0_up.z = Servo0_up.z - z_const * cos(alpha1);

//     // Calculate the pos in 3d of Servo1
//     Servo1_up.x = Servo1_down.x + z_const * sin(alpha2) * sign(_angle2 - 90);
//     Servo1_up.y = Servo1_down.y;
//     Servo1_up.z = Servo1_up.z - z_const * cos(alpha2);

//     // Calculate the pos in 3d of Servo2
//     Servo2_up.x = Servo2_down.x + z_const * sin(alpha3) * sign(_angle3 - 90);
//     Servo2_up.y = Servo2_down.y;
//     Servo2_up.z = Servo2_up.z - z_const * cos(alpha3);

//     // Calculate the pos in 3d of Servo3
//     Servo3_up.x = Servo3_down.x + z_const * sin(alpha4) * sign(_angle4 - 90);
//     Servo3_up.y = Servo3_down.y;
//     Servo3_up.z = Servo3_up.z - z_const * cos(alpha4);
// }
// void isClearFromGround(float angle1, float angle2, float angle3, float angle4, float &Angle1Dist, float &Angle2Dist, float &Angle3Dist, float &Angle4Dist, toStoreABCD &toStore)
// {
//     Point Servo0 = {0, 0, 0};
//     Point Servo1 = {0, -y_const, 0};
//     Point Servo2 = {x_const, 0, 0};
//     Point Servo3 = {x_const, -y_const, 0};

//     float _angle1 = abs(90 - angle1);
//     float _angle2 = abs(90 - angle2);
//     float _angle3 = abs(90 - angle3);
//     float _angle4 = abs(90 - angle4);

//     float alpha1 = radians(_angle1);
//     float alpha2 = radians(_angle2);
//     float alpha3 = radians(_angle3);
//     float alpha4 = radians(_angle4);

//     // Calculate the pos in 3d of Servo0
//     float x1 = z * sin(alpha1) * sign(_angle1 - 90);
//     float z1 = z * cos(alpha1);

//     Servo0.x = Servo0.x + x1;
//     Servo0.z = Servo0.z - z1;

//     // Calculate the pos in 3d of Servo1
//     float x2 = z * sin(alpha2) * sign(_angle2 - 90);
//     float z2 = z * cos(alpha2);

//     Servo1.x = Servo1.x + x2;
//     Servo1.z = Servo1.z - z2;

//     // Calculate the pos in 3d of Servo2
//     float x3 = z * sin(alpha3) * sign(_angle3 - 90);
//     float z3 = z * cos(alpha3);

//     Servo2.x = Servo2.x + x3;
//     Servo2.z = Servo2.z - z3;

//     // Calculate the pos in 3d of Servo3
//     float x4 = z * sin(alpha4) * sign(_angle4 - 90);
//     float z4 = z * cos(alpha4);

//     Servo3.x = Servo3.x + x4;
//     Servo3.z = Servo3.z - z4;

//     // Calculate the plane equation coefficients
//     float A, B, C, D;
//     calculatePlaneEquation(Servo1, Servo2, Servo3, A, B, C, D);
//     Angle1Dist = distancePointToPlane(A, B, C, D, Servo0.x, Servo0.y, Servo0.z);
//     Serial.print("Angle1 isSameSideOfPlane: ");
//     Serial.println(isSameSideOfPlane(A, B, C, D, Servo0.x, Servo0.y, Servo0.z, 0, 0, 10));
//     toStore.A1 = A;
//     toStore.B1 = B;
//     toStore.C1 = C;
//     toStore.D1 = D;

//     calculatePlaneEquation(Servo0, Servo2, Servo3, A, B, C, D);
//     Angle2Dist = distancePointToPlane(A, B, C, D, Servo1.x, Servo1.y, Servo1.z);
//     Serial.print("Angle2 isSameSideOfPlane: ");
//     Serial.println(isSameSideOfPlane(A, B, C, D, Servo1.x, Servo1.y, Servo1.z, 0, 0, 10));
//     toStore.A2 = A;
//     toStore.B2 = B;
//     toStore.C2 = C;
//     toStore.D2 = D;

//     calculatePlaneEquation(Servo0, Servo1, Servo3, A, B, C, D);
//     Angle3Dist = distancePointToPlane(A, B, C, D, Servo2.x, Servo2.y, Servo2.z);
//     Serial.print("Angle3 isSameSideOfPlane: ");
//     Serial.println(isSameSideOfPlane(A, B, C, D, Servo2.x, Servo2.y, Servo2.z, 0, 0, 10));
//     toStore.A3 = A;
//     toStore.B3 = B;
//     toStore.C3 = C;
//     toStore.D3 = D;

//     calculatePlaneEquation(Servo0, Servo1, Servo2, A, B, C, D);
//     Angle4Dist = distancePointToPlane(A, B, C, D, Servo3.x, Servo3.y, Servo3.z);
//     Serial.print("Angle4 isSameSideOfPlane: ");
//     Serial.println(isSameSideOfPlane(A, B, C, D, Servo3.x, Servo3.y, Servo3.z, 0, 0, 10));
//     toStore.A4 = A;
//     toStore.B4 = B;
//     toStore.C4 = C;
//     toStore.D4 = D;
// }
// void calculatePlaneEquation(Point P1, Point P2, Point P3, float &A, float &B, float &C, float &D)
// {
//     // Compute vectors v1 and v2
//     float v1x = P2.x - P1.x;
//     float v1y = P2.y - P1.y;
//     float v1z = P2.z - P1.z;

//     float v2x = P3.x - P1.x;
//     float v2y = P3.y - P1.y;
//     float v2z = P3.z - P1.z;

//     // Compute the cross product to get the normal vector (A, B, C)
//     A = v1y * v2z - v1z * v2y;
//     B = v1z * v2x - v1x * v2z;
//     C = v1x * v2y - v1y * v2x;

//     // Compute D using the normal vector and point P1
//     D = -(A * P1.x + B * P1.y + C * P1.z);
// }

// float distancePointToPlane(float A, float B, float C, float D, float x1, float y1, float z1)
// {
//     // Calculate the numerator of the distance formula
//     float numerator = abs(A * x1 + B * y1 + C * z1 + D);

//     // Calculate the denominator of the distance formula
//     float denominator = sqrt(A * A + B * B + C * C);

//     // Calculate the distance
//     float distance = numerator / denominator;

//     return distance;
// }
// // Function to evaluate the plane equation for a given point
// float evaluatePlane(float A, float B, float C, float D, float x, float y, float z)
// {
//     return A * x + B * y + C * z + D;
// }

// // Function to check if two points are on the same side of the plane
// bool isSameSideOfPlane(float A, float B, float C, float D, float x1, float y1, float z1, float x2, float y2, float z2)
// {
//     float eval1 = evaluatePlane(A, B, C, D, x1, y1, z1);
//     float eval2 = evaluatePlane(A, B, C, D, x2, y2, z2);
//     return (eval1 * eval2) > 0;
// }

// void setup()
// {
//     // Initialize serial communication at 9600 bits per second
//     Serial.begin(9600);

//     Serial.println("Starting test...");
//     float angle1 = 0;
//     float angle2 = 80;
//     float angle3 = 80;
//     float angle4 = 90;

//     // Calculate the absolute differences from 90 degrees
//     float diff1 = abs(angle1 - 90);
//     float diff2 = abs(angle2 - 90);
//     float diff3 = abs(angle3 - 90);
//     float diff4 = abs(angle4 - 90);

//     // Determine the smallest difference
//     float minDiff = diff1;
//     int closestAngleNumber = 0;
//     float closestAngle = angle1;

//     if (diff2 < minDiff)
//     {
//         minDiff = diff2;
//         closestAngleNumber = 1;
//         closestAngle = angle2;
//     }
//     if (diff3 < minDiff)
//     {
//         minDiff = diff3;
//         closestAngleNumber = 2;
//         closestAngle = angle3;
//     }
//     if (diff4 < minDiff)
//     {
//         minDiff = diff4;
//         closestAngleNumber = 3;
//         closestAngle = angle4;
//     }
//     int theServoToCheckNumber = 0;
//     if (closestAngleNumber == 0 || closestAngleNumber == 3)
//     {
//         theServoToCheckNumber = (closestAngle <= 90) ? 1 : 2;
//     }
//     else if (closestAngleNumber == 1 || closestAngleNumber == 2)
//     {
//         theServoToCheckNumber = (closestAngle <= 90) ? 0 : 3;
//     }

//     float Angle1Dist,
//         Angle2Dist, Angle3Dist, Angle4Dist;
//     toStoreABCD toStore;
//     isClearFromGround(angle1, angle2, angle3, angle4, Angle1Dist, Angle2Dist, Angle3Dist, Angle4Dist, toStore);

//     if (theServoToCheckNumber == 0)
//     {
//         Serial.print("Angle1Dist: ");
//         Serial.println(Angle1Dist);
//         Serial.print("A: ");
//         Serial.println(toStore.A1);
//         Serial.print("B: ");
//         Serial.println(toStore.B1);
//         Serial.print("C: ");
//         Serial.println(toStore.C1);
//         Serial.print("D: ");
//         Serial.println(toStore.D1);
//         float d = distancePointToPlane(toStore.A1, toStore.B1, toStore.C1, toStore.D1, Servo0_const.x, Servo0_const.y, Servo0_const.z);
//         Serial.print("Distance to Plane");
//         Serial.println(d);
//         if (d > 1.47)
//         {
//             Serial.print("isOffPlane: ");
//         }
//     }
// }
// void loop()
// {
// }