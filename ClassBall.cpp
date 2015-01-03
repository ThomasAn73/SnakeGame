#include "ClassBall.hpp"
#include <cmath>
#include <SFML/Graphics.hpp>
#include <iostream>

//***TYPEBALL CLASS implementation****************************************************************************
//************************************************************************************************************

//---CONSTRUCTORS---------------------------------------------------------------------------------------------
TypeBall::TypeBall ()
{
    radius=2; sides=4;

    setFillColor(0,0,0); setOutlineColor(0,0,255);
    setSFMLCirclePref(radius,sides,fillColor, outlineColor);
}

TypeBall::TypeBall (float CirRadius)
{
    if (CirRadius<=0) CirRadius=2;
    radius=CirRadius; sides=CirRadius*2;

    setFillColor(0,0,0); setOutlineColor(0,0,255);
    setSFMLCirclePref(radius,sides,fillColor, outlineColor);
}

TypeBall::TypeBall (float CirRadius, TypeVec CurVel)
{
    if (CirRadius<=0) CirRadius=2;
    radius=CirRadius; sides=CirRadius*2;
    CurVelocity=CurVel; //Memberwise copy; I hope.

    setFillColor(0,0,0); setOutlineColor(0,0,255);
    setSFMLCirclePref(radius,sides,fillColor, outlineColor);
}

//---GET methods------------------------------------------------------------------------------------
float TypeBall::getRadius () { return radius; }

float TypeBall::getSpeed () { return CurVelocity.getMagnitude(); }

float TypeBall::getElasticity () {return Elasticity;}

float TypeBall::getMass () {return Mass;}

TypeCartesian TypeBall::getPosition() { return CurVelocity.getOrigin(); }

TypeVec TypeBall::getVelocity () { return CurVelocity; }

TypeCartesian TypeBall::getPixelPosition()
{
    TypeCartesian MetPos=CurVelocity.getOrigin();
    return PixelPosition(MetPos);
}

sf::CircleShape TypeBall::getSFMLcircle() { return sfmlCircle; }

//---SET methods--------------------------------------------------------------------------------------------
void TypeBall::setPixelPosition (TypeCartesian PixPos)
{
    TypeCartesian MetPos=MeterPosition(PixPos);
    setPosition(MetPos);
}

void TypeBall::setSFMLCirclePref(float CirRadius, unsigned int Sides, TypeRGB fillcolor, TypeRGB OutlineColor, float OutlineWidth)
{
    sfmlCircle.setRadius (CirRadius);
    sfmlCircle.setPointCount (Sides);
    sfmlCircle.setOutlineThickness(OutlineWidth);
    sfmlCircle.setFillColor(sf::Color(fillColor.R,fillColor.G,fillColor.B));
    sfmlCircle.setOutlineColor(sf::Color(outlineColor.R,outlineColor.G,outlineColor.B));
}

void TypeBall::setPosition (TypeCartesian NewPos)
{
    CurVelocity.setOrigin(NewPos);              //Store position in meters

    TypeCartesian PixPos=PixelPosition(NewPos); //Convert meters to pixels
    sfmlCircle.setPosition(PixPos.X-radius,PixPos.Y-radius);
}

void TypeBall::setFromCartesian (TypeCartesian VelocityComponents, TypeCartesian NewPosition, float atTime)
{
    CurVelocity.setFromCartesian(VelocityComponents, NewPosition, atTime);

    TypeCartesian PixPos=PixelPosition(NewPosition); //Convert meters to pixels
    sfmlCircle.setPosition(PixPos.X-radius,PixPos.Y-radius);
    //std::cout<<"X:"<<PixPos.X<<" Y:"<<PixPos.Y<<"\n";
}

void TypeBall::setElasticity (float Elast) { if (Elast<0) {Elast=1;} Elasticity=Elast;}

void TypeBall::setMass (float BallMass) { if (BallMass<0) {BallMass=1;} Mass=BallMass;}

void TypeBall::setSpeed (float VelMagnitude)
{ CurVelocity.setMagnitude(VelMagnitude); }

void TypeBall::setVelocity (TypeVec Velocity)
    { CurVelocity=Velocity; }

void TypeBall::setFillColor (unsigned short R, unsigned short G, unsigned short B)
{
    fillColor.R=R; fillColor.G=G; fillColor.B=B;
    sfmlCircle.setFillColor(sf::Color(R,G,B));
}

void TypeBall::setOutlineColor (unsigned short R, unsigned short G, unsigned short B)
{
    outlineColor.R=R; outlineColor.G=G; outlineColor.B=B;
    sfmlCircle.setOutlineColor(sf::Color(R,G,B));
}

void TypeBall::setRadius (float NewRadius)
    {if(NewRadius>=2) {radius=NewRadius;} else {radius=2;} sfmlCircle.setRadius(radius);}

void TypeBall::setSides (int NewCount)
    { if(NewCount>=3) {sides=NewCount;} else {sides=3;} sfmlCircle.setPointCount(sides);}

//---general methods---------------------------------------------------------------------------------
float TypeBall::PixelDistance (float MeterDist) { return CurVelocity.PixelDistance(MeterDist); }

float TypeBall::MeterDistance (float PixelDist) { return CurVelocity.MeterDistance(PixelDist); }

TypeCartesian TypeBall::PixelPosition (TypeCartesian MeterPos)
{
    TypeCartesian PixelPos;
    PixelPos.X=PixelDistance (MeterPos.X);
    PixelPos.Y=-PixelDistance (MeterPos.Y);

    return PixelPos;
}

TypeCartesian TypeBall::MeterPosition (TypeCartesian PixPos)
{
    TypeCartesian MetPos;

    MetPos.X=MeterDistance(PixPos.X);
    MetPos.Y=-MeterDistance(PixPos.Y);;

    return MetPos;
}

TypeVec TypeBall::CalcDistanceToPoint (TypeCartesian Point)
{ return CurVelocity.CalcVectorToPoint(Point); }

void TypeBall::showData()
{
    TypeCartesian Origin=CurVelocity.getOrigin();
    using namespace std;
    cout<<"R:"<<radius<<" P:"<<sides<<" Vel:"<<CurVelocity.getMagnitude()<<" Dir:"<<CurVelocity.getDirection();
    cout<<" Xpos:"<<Origin.X<<" Ypos:"<<Origin.Y<<endl;

}
