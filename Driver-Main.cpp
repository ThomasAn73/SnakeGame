#include "ClassSnake.hpp"
#include "ClassProgressBar.hpp"
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <SFML/Graphics.hpp>

void GameStateReport(int GameState);
void AIupdate (sf::RenderWindow& ThisWindow, TypeSnake& GoodSnake, TypeSnake& BadSnake);

int main ()
{
    using namespace sf;
    using namespace std;

    int NumBuffer=0, UserInput=0;           //for the processing keyboard input
    int GameState=0;                        //0=in progress, 1=GameOver(won), -1=GameUnder(lost)
    Event Sensed;                           //Create an object of type "Event" and give it a name: "Sensed"
    Clock TheClock; float CurrentTime=0.0;
    TypeCartesian wSize={800,500};

    srand(time(NULL));

    RenderWindow MainWindow;                //Declare an sfml render window
    MainWindow.create(VideoMode(wSize.X,wSize.Y), "Snake Game");

    //Game objects --- these should be part of a GAME class (later)
    TypeProgressBar ProgressBar (150, 0,0, wSize.X, 5);
    TypeSnake GoodSnake(20,(wSize.X)/2, (wSize.Y)/2, 0,180,0); //No. of segments, positionX, posY, RGB
    TypeSnake BadSnake(0,50, 50, 108,0,0);                    //No. of segments, positionX, posY, RGB
    TypeQuizNavigation MathToMove; //MathToMove.ShowQuestion();
    TypeBouncy Leftovers;

    TheClock.restart();

    //Everything happens within this loop
    while (MainWindow.isOpen()==true || GameState==0)
    {
        CurrentTime=CurrentTime+0.0005;

        BadSnake.setHitState(false);
        AIupdate (MainWindow, GoodSnake, BadSnake);
        if(BadSnake.getHitState()==true) { GameState=2; break;}

        //Functions that display objects on screen
        MainWindow.clear();
        GoodSnake.Show(MainWindow, CurrentTime);
        BadSnake.Show(MainWindow, CurrentTime);
        MathToMove.ShowQuestion(MainWindow,GoodSnake);
        if(GoodSnake.getHitState()==true)
        {
            if(GoodSnake.getNumSegments()>0)
                {Leftovers.OneMoreBall(GoodSnake.DeleteSegment());}
            GoodSnake.setHitState(false);
        }
        Leftovers.Show(MainWindow, CurrentTime);
        ProgressBar.Show(MainWindow, CurrentTime);
        MainWindow.display();

        //Handle events
        while (MainWindow.pollEvent(Sensed)==true)
        {
            if (Sensed.type == Event::Closed) {MainWindow.close(); GameState=-2; break;}
            if (Sensed.type == Event::KeyPressed)
            {
                if (Sensed.key.code == Keyboard::Num0) {NumBuffer=NumBuffer*10+0; }
                else if (Sensed.key.code == Keyboard::Num1) {NumBuffer=NumBuffer*10+1; }
                else if (Sensed.key.code == Keyboard::Num2) {NumBuffer=NumBuffer*10+2; }
                else if (Sensed.key.code == Keyboard::Num3) {NumBuffer=NumBuffer*10+3; }
                else if (Sensed.key.code == Keyboard::Num4) {NumBuffer=NumBuffer*10+4; }
                else if (Sensed.key.code == Keyboard::Num5) {NumBuffer=NumBuffer*10+5; }
                else if (Sensed.key.code == Keyboard::Num6) {NumBuffer=NumBuffer*10+6; }
                else if (Sensed.key.code == Keyboard::Num7) {NumBuffer=NumBuffer*10+7; }
                else if (Sensed.key.code == Keyboard::Num8) {NumBuffer=NumBuffer*10+8; }
                else if (Sensed.key.code == Keyboard::Num9) {NumBuffer=NumBuffer*10+9; }
                else if (Sensed.key.code == Keyboard::Return)
                {
                    UserInput=NumBuffer;
                    NumBuffer=0;

                    if (MathToMove.IsValid(UserInput))
                    {
                        GoodSnake.Turn(MathToMove.Direction(UserInput));
                        MathToMove.Reset();
                        //MathToMove.ShowQuestion();
                    }
                    else
                    {
                        std::cout<<"You entered:"<<UserInput<<" <--- WRONG answer\n";
                        if(GoodSnake.getNumSegments()>0) Leftovers.OneMoreBall(GoodSnake.DeleteSegment());
                    }
                }
            }

            else if (Sensed.key.code == sf::Keyboard::Up) {GoodSnake.Turn(90);}
            else if (Sensed.key.code == sf::Keyboard::Down) {GoodSnake.Turn(-90);}
            else if (Sensed.key.code == sf::Keyboard::Left) {GoodSnake.Turn(180);}
            else if (Sensed.key.code == sf::Keyboard::Right) {GoodSnake.Turn(0);}
            else if (Sensed.key.code == sf::Keyboard::D) {GoodSnake.DeleteSegment();}
            else if (Sensed.key.code == sf::Keyboard::A) {GoodSnake.AddSegment();}
        }

        //Check for GameOver;
        if(GoodSnake.getNumSegments()==0) GameState=-1;
        else if(ProgressBar.getIsMaxed()==true) GameState=1;

        if(GameState!=0 && MainWindow.isOpen()==true ) MainWindow.close();

    }//end while isOpen

    GameStateReport(GameState);

    return 0;
}

void AIupdate (sf::RenderWindow& ThisWindow, TypeSnake& GoodSnake, TypeSnake& BadSnake)
{
    TypeVec GoodVel=GoodSnake.getHeadVelocity();
    TypeVec BadVel=BadSnake.getHeadVelocity();
    TypeVec DistVec=GoodVel.CalcVectorToPoint(BadVel);
    TypeCartesian GoodGuyLocation=GoodSnake.getHeadPixelPosition();
    sf::Vector2u wSize=ThisWindow.getSize();
    float PixDistance=BadVel.PixelDistance(DistVec.getMagnitude());
    float DirDelta=DistVec.getDirection()-GoodVel.getDirection();
    float CollisionDist=BadSnake.getHeadSize()+GoodSnake.getHeadSize();
    float DetectionDist=BadSnake.getHeadSize()*12;
    float SpeedFactor;
    int Sign=1;

    //DirDelta=GoodVel.CleanDir(DirDelta,false);
    SpeedFactor=0.8-0.4*PixDistance/(DetectionDist-CollisionDist);
    if (SpeedFactor<0.4) SpeedFactor=0.4;
    BadVel.setMagnitude(GoodVel.getMagnitude()*SpeedFactor);

    if (PixDistance<CollisionDist) { BadSnake.setHitState(true); return; }

    if (abs(DirDelta)>=90 && PixDistance>=DetectionDist) BadVel.setDirection(DistVec.OpositeDir());
    if (PixDistance<DetectionDist)
    {
        //Corner and window boundary avoidance spin
        if(GoodGuyLocation.X<GoodSnake.getHeadSize()*4 && GoodVel.getDirection()==90) { Sign=-1;}
        else if(GoodGuyLocation.X<GoodSnake.getHeadSize()*4 && GoodVel.getDirection()==270) { Sign=1;}
        else if(GoodGuyLocation.X>wSize.x-GoodSnake.getHeadSize()*4 && GoodVel.getDirection()==90) { Sign=1;}
        else if(GoodGuyLocation.X>wSize.x-GoodSnake.getHeadSize()*4 && GoodVel.getDirection()==270) { Sign=-1;}

        else if(GoodGuyLocation.Y<GoodSnake.getHeadSize()*4 && GoodVel.getDirection()==0) { Sign=-1;}
        else if(GoodGuyLocation.Y<GoodSnake.getHeadSize()*4 && GoodVel.getDirection()==180) { Sign=1;}
        else if(GoodGuyLocation.Y>wSize.y-GoodSnake.getHeadSize()*4 && GoodVel.getDirection()==0) { Sign=1; }
        else if(GoodGuyLocation.Y>wSize.y-GoodSnake.getHeadSize()*4 && GoodVel.getDirection()==180) { Sign=-1;}

        else if(DirDelta<0) Sign=-1; else Sign=1;

        if (abs(DirDelta)<90) {BadVel.setDirection(DistVec.getDirection()+45*Sign);}
    }

    BadSnake.setHeadVelocity(BadVel);
}

void GameStateReport(int GameState)
{
    switch (GameState)
    {
    case 0:
        std::cout<<"--Still Playing--\n"; break;
    case -1:
        std::cout<<"--GAME OVER-- (You lost)\n"; break;
    case -2:
        std::cout<<"--Game was Aborted\n"; break;
    case 1:
        std::cout<<"--GAME OVER-- (You Survived)\n"; break;
    case 2:
        std::cout<<"--GAME OVER-- (You captured the Red Guy)\n"; break;
    default:
        std::cout<<"--No idea what you are doing--\n";
    }
}
