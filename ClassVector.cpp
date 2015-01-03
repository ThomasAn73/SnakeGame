#include "ClassVector.hpp"
#include <cmath>

#define pi 3.141592654

//***TYPEVEC CLASS implementation*****************************************************************************
//************************************************************************************************************

//---CONSTRUCTORS---------------------------------------------------------------------------------------------
TypeVec::TypeVec () { Dirct=0; Magn=0; TimeStamp=0.0; VecOrigin.X=0.0; VecOrigin.Y=0.0;}

TypeVec::TypeVec (float Mag, float Dir, float Xpos, float Ypos, float atTime)
{
    setDirection(Dir);
    setMagnitude(Mag);
    setTimeStamp(atTime);
    setOrigin(Xpos, Ypos);
}

TypeVec::~TypeVec () {}


//---MEMBER functions-----------------------------------------------------------------------------------------

float TypeVec::ToRad (float Deg) { return 2.0 * pi * Deg/360.0; }

float TypeVec::ToDeg (float Rad) { return 360.0 * Rad / (2.0*pi); }

float TypeVec::PixelDistance (float MeterDist)
{ const float PixPerMeter=5*(100/2.54); return MeterDist*PixPerMeter; }

float TypeVec::MeterDistance (float PixelDist)
{ const float PixPerMeter=5*(100/2.54); return PixelDist/PixPerMeter; }

float TypeVec::CleanDir (float ThisDir, bool isPlane)
{
    if (std::abs(ThisDir)>360) {ThisDir=fmod(ThisDir,360.0);} //Put the Dirct back in the -360 to 360 range
    if (ThisDir<0) {ThisDir=360+ThisDir;}                     //Eliminate negative angles
    if (isPlane==true && ThisDir>=180) {ThisDir=ThisDir-180;}

    return ThisDir;
}

float TypeVec::OpositeDir()
{
    float Result;
    Result=Dirct+180.0;
    if (std::abs(Result)>360) Result=fmod(Result,360.0);

    return Result;
}

float TypeVec::ReflectionAngle (float PlaneAngle)
{
    float IncidentAngle, Result;
    Dirct=CleanDir(Dirct);
    PlaneAngle=CleanDir(PlaneAngle, true);
    IncidentAngle=Dirct-PlaneAngle;

    //the Dirct angle is always in reference to horizontal
    Result=-IncidentAngle + PlaneAngle;
    Result=CleanDir(Result);
    //std::cout<<"oldDir:"<<IncidentAngle<<" NewDir:"<<Result<<"\n";
    return Result;
}

//Direction angle in degrees from cartesian components
float TypeVec::CalcDirection (TypeCartesian Component)
{
    float Result;

    if (Component.X==0 && Component.Y>=0) {Result=acos(0.0);}
    else if (Component.X==0 && Component.Y<=0) {Result=-acos(0.0);}
    else if (Component.X>0 && Component.Y>=0) Result=atan(Component.Y/Component.X);
    else if (Component.X<0 && Component.Y<=0) Result=atan(Component.Y/Component.X)+ToRad(180);
    else if (Component.X<0 && Component.Y>=0) Result=atan(Component.Y/Component.X)+ToRad(180);
    else Result=atan(Component.Y/Component.X);

    Result=ToDeg(Result);

    return CleanDir(Result);
}

//Direction angle in degrees; given the current origin point and a destination point
float TypeVec::CalcDirectionToPoint (TypeCartesian DestPoint)
{ return CalcVectorToPoint(DestPoint).getDirection(); }

float TypeVec::CalcDistanceToPoint (TypeCartesian DestPoint)
{ return CalcVectorToPoint(DestPoint).getMagnitude(); }

TypeVec TypeVec::CalcVectorToPoint (TypeCartesian DestPoint)
{
    TypeVec ResultVec;
    float ResultDistance, Direction;
    TypeCartesian Distance;

    Distance.X=DestPoint.X-VecOrigin.X;
    Distance.Y=DestPoint.Y-VecOrigin.Y;

    ResultDistance=sqrt(Distance.X*Distance.X+Distance.Y*Distance.Y);
    Direction=CalcDirection(Distance);

    ResultVec.setDirection(Direction);
    ResultVec.setMagnitude(ResultDistance);

    return ResultVec;
}

TypeVec TypeVec::CalcVectorToPoint (TypeVec DestVec)
{
    TypeCartesian ToPoint=DestVec.getOrigin();
    return CalcVectorToPoint(ToPoint);
}

//---SET methods----------------------------------------------------------------------------------------------
void TypeVec::setOrigin (TypeCartesian Origin) { VecOrigin=Origin; }

void TypeVec::setOrigin (float Xpos, float Ypos) { VecOrigin.X=Xpos; VecOrigin.Y=Ypos; }

void TypeVec::setMagnitude (float Mag)
{
    if (Mag>=0) {Magn=Mag;}
    else { Magn=-Mag; Dirct=OpositeDir(); }
}

void TypeVec::setFromCartesian (TypeCartesian VelocityComponents, TypeCartesian NewPosition, float NewTimeStamp)
{
    Magn=sqrt(VelocityComponents.X*VelocityComponents.X + VelocityComponents.Y*VelocityComponents.Y); //This will always be positive (no need to call setMagnitude)
    Dirct=CalcDirection(VelocityComponents);
    VecOrigin=NewPosition;
    TimeStamp=NewTimeStamp;
}

void TypeVec::setTimeStamp (float ThisTime) {if (ThisTime>=0) TimeStamp=ThisTime; else TimeStamp=0;}

void TypeVec::setDirection (float Dir) { Dirct=CleanDir(Dir); }

//---GET methods-------------------------------------------------------------------------------------------
float TypeVec::getMagnitude () { return Magn; }

float TypeVec::getDirection () { return Dirct; }

float TypeVec::getTimeStamp () {return TimeStamp;}

float TypeVec::getRadsDirection () { return ToRad(Dirct); }

TypeCartesian TypeVec::getOrigin () { return VecOrigin; }

TypeCartesian TypeVec::getCartesianVel()
{
    TypeCartesian Result;
    Result.X=cos(ToRad(Dirct)) * Magn;
    Result.Y=sin(ToRad(Dirct)) * Magn;
    return Result;
}

TypeVec TypeVec::operator+ (TypeVec& ThatVec)
{
    TypeVec ResultVec;
    TypeCartesian ThisXYvel=getCartesianVel();
    TypeCartesian ThatXYvel=ThatVec.getCartesianVel();
    TypeCartesian NewXYvel;

    NewXYvel.X=ThisXYvel.X+ThatXYvel.X;
    NewXYvel.Y=ThisXYvel.Y+ThatXYvel.Y;

    ResultVec.setFromCartesian(NewXYvel, VecOrigin, TimeStamp);
    return ResultVec;
}

TypeVec TypeVec::operator- (TypeVec& ThatVec)
{
    TypeVec ResultVec;
    TypeCartesian ThisXYvel=getCartesianVel();
    TypeCartesian ThatXYvel=ThatVec.getCartesianVel();
    TypeCartesian NewXYvel;

    NewXYvel.X=ThisXYvel.X-ThatXYvel.X;
    NewXYvel.Y=ThisXYvel.Y-ThatXYvel.Y;

    ResultVec.setFromCartesian(NewXYvel, VecOrigin, TimeStamp);
    return ResultVec;
}
