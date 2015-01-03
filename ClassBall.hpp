#ifndef CuteBall0101110_h
#define CuteBall0101110_h

#include <SFML/Graphics.hpp>
#include "ClassVector.hpp"
#include <vector>

class TypeBall
{
private:
    //Private variables
    float   radius;
    int     sides;

    TypeVec CurVelocity;            //mathematical vector; contains Magnitude, Direction, Origin, and TimeStamp
    TypeVec TotalForce;
    std::vector <TypeVec> Acceler;  //Acceleration vectors m/s^2

    float   Elasticity;
    float   Mass;                   //Kg

    TypeRGB fillColor;
    TypeRGB outlineColor;

    sf::CircleShape sfmlCircle;

public:
    //Constructors
    TypeBall();
    TypeBall(float Radius);
    TypeBall(float Radius, TypeVec CurVel);

    //set Methods
    void setElasticity (float Elast);
    void setMass (float BallMass);
    void setVelocity (TypeVec Velocity);
    void setSpeed (float VelMagnitude);
    void setPosition (TypeCartesian NewPos);
    void setPixelPosition (TypeCartesian PixPos);
    void setRadius (float NewRadius);
    void setSides (int NewCount);
    void setFillColor (unsigned short R, unsigned short G, unsigned short B);
    void setOutlineColor (unsigned short R, unsigned short G, unsigned short B);
    void setFromCartesian (TypeCartesian NewVelocity, TypeCartesian NewPosition, float atTime=0.0);

    void setSFMLCirclePref(float CirRadius, unsigned int Sides, TypeRGB fillcolor, TypeRGB OutlineColor, float OutlineWidth=1.0);

    //get Methods
    float getRadius ();
    float getSpeed ();
    float getElasticity ();
    float getMass ();
    TypeCartesian getPosition();
    TypeCartesian getPixelPosition();
    TypeVec getVelocity ();
    sf::CircleShape getSFMLcircle();

    TypeVec CalcDistanceToPoint (TypeCartesian Point);
    TypeCartesian PixelPosition (TypeCartesian MeterPos);
    TypeCartesian MeterPosition (TypeCartesian PixPos);
    float PixelDistance (float MeterDist);
    float MeterDistance (float PixelDist);
    void showData ();
};

#endif // CuteBall0101110_h
