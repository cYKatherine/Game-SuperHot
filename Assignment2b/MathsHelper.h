#ifndef MATHS_HELPER_H
#define MATHS_HELPER_H

#include <cstdlib>

#define PI 3.14159
#define ToRadians(degree) ((degree) * (PI / 180.0f))
#define ToDegrees(radian) ((radian * 180) / (PI))

class MathsHelper
{
public:
	static float RandomRange(float min, float max)
	{
		return min + (max - min) * ((float)rand() / RAND_MAX);
	}

	static int RandomRange(int min, int max)
	{
		// Includes min and max
		return rand() % (max - min + 1) + min;
	}

	static float RemapRange(float value, float fromMin, float fromMax, float toMin, float toMax)
	{
		// Example - remap 10 from range 0-20 to range 0-100. Result is 50
		return (((value - fromMin) * (toMax - toMin)) / (fromMax - fromMin)) + toMin;
	}

	// Amount of 0 will result in value1, amount of 1 will result in value 2
	static float LerpFloat(float value1, float value2, float amount)
	{
		return value1 + (value2 - value1) * amount;
	}

	static float Clamp(float value, float min, float max)
	{
		if (value > max)
			return max;
		else if (value < min)
			return min;
		else
			return value;
	}

};

#endif