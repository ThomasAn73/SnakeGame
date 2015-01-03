#include "ClassProgressBar.hpp"

//***TYPEPROGRESSBAR****************************************************************************************************
//**********************************************************************************************************************

//---Constructors-------------------------------------------------------------------------------------------------------
TypeProgressBar::TypeProgressBar()
{
    MaxDuration=60;
    Progress=0.0;

    Position.X=0; Position.Y=0;
    Dimensions.X=100; Dimensions.Y=10;

    setBackFillColor(40,40,40); setBackOutlineColor(255,255,255);
    setForeFillColor(180,180,0); setForeOutlineColor(255,255,0);
}

TypeProgressBar::TypeProgressBar(float MaxTime, float PosX, float PosY, float Length, float Width)
{
    if(MaxTime<=0) {MaxTime=60;} MaxDuration=MaxTime;

    Progress=0.0;

    setPosition(PosX, PosY);
    setDimensions(Length,Width);

    setBackFillColor(40,40,40); setBackOutlineColor(255,255,255);
    setForeFillColor(180,180,0); setForeOutlineColor(255,255,0);
}

//---METHODS-------------------------------------------------------------------------------------------------------------

bool  TypeProgressBar::getIsMaxed () {return IsMaxed;}

float TypeProgressBar::getProgress () {return Progress;}

void TypeProgressBar::setProgress (float TimeLapsed)
{
    if (TimeLapsed<0) TimeLapsed=0;
    else if (TimeLapsed>=MaxDuration) {TimeLapsed=MaxDuration; IsMaxed=true;}
    Progress=TimeLapsed/MaxDuration;

    sf::Vector2f sfSize=BackgroundShape.getSize(); //This way you get the total possible length
    sfSize.x=sfSize.x*Progress;

    ForegroundShape.setSize(sfSize);
}

void TypeProgressBar::setPosition (float xAxis, float yAxis)
{
    Position.X=xAxis; Position.Y=yAxis;
    BackgroundShape.setPosition(xAxis,yAxis);
    ForegroundShape.setPosition(xAxis,yAxis);
}

void TypeProgressBar::setDimensions (float Length, float Width)
{
    if (Length<0) {Length=100;} if (Width<0) Width=10;
    Dimensions.X=Length; Dimensions.Y=Width;

    sf::Vector2f sfSize; sfSize.x=Length; sfSize.y=Width;
    BackgroundShape.setSize(sfSize);

    sfSize.x=2; sfSize.y=Width;
    ForegroundShape.setSize(sfSize);
}

void TypeProgressBar::Show (sf::RenderWindow& ThisWindow, float CurrentTime)
{
    setProgress(CurrentTime);

    ThisWindow.draw(BackgroundShape);
    ThisWindow.draw(ForegroundShape);
}

void TypeProgressBar::setBackFillColor (unsigned short R, unsigned short G, unsigned short B)
{
    BackFillColor.R=R; BackFillColor.G=G; BackFillColor.B=B;
    BackgroundShape.setFillColor(sf::Color(R,G,B));
}

void TypeProgressBar::setBackOutlineColor (unsigned short R, unsigned short G, unsigned short B)
{
    BackOutlineColor.R=R; BackOutlineColor.G=G; BackOutlineColor.B=B;
    BackgroundShape.setOutlineColor(sf::Color(R,G,B));
}

void TypeProgressBar::setForeFillColor (unsigned short R, unsigned short G, unsigned short B)
{
    ForeFillColor.R=R; ForeFillColor.G=G; ForeFillColor.B=B;
    ForegroundShape.setFillColor(sf::Color(R,G,B));
}

void TypeProgressBar::setForeOutlineColor (unsigned short R, unsigned short G, unsigned short B)
{
    ForeOutlineColor.R=R; ForeOutlineColor.G=G; ForeOutlineColor.B=B;
    ForegroundShape.setOutlineColor(sf::Color(R,G,B));
}
