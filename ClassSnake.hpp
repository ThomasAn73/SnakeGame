#ifndef SnakeClass_0101110_h
#define SnakeClass_0101110_h

#include <SFML/Graphics.hpp>
#include "ClassBall.hpp"
#include <vector>
#include <string>

class TypeSnake
{
private:
    int NumSegments=0;               //Number of segments for the snake
    int MaxSegments=100;

    float HeadSize=15;
    float SegmentSize=10;
    float DefaultSpeed=0.5;

    bool TookHit=false;

    TypeBall SnakeHead;
    std::vector <TypeVec> MovesQueue;     //This queue stores changes in direction of the head, so the tail segments can follow
    std::vector <TypeBall> SegmentQueue;  //This queue holds the tail segments

    //Private Methods
    void UpdatePosition (TypeBall& ThisBall, float CurrentTime);
    void UpdateTailPosition (float CurrentTime);
    void UpdateTailPositionSmooth (float CurrentTime);

public:
    //Constructors
    TypeSnake ();
    TypeSnake (int Segments, float PosX, float PosY, unsigned short R=255,unsigned short G=255,unsigned short B=255);
    ~TypeSnake ();

    //Methods
    bool  getHitState ();
    int   getNumSegments ();
    float getHeadSize ();
    TypeCartesian getHeadPixelPosition ();
    TypeVec getHeadVelocity();

    void setFillColor (unsigned short R,unsigned short G,unsigned short B);
    void setHeadVelocity (TypeVec NewVelocityVec);

    void AddSegment ();
    void Turn (float Dir);
    void setHitState (bool IsHit);
    void Show (sf::RenderWindow& ThisWindow, float CurrentTime);
    float PixelDistance (float MeterDist);
    TypeBall DeleteSegment ();

};

//***Another Class******************************************************************************************

class TypeQuizNavigation
{
private:
    struct TypeThreeNum { int a; int b; int c; };

    TypeThreeNum Left, Up, Down, Right;
    int MaxNum=20;
    unsigned DefaultFontSize=14;

    int UniqueNum();

    sf::Font TextFont;
    sf::Text LeftMath;
    sf::Text RightMath;
    sf::Text UpMath;
    sf::Text DownMath;

public:
    //Constructor
    TypeQuizNavigation();

    void Reset ();
    void ShowQuestion();
    void ShowQuestion(sf::RenderWindow& ThisWindow, TypeSnake ThisSnake);
    void setTextFont(std::string fontFile, unsigned fontSize);
    bool IsValid (int ThisAnswer);
    float Direction (int ThisAnswer);

};

//***And another**********************************************************************************************
//I need to make separate files for these classes

class TypeBouncy
{
private:
    int Population=0;
    std::vector <TypeBall> Balls;
public:
    void OneMoreBall (TypeBall OneBall);
    void Show (sf::RenderWindow& ThisWindow, float CurrentTime);
};

#endif // SnakeClass_0101110_h
