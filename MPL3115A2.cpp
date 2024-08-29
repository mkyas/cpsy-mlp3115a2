
#include <ctime>

extern "C" {
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
}
#include <sys/ioctl.h>

#include <fcntl.h>
#include <unistd.h>

#include <cstring>
#include <stdexcept>

#include "MPL3115A2.hpp"


static const timespec centisec = { 0, 10000000 };


MPL3115A2::MPL3115A2(int smbus, std::uint8_t address) : smbus(0), address(address)
{
	char name[20];
	snprintf(name, sizeof(name) - 1, "/dev/i2c-%d", smbus);
	this->smbus = open(name, O_RDWR);
	if (this->smbus < 0) {
		// Exceptions are usually avoided or forbidden in embedded code.
		// We like to avoid some ideoms from embedded programming
		// to realise the RAII principle.
		throw std::runtime_error("Device does not exist");
	}

	if (ioctl(this->smbus, I2C_SLAVE, address) < 0) {
		throw std::runtime_error("Address not found");
	}
}

MPL3115A2::~MPL3115A2()
{
	close(this->smbus);
}



void MPL3115A2::_set_mode(std::uint8_t mode)
{
	while (i2c_smbus_read_byte_data(this->smbus, MPL3115A2::CTRL_REG1_OST) < 0) {
		nanosleep(&centisec, nullptr);
	}
	this->_ctrl_reg1.bit.ALT = mode;
	i2c_smbus_write_byte_data(this->smbus, MPL3115A2::CTRL_REG1, this->_ctrl_reg1.reg);
}

void MPL3115A2::_one_shot(void)
{
	this->_ctrl_reg1.reg = i2c_smbus_read_byte_data(this->smbus, MPL3115A2::REGISTER_STATUS);
	while (this->_ctrl_reg1.bit.OST) {
		nanosleep(&centisec, nullptr);
		this->_ctrl_reg1.reg = i2c_smbus_read_byte_data(this->smbus, MPL3115A2::REGISTER_STATUS);
	}
	this->_ctrl_reg1.bit.OST = 1;
	i2c_smbus_write_byte_data(this->smbus, MPL3115A2::CTRL_REG1, this->_ctrl_reg1.reg);
}

void MPL3115A2::_await_completion(void)
{
	while (0 != (i2c_smbus_read_byte_data(this->smbus, MPL3115A2::REGISTER_STATUS) & MPL3115A2::REGISTER_STATUS_PTDR)) {
		nanosleep(&centisec, nullptr);
	}
}
			


float MPL3115A2::pressure()
{
	this->_set_mode(0);
	this->_one_shot();
	this->_await_completion();
	i2c_smbus_write_byte(this->smbus, MPL3115A2::REGISTER_PRESSURE_MSB);
	i2c_smbus_read_block_data(this->smbus, MPL3115A2::REGISTER_PRESSURE_MSB, this->buffer);
	std::uint32_t p = std::uint32_t(this->buffer[0]) << 16 | std::uint32_t(this->buffer[1]) << 8 | std::uint32_t(this->buffer[2]);
	return float(p) / 6400.0;
}


float MPL3115A2::altitude()
{
	this->_set_mode(1);
	this->_one_shot();
	this->_await_completion();
	i2c_smbus_write_byte(this->smbus, MPL3115A2::REGISTER_PRESSURE_MSB);
	i2c_smbus_read_block_data(this->smbus, MPL3115A2::REGISTER_PRESSURE_MSB, this->buffer);
	std::uint32_t a = std::uint32_t(this->buffer[0]) << 24 | std::uint32_t(this->buffer[1]) << 16 | std::uint32_t(this->buffer[2] << 8);
	return float(a) / 65536.0;
}


float MPL3115A2::temperature()
{
	this->_set_mode(1);
	this->_one_shot();
	this->_await_completion();
	i2c_smbus_write_byte(this->smbus, MPL3115A2::REGISTER_PRESSURE_MSB);
	i2c_smbus_read_block_data(this->smbus, MPL3115A2::REGISTER_PRESSURE_MSB, this->buffer);
	std::uint32_t t = std::uint32_t(this->buffer[3]) << 8 | std::uint32_t(this->buffer[4]);
	return float(t) / 256.0;
}