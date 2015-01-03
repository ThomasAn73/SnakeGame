#include "ClassSnake.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <sstream>

void Gravity (TypeBall& ThisBall, float currentTime);
bool CheckBoundCollision (TypeBall& ThisBall, TypeCartesian BoxSize, int ObjSize=0, float Elast=1);
void TextColor (bool isRed, bool isGreen, bool isBlue, bool isBright=false);

//***TYPESNAKE class implementation************************************************************
//*********************************************************************************************

//---Constructors------------------------------------------------------------------------------
TypeSnake::TypeSnake ()
{
    NumSegments=0;
    SnakeHead.setRadius(HeadSize);
    SnakeHead.setSides(HeadSize);
    SnakeHead.setFillColor(0,180,0);
    SnakeHead.setOutlineColor(0,255,0);
}

TypeSnake::TypeSnake (int Segments, float PosX, float PosY, unsigned short R,unsigned short G,unsigned short B)
{
    if (Segments<0)Segments=0; else if(Segments>MaxSegments) Segments=MaxSegments;

    SnakeHead.setRadius(HeadSize);
    SnakeHead.setSides(HeadSize);
    SnakeHead.setFillColor(R,G,B);
    SnakeHead.setOutlineColor(0,255,0);

    TypeCartesian PixPos={PosX,PosY};
    SnakeHead.setPixelPosition(PixPos);

    for (int i=0;i<Segments;i++) { AddSegment();}
}

TypeSnake::~TypeSnake ()
{
    //Nothing to do really. We don't have any calls to new
}

//---METHODS-------------------------------------------------------------------------------------
void TypeSnake::AddSegment ()
{
    if (NumSegments>=MaxSegments) return;

    TypeBall OneSeg;
    OneSeg.setRadius(SegmentSize);
    OneSeg.setSides(SegmentSize);
    OneSeg.setFillColor(0,80,0);
    OneSeg.setOutlineColor(0,255,0);

    if (NumSegments==0) OneSeg.setPixelPosition(SnakeHead.getPixelPosition());
    else OneSeg.setPixelPosition(SegmentQueue[NumSegments-1].getPixelPosition());

    SegmentQueue.push_back(OneSeg);
    NumSegments=SegmentQueue.size();
}

TypeBall TypeSnake::DeleteSegment ()
{
    TypeBall ToDelete;

    if (NumSegments==0) return ToDelete;

    ToDelete=SegmentQueue[NumSegments-1];

    SegmentQueue.erase(SegmentQueue.end());
    NumSegments = SegmentQueue.size();

    return ToDelete;
}

void TypeSnake::Turn (float Dir)
{
    TypeVec Velocity=SnakeHead.getVelocity();
    float OldDir=Velocity.getDirection();

    Velocity.setDirection(Dir);
    if(Velocity.getMagnitude()>0.0 && Velocity.getDirection()==OldDir) return;
    if(Velocity.getMagnitude()==0.0) Velocity.setMagnitude(DefaultSpeed);

    SnakeHead.setVelocity(Velocity);

    // Update the moves queue
    Velocity.setTimeStamp(0.0);
    MovesQueue.push_back(Velocity);
    //std::cout<<MovesQueue.size();

}

void TypeSnake::UpdatePosition (TypeBall& ThisBall, float CurrentTime)
{
    TypeVec OldVelocity=ThisBall.getVelocity();
    TypeCartesian NewPos, NewVel, OldPos;

    float TimeDif=CurrentTime-OldVelocity.getTimeStamp();
    float InitSpeed=OldVelocity.getMagnitude();
    float InitRadsDir=OldVelocity.getRadsDirection();
    float InitDir=OldVelocity.getDirection();

    OldPos=OldVelocity.getOrigin();

    //This is in meters.
    NewPos.X=OldPos.X+(InitSpeed*cos(InitRadsDir))*TimeDif;
    NewPos.Y=OldPos.Y+(InitSpeed*sin(InitRadsDir))*TimeDif;

    NewVel.X=(InitSpeed*cos(InitRadsDir));
    NewVel.Y=(InitSpeed*sin(InitRadsDir));

    if(InitDir==90 || InitDir==270) NewVel.X=0.0;
    else if(InitDir==0 || InitDir==180) NewVel.Y=0.0;

    ThisBall.setFromCartesian(NewVel,NewPos,CurrentTime);
}

void TypeSnake::Show (sf::RenderWindow& ThisWindow, float CurrentTime)
{
    TypeCartesian BoxSize;
    BoxSize.X=ThisWindow.getSize().x;
    BoxSize.Y=ThisWindow.getSize().y;

    //Update head position
    UpdatePosition (SnakeHead, CurrentTime);
    TookHit=CheckBoundCollision(SnakeHead, BoxSize, HeadSize*2);
    ThisWindow.draw(SnakeHead.getSFMLcircle());

    //Update tail position
    UpdateTailPositionSmooth(CurrentTime);
    //UpdateTailPosition(CurrentTime);
    for (int i=0;i<NumSegments;i++) { ThisWindow.draw(SegmentQueue[i].getSFMLcircle()); }
}

void TypeSnake::setHeadVelocity (TypeVec NewVelocityVec) { SnakeHead.setVelocity (NewVelocityVec); }

void TypeSnake::setFillColor (unsigned short R,unsigned short G,unsigned short B) { SnakeHead.setFillColor (R,G,B); }

bool TypeSnake::getHitState () { return TookHit; }

void TypeSnake::setHitState (bool isHit) {TookHit=isHit;}

void TypeSnake::UpdateTailPosition (float CurrentTime)
{
    //The snakeHead leaves a trail of breadcrumps  --> the movesQueue
    //Each time a segment reaches its target breadcrump it increments the Vec timeStamp
    //As such, segment[0] is looking for breadcrump with a zero timestamp
    //segment[1] looks for breadcrump with a one timestamp, etc, etc
//std::cout<<"Moves in queue="<<MovesQueue.size()<<"\n";
    //Iterate through each tail segment
    for (int i=0;i<NumSegments;i++)
    {
        UpdatePosition(SegmentQueue[i],CurrentTime);
        //Which breadcrump should this particular segment follow ?
        TypeVec BreadCrump, follower, leader, DistToCrumpVec, DistToLeaderVec;
        float foundCrump=-1, PixDistToCrump, PixDistToLeader;
        for (unsigned k=0;k<MovesQueue.size();k++)
        {//std::cout<<"move...";
            float CrumpAge=MovesQueue[k].getTimeStamp(); //std::cout<<CrumpAge<<".";
            if(CrumpAge<=i) {foundCrump=k; BreadCrump=MovesQueue[k]; break;}
        }
        if (foundCrump==-1) {BreadCrump=SnakeHead.getVelocity();}

        if (i==0) {leader=SnakeHead.getVelocity(); } else {leader = SegmentQueue[i-1].getVelocity(); }

        follower=SegmentQueue[i].getVelocity();

        DistToCrumpVec=follower.CalcVectorToPoint(BreadCrump);
        PixDistToCrump=PixelDistance(DistToCrumpVec.getMagnitude());

        DistToLeaderVec=follower.CalcVectorToPoint(leader);
        PixDistToLeader=PixelDistance(DistToLeaderVec.getMagnitude());

        follower.setMagnitude(leader.getMagnitude());
        follower.setDirection(DistToCrumpVec.getDirection());

        if (PixDistToCrump <SegmentSize/10)
        {

            if (PixDistToLeader < SegmentSize*2 && i==0) {break;}
                //Follow the direction encoded in the crump
            follower.setMagnitude(BreadCrump.getMagnitude());
            follower.setDirection(BreadCrump.getDirection());

            if(foundCrump>-1) MovesQueue[foundCrump].setTimeStamp(i+1.0);
            if(i==NumSegments-1 && MovesQueue.size()>0) MovesQueue.erase(MovesQueue.begin());
        }

        //speed up or slow down if too close or too far from the leader segment
        if (PixDistToLeader<SegmentSize*1.5) follower.setMagnitude(leader.getMagnitude()*0.7);
        if (PixDistToLeader>SegmentSize*2.2)follower.setMagnitude(leader.getMagnitude()*1.5);

        SegmentQueue[i].setVelocity(follower);
    }
}

void TypeSnake::UpdateTailPositionSmooth (float CurrentTime)
{
    if (MovesQueue.size()>0) MovesQueue.clear();
    //Iterate through each tail segment
    for (int i=0;i<NumSegments;i++)
    {
        UpdatePosition(SegmentQueue[i],CurrentTime);

        TypeVec follower, leader, DistToLeaderVec;
        float PixDistToLeader;

        if (i==0) {leader=SnakeHead.getVelocity(); } else {leader = SegmentQueue[i-1].getVelocity(); }

        follower=SegmentQueue[i].getVelocity();

        DistToLeaderVec=follower.CalcVectorToPoint(leader);
        PixDistToLeader=PixelDistance(DistToLeaderVec.getMagnitude());

        follower.setMagnitude(leader.getMagnitude());
        follower.setDirection(DistToLeaderVec.getDirection());

        //speed up or slow down if too close or too far from the leader segment
        if (PixDistToLeader<SegmentSize*1.8) follower.setMagnitude(leader.getMagnitude()*0.7);
        if(PixDistToLeader>SegmentSize*2.2)follower.setMagnitude(leader.getMagnitude()*1.5);

        SegmentQueue[i].setVelocity(follower);
    }
}

float TypeSnake::getHeadSize () { return HeadSize; }

TypeVec TypeSnake::getHeadVelocity() { return SnakeHead.getVelocity(); }

TypeCartesian TypeSnake::getHeadPixelPosition () { return SnakeHead.getPixelPosition(); }

int  TypeSnake::getNumSegments () { return NumSegments; }

float TypeSnake::PixelDistance (float MeterDist)
{ const float PixPerMeter=5*(100/2.54); return MeterDist*PixPerMeter; }

//***TYPEQUIZNAV class implementation****************************************************************************
//***************************************************************************************************************

//---Contructor--------------------------------------------------------------------------------------------------
TypeQuizNavigation::TypeQuizNavigation() { Reset(); setTextFont ("Arial.ttf", DefaultFontSize);}

//---METHODS-----------------------------------------------------------------------------------------------------
void TypeQuizNavigation::Reset ()
{
    //Generate four unique results
    Left.c=UniqueNum();
    Up.c=UniqueNum();
    Down.c=UniqueNum();
    Right.c=UniqueNum();

    //Generate first operant
    Left.a=rand()%(Left.c);
    Up.a=rand()%(Up.c);
    Down.a=rand()%(Down.c);
    Right.a=rand()%(Right.c);

    //Generate second operant
    Left.b=Left.c-Left.a;
    Up.b=Up.c-Up.a;
    Down.b=Down.c-Down.a;
    Right.b=Right.c-Right.a;

    std::stringstream TempStr;
    TempStr<<Left.a<<"+"<<Left.b<<"=?"; LeftMath.setString(TempStr.str()); TempStr.str("");
    TempStr<<Right.a<<"+"<<Right.b<<"=?"; RightMath.setString(TempStr.str()); TempStr.str("");
    TempStr<<Up.a<<"+"<<Up.b<<"=?"; UpMath.setString(TempStr.str()); TempStr.str("");
    TempStr<<Down.a<<"+"<<Down.b<<"=?"; DownMath.setString(TempStr.str()); TempStr.str("");

}

int TypeQuizNavigation::UniqueNum()
{
    srand(time(NULL));
    int result;

    do{ result=rand()%(MaxNum-1)+1;}
    while (Left.c==result || Up.c==result || Down.c==result || Right.c==result);

    return result;
}

bool TypeQuizNavigation::IsValid (int ThisAnswer)
{
    if (ThisAnswer==Left.c || ThisAnswer==Up.c || ThisAnswer==Down.c || ThisAnswer==Right.c) {return true;} else {return false;}
}

float TypeQuizNavigation::Direction (int ThisAnswer)
{
    if(IsValid(ThisAnswer)==false) return 45; //Diagonal is not a valid direction, as a hint that the answer was wrong

    if(ThisAnswer==Left.c) return 180;
    if(ThisAnswer==Up.c) return 90;
    if(ThisAnswer==Down.c) return -90;
    if(ThisAnswer==Right.c) return 0;

    return 0;
}

void TypeQuizNavigation::setTextFont(std::string fontFile, unsigned fontSize)
{
    TextFont.loadFromFile(fontFile);

    LeftMath.setFont(TextFont);
    RightMath.setFont(TextFont);
    UpMath.setFont(TextFont);
    DownMath.setFont(TextFont);

    LeftMath.setCharacterSize(fontSize);
    RightMath.setCharacterSize(fontSize);
    UpMath.setCharacterSize(fontSize);
    DownMath.setCharacterSize(fontSize);
}

void TypeQuizNavigation::ShowQuestion(sf::RenderWindow& ThisWindow, TypeSnake ThisSnake)
{
    TypeCartesian TextPos;
    sf::FloatRect TextRect;

    TextRect = LeftMath.getLocalBounds();  TextPos=ThisSnake.getHeadPixelPosition();
    TextPos.X=TextPos.X-TextRect.width-ThisSnake.getHeadSize()-5;
    TextPos.Y=TextPos.Y-TextRect.height/2;
    LeftMath.setPosition(TextPos.X,TextPos.Y);

    TextRect = RightMath.getLocalBounds();  TextPos=ThisSnake.getHeadPixelPosition();
    TextPos.X=TextPos.X+ThisSnake.getHeadSize()+5;
    TextPos.Y=TextPos.Y-TextRect.height/2;
    RightMath.setPosition(TextPos.X,TextPos.Y);

    TextRect = UpMath.getLocalBounds();  TextPos=ThisSnake.getHeadPixelPosition();
    TextPos.X=TextPos.X-TextRect.width/2;
    TextPos.Y=TextPos.Y-TextRect.height-ThisSnake.getHeadSize()-10;
    UpMath.setPosition(TextPos.X,TextPos.Y);

    TextRect = DownMath.getLocalBounds();  TextPos=ThisSnake.getHeadPixelPosition();
    TextPos.X=TextPos.X-TextRect.width/2;
    TextPos.Y=TextPos.Y+ThisSnake.getHeadSize()+5;
    DownMath.setPosition(TextPos.X,TextPos.Y);

    ThisWindow.draw(LeftMath);
    ThisWindow.draw(RightMath);
    ThisWindow.draw(UpMath);
    ThisWindow.draw(DownMath);
}


void TypeQuizNavigation::ShowQuestion()
{
    std::cout<<"\nAnswer one of the following to navigate (then press Return):\n";
    TextColor(true,true,false,true); std::cout<<"UP: "<<Up.a<<"+"<<Up.b<<"=?"<<"\n";
    TextColor(true,true,true,true); std::cout<<"LEFT: "<<Left.a<<"+"<<Left.b<<"=?"<<"  --  ";
    TextColor(false,true,false,true); std::cout<<"RIGHT: "<<Right.a<<"+"<<Right.b<<"=?"<<"\n";
    TextColor(true,false,false,true); std::cout<<"DOWN: "<<Down.a<<"+"<<Down.b<<"=?"<<"\n";
    TextColor(true,true,true);
}

//***TYPEBOUNCY*********************************************************************************************************
//**********************************************************************************************************************

void TypeBouncy::OneMoreBall (TypeBall OneBall)
{
    Balls.push_back(OneBall);
    Population=Balls.size();
}

void TypeBouncy::Show (sf::RenderWindow& ThisWindow, float CurrentTime)
{
    if (Population==0) return;

    TypeCartesian wSize;
    wSize.X=ThisWindow.getSize().x;
    wSize.Y=ThisWindow.getSize().y;

    for (int i=0;i<Population;i++)
    {
        Gravity(Balls[i], CurrentTime);
        CheckBoundCollision (Balls[i], wSize, Balls[i].getRadius()*2, 0.9);
        ThisWindow.draw(Balls[i].getSFMLcircle());
    }
}

//***These functions do not have a class container yet. They are just loose ********************************************
//**********************************************************************************************************************

bool CheckBoundCollision (TypeBall& ThisBall, TypeCartesian BoxSize, int ObjSize, float Elast)
{
    bool Collided=false;
    TypeCartesian BallPos=ThisBall.getPixelPosition();
    TypeVec BallVelocity=ThisBall.getVelocity();
    float Speed=BallVelocity.getMagnitude();
    float ReflectedDir;

    if(BallPos.Y>BoxSize.Y-ObjSize/2)
    {
        BallPos.Y=BoxSize.Y-ObjSize/2;
        ReflectedDir=BallVelocity.ReflectionAngle(0.0);
        Collided=true;
    }
    else if (BallPos.Y<ObjSize/2)
    {
        BallPos.Y=ObjSize/2;
        ReflectedDir=BallVelocity.ReflectionAngle(180.0);
        Collided=true;
    }

    if (BallPos.X>BoxSize.X-ObjSize/2)
    {
        BallPos.X=BoxSize.X-ObjSize/2;
        ReflectedDir=BallVelocity.ReflectionAngle(90.0);
        Collided=true;
    }
    else if (BallPos.X<ObjSize/2)
    {
        BallPos.X=ObjSize/2;
        ReflectedDir=BallVelocity.ReflectionAngle(-90.0);
        Collided=true;
    }

    if(Collided==true)
    {
        BallVelocity.setDirection(ReflectedDir);
        BallVelocity.setMagnitude(Speed*Elast);
        ThisBall.setVelocity(BallVelocity);
        ThisBall.setPixelPosition(BallPos);
    }

    return Collided;
}

void Gravity (TypeBall& ThisBall, float CurrentTime)
{
    const float g=9.8;

    TypeVec OldVelocity=ThisBall.getVelocity();
    TypeCartesian NewPos, NewVel, OldPos;

    float TimeDif=CurrentTime-OldVelocity.getTimeStamp();
    float InitSpeed=OldVelocity.getMagnitude();
    float InitDir=OldVelocity.getRadsDirection();

    OldPos=OldVelocity.getOrigin();

    //This is in meters.
    NewPos.X=OldPos.X+(InitSpeed*cos(InitDir))*TimeDif;
    NewPos.Y=OldPos.Y+(InitSpeed*sin(InitDir))*TimeDif-0.5*g*TimeDif*TimeDif;

    NewVel.X=(InitSpeed*cos(InitDir));
    NewVel.Y=(InitSpeed*sin(InitDir))-g*TimeDif;

    ThisBall.setFromCartesian(NewVel,NewPos,CurrentTime);
}

void TextColor (bool isRed, bool isGreen, bool isBlue, bool isBright)
{
    HANDLE hConsole; hConsole = GetStdHandle (STD_OUTPUT_HANDLE);
    WORD Green = FOREGROUND_GREEN, Blue = FOREGROUND_BLUE, Red = FOREGROUND_RED, Bright = FOREGROUND_INTENSITY;

    WORD FinalColor=0;
    if(isRed) FinalColor=FinalColor+Red;
    if(isGreen) FinalColor=FinalColor+Green;
    if(isBlue) FinalColor=FinalColor+Blue;
    if(isBright) FinalColor=FinalColor+Bright;

    // Use the three primary colors for mixing any other color.
    // Use FOREGROUND_INTENSITY for brighter colors.
    SetConsoleTextAttribute (hConsole, FinalColor);

}
