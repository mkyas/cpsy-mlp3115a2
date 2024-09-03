
#include <iostream>
#include "MPL3115A2.hpp"

int main()
{
	MPL3115A2 sensor;

	std::cout << "Pressure " << sensor.pressure() << " hPa" << std::endl
		  << "Altitude " << sensor.altitude() << " m"
		  << "Temperature " << sensor.temperature() << " C"
		  << std::endl;
}
