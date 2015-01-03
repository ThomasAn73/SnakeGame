#ifndef ProgressBar_0101110_hpp
#define ProgressBar_0101110_hpp

#include <SFML/Graphics.hpp>
#include "ClassVector.hpp"

class TypeProgressBar
{
private:
    float MaxDuration;      //seconds
    float Progress;         //Percent complete

    bool IsMaxed=false;

    TypeCartesian Dimensions;
    TypeCartesian Position;

    sf::RectangleShape BackgroundShape;
    TypeRGB BackFillColor;
    TypeRGB BackOutlineColor;

    sf::RectangleShape ForegroundShape;
    TypeRGB ForeFillColor;
    TypeRGB ForeOutlineColor;

    //Private methods;
    void setProgress (float TimeLapsed);

public:
    //Constractors
    TypeProgressBar ();
    TypeProgressBar (float MaxTime, float PosX, float PosY, float Length, float width);

    //Methods
    float getProgress ();
    bool  getIsMaxed ();

    void setMaxDuration (float ThisMuch);
    void setDimensions (TypeCartesian BarSize);
    void setDimensions (float xAxis, float Yaxis);
    void setPosition (float xAxis, float Yaxis);
    void setBackFillColor (unsigned short R, unsigned short G, unsigned short B);
    void setBackOutlineColor (unsigned short R, unsigned short G, unsigned short B);
    void setForeFillColor (unsigned short R, unsigned short G, unsigned short B);
    void setForeOutlineColor (unsigned short R, unsigned short G, unsigned short B);

    void Show (sf::RenderWindow& ThisWindow, float CurrentTime);
};

#endif // ProgressBar_0101110_hpp
