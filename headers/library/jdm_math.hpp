#pragma once
#include "../higherObject.hpp"

class JDMMath 
{
public:
    enum MathLibrary {
        Addition,
        Subtraction,
        Multiplication,
        Division,
        Exponentiation,
        SquareRoot,
        AbsoluteValue,
        Factorial,
        Sin,
        Cos,
        Tangent,
        ASin,
        ACos,
        Atan,
        Atan2,
        Logarithms,
        Ceiling,
        Floor,
        GreatestCommonDivisor,
        LeastCommonMultiple,
        RandomNumberGeneration,
        PrimeNumberCheck,
        Rounding,
        DegreesToRadians,
        RadiansToDegrees
    };
    static std::unordered_map<std::string, MathLibrary> mathLibrary ;

public:
	static const std::shared_ptr<HigherObject> add() { return nullptr; }
	static const std::shared_ptr<HigherObject> sub() { return nullptr; }
	static const std::shared_ptr<HigherObject> mul() { return nullptr; }
	static const std::shared_ptr<HigherObject> div() { return nullptr; }
	static const std::shared_ptr<HigherObject> pow() { return nullptr; }
	static const std::shared_ptr<HigherObject> sqrt() { return nullptr; }
	static const std::shared_ptr<HigherObject> abs() { return nullptr; }
	static const std::shared_ptr<HigherObject> factorial() { return nullptr; }
	static const std::shared_ptr<HigherObject> sin() { return nullptr; }
	static const std::shared_ptr<HigherObject> cos() { return nullptr; }
	static const std::shared_ptr<HigherObject> tan() { return nullptr; }
	static const std::shared_ptr<HigherObject> asin() { return nullptr; }
	static const std::shared_ptr<HigherObject> acos() { return nullptr; }
	static const std::shared_ptr<HigherObject> atan() { return nullptr; }
	static const std::shared_ptr<HigherObject> atan2() { return nullptr; }
	static const std::shared_ptr<HigherObject> log() { return nullptr; }
	static const std::shared_ptr<HigherObject> ceil() { return nullptr; }
	static const std::shared_ptr<HigherObject> floor() { return nullptr; }
	static const std::shared_ptr<HigherObject> gcd() { return nullptr; }
	static const std::shared_ptr<HigherObject> lcm() { return nullptr; }
	static const std::shared_ptr<HigherObject> random() { return nullptr; }
	static const std::shared_ptr<HigherObject> isPrime() { return nullptr; }
	static const std::shared_ptr<HigherObject> round() { return nullptr; }
	static const std::shared_ptr<HigherObject> degToRad() { return nullptr; }
	static const std::shared_ptr<HigherObject> radToDeg() { return nullptr; }
};

std::unordered_map<std::string, JDMMath::MathLibrary> JDMMath::mathLibrary = {
    {"add", Addition},
    {"sub", Subtraction},
    {"mul", Multiplication},
    {"div", Division},
    {"pow", Exponentiation},
    {"sqrt", SquareRoot},
    {"abs", AbsoluteValue},
    {"factorial", Factorial},
    {"sin", Sin},
    {"cos", Cos},
    {"tan", Tangent},
    {"asin", ASin},
    {"acos", ACos},
    {"atan", Atan},
    {"atan2", Atan2},
    {"log", Logarithms},
    {"ceil", Ceiling},
    {"floor", Floor},
    {"gcd", GreatestCommonDivisor},
    {"lcm", LeastCommonMultiple},
    {"random", RandomNumberGeneration},
    {"isPrime", PrimeNumberCheck},
    {"round", Rounding},
    {"degToRad", DegreesToRadians},
    {"radToDeg", RadiansToDegrees}
};