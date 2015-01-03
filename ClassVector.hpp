#ifndef ClassVector_0101110_h
#define ClassVector_0101110_h

struct TypeCartesian { float X; float Y; };
struct TypeRGB { int R; int G; int B; };

class TypeVec
{
private:
    //Private variables
    float Magn=0;
    float Dirct=0;                  //in degrees
    float TimeStamp=0.0;            //since a force last acted on the object
    TypeCartesian VecOrigin={0,0};  //position of vector origin

    //Private Methods
    float ToRad (float Deg);
    float ToDeg (float Rad);

public:
    //Consrt/Destr
    TypeVec();
    TypeVec (float Mag, float Dir, float Xpos, float Ypos, float NewTimeStamp=0.0);
    ~TypeVec();

    //METHODS
    float PixelDistance (float MeterDist);
    float MeterDistance (float PixelDist);
    float ReflectionAngle (float PlaneAngle);
    float OpositeDir ();
    float CleanDir (float ThisDir, bool isPlane=false);
    float CalcDirection (TypeCartesian Component);
    float CalcDirectionToPoint (TypeCartesian DestPoint);
    float CalcDistanceToPoint (TypeCartesian DestPoint);
    TypeVec CalcVectorToPoint (TypeCartesian DestPoint);
    TypeVec CalcVectorToPoint (TypeVec DestVec);

    //SET methods
    void setMagnitude (float Mag);
    void setDirection (float Dir);
    void setFromCartesian (TypeCartesian NewVelocity, TypeCartesian NewPosition, float atTime=0.0);
    void setTimeStamp (float ThisTime);
    void setOrigin (TypeCartesian Origin);
    void setOrigin (float Xpos, float Ypos);

    //GET methods
    float getMagnitude ();
    float getDirection ();
    float getRadsDirection ();
    float getTimeStamp ();
    TypeCartesian getCartesianVel();
    TypeCartesian getOrigin ();

    //Operator overloading methods -> for vector arithmitic
    TypeVec operator+ (TypeVec& ThatVec);
    TypeVec operator- (TypeVec& ThatVec);
    //bool operator> (TypeVec& ThisVec);
    //bool operator< (TypeVec& ThisVec);
    //bool operator== (TypeVec& ThisVec);
    //TypeVec operator* (TypeVec& ThisVec); //Dot product I suppose
};

#endif
