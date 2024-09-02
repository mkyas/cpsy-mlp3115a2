/* Data sheet at https://www.nxp.com/docs/en/data-sheet/MPL3115A2.pdf */

#include <bit>
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

	std::uint8_t whoami = i2c_smbus_read_byte_data(this->smbus, MPL3115A2::WHO_AM_I);
	if (whoami != 0xc4) {
		throw std::runtime_error("Not a MPL3115A2");
	}

	i2c_smbus_write_byte_data(this->smbus, MPL3115A2::CTRL_REG1, MPL3115A2::CTRL_REG1_RST);
	while (i2c_smbus_read_byte_data(this->smbus, MPL3115A2::CTRL_REG1) & MPL3115A2::CTRL_REG1_RST) {
		nanosleep(&centisec, nullptr);
	}

	// set oversampling and altitude mode
	_ctrl_reg1.reg = MPL3115A2::CTRL_REG1_OS128 | MPL3115A2::CTRL_REG1_ALT;
	i2c_smbus_write_byte_data(this->smbus, MPL3115A2::CTRL_REG1, _ctrl_reg1.reg);

  	// enable data ready events for pressure/altitude and temperature
	i2c_smbus_write_byte_data(this->smbus, MPL3115A2::PT_DATA_CFG,
			          MPL3115A2::PT_DATA_CFG_TDEFE |
                                  MPL3115A2::PT_DATA_CFG_PDEFE |
                                  MPL3115A2::PT_DATA_CFG_DREM);
}

MPL3115A2::~MPL3115A2()
{
	close(this->smbus);
}



void MPL3115A2::_set_mode(std::uint8_t mode)
{
	this->_ctrl_reg1.reg = i2c_smbus_read_byte_data(this->smbus, MPL3115A2::CTRL_REG1);
	this->_ctrl_reg1.bit.ALT = mode;
	i2c_smbus_write_byte_data(this->smbus, MPL3115A2::CTRL_REG1, this->_ctrl_reg1.reg);
}

void MPL3115A2::_one_shot(void)
{
	this->_ctrl_reg1.reg = i2c_smbus_read_byte_data(this->smbus, MPL3115A2::CTRL_REG1);
	while (this->_ctrl_reg1.bit.OST) {
		nanosleep(&centisec, nullptr);
		this->_ctrl_reg1.reg = i2c_smbus_read_byte_data(this->smbus, MPL3115A2::CTRL_REG1);
	}
	this->_ctrl_reg1.bit.OST = 1;
	i2c_smbus_write_byte_data(this->smbus, MPL3115A2::CTRL_REG1, this->_ctrl_reg1.reg);
}

void MPL3115A2::_await_completion(std::uint8_t status)
{
	while (0 == (i2c_smbus_read_byte_data(this->smbus, MPL3115A2::STATUS) & status)) {
		nanosleep(&centisec, nullptr);
	}
}
			


float MPL3115A2::pressure()
{
	this->_set_mode(0);
	this->_one_shot();
	this->_await_completion();
	i2c_smbus_read_block_data(this->smbus, MPL3115A2::OUT_P_MSB, this->buffer);
	std::uint32_t p;
	if constexpr (std::endian::native == std::endian::big) {
		p = std::uint32_t(this->buffer[0]) << 16 | std::uint32_t(this->buffer[1]) << 8 | std::uint32_t(this->buffer[2]);
	} else if constexpr (std::endian::native == std::endian::little) {
		p = std::uint32_t(this->buffer[2]) << 16 | std::uint32_t(this->buffer[1]) << 8 | std::uint32_t(this->buffer[0]);
	} else {
		throw std::runtime_error("Unknown endianess");
	}
	return float(p) / 6400.0;
}


float MPL3115A2::altitude()
{
	this->_set_mode(1);
	this->_one_shot();
	this->_await_completion();
	i2c_smbus_read_block_data(this->smbus, MPL3115A2::OUT_P_MSB, this->buffer);
	std::uint32_t a;
	if constexpr (std::endian::native == std::endian::big) {
		a = std::uint32_t(this->buffer[0]) << 24 | std::uint32_t(this->buffer[1]) << 16 | std::uint32_t(this->buffer[2] << 8);
	} else if constexpr (std::endian::native == std::endian::little) {
		a = std::uint32_t(this->buffer[2]) << 24 | std::uint32_t(this->buffer[1]) << 16 | std::uint32_t(this->buffer[0] << 8);
	} else {
		throw std::runtime_error("Unknown endianess");
	}
	return float(a) / 65536.0;
}


float MPL3115A2::temperature()
{
	this->_one_shot();
	this->_await_completion();
	i2c_smbus_read_block_data(this->smbus, MPL3115A2::OUT_P_MSB, this->buffer);
	std::uint32_t t;
	if constexpr (std::endian::native == std::endian::big) {
		t = std::uint32_t(this->buffer[3]) << 8 | std::uint32_t(this->buffer[4]);
	} else if constexpr (std::endian::native == std::endian::little) {
		t = std::uint32_t(this->buffer[4]) << 8 | std::uint32_t(this->buffer[3]);
	} else {
		throw std::runtime_error("Unknown endianess");
	}
	return float(t) / 256.0;
}
