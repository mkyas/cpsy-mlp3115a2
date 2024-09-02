
#ifndef MPL3115A2_INCLUDED
#define MPL3115A2_INCLUDED

#include <cstdint>

class MPL3115A2
{
	static constexpr std::uint8_t ADDRESS                  = 0x60;

	static constexpr std::uint8_t REGISTER_STATUS          = 0x00;
	static constexpr std::uint8_t REGISTER_PRESSURE_MSB    = 0x01;
	static constexpr std::uint8_t REGISTER_PRESSURE_CSB    = 0x02;
	static constexpr std::uint8_t REGISTER_PRESSURE_LSB    = 0x03;
	static constexpr std::uint8_t REGISTER_TEMP_MSB        = 0x04;
	static constexpr std::uint8_t REGISTER_TEMP_LSB        = 0x05;
	static constexpr std::uint8_t REGISTER_DR_STATUS       = 0x06;
	static constexpr std::uint8_t OUT_P_DELTA_MSB          = 0x07;
	static constexpr std::uint8_t OUT_P_DELTA_CSB          = 0x08;
	static constexpr std::uint8_t OUT_P_DELTA_LSB          = 0x09;
	static constexpr std::uint8_t OUT_T_DELTA_MSB          = 0x0a;
	static constexpr std::uint8_t OUT_T_DELTA_LSB          = 0x0b;
	static constexpr std::uint8_t WHOAMI                   = 0x0c;
	static constexpr std::uint8_t BAR_IN_MSB               = 0x14;
	static constexpr std::uint8_t BAR_IN_LSB               = 0x15;
	static constexpr std::uint8_t OFF_H                    = 0x2d;

	static constexpr std::uint8_t REGISTER_STATUS_TDR      = 0x02;
	static constexpr std::uint8_t REGISTER_STATUS_PDR      = 0x04;
	static constexpr std::uint8_t REGISTER_STATUS_PTDR     = 0x08;

	static constexpr std::uint8_t PT_DATA_CFG              = 0x13;
	static constexpr std::uint8_t PT_DATA_CFG_TDEFE        = 0x01;
	static constexpr std::uint8_t PT_DATA_CFG_PDEFE        = 0x02;
	static constexpr std::uint8_t PT_DATA_CFG_DREM         = 0x04;

	static constexpr std::uint8_t CTRL_REG1                = 0x26;
	static constexpr std::uint8_t CTRL_REG2                = 0x27;
	static constexpr std::uint8_t CTRL_REG3                = 0x28;
	static constexpr std::uint8_t CTRL_REG4                = 0x29;
	static constexpr std::uint8_t CTRL_REG5                = 0x2a;

	static constexpr std::uint8_t CTRL_REG1_SBYB           = 0x01;
	static constexpr std::uint8_t CTRL_REG1_OST            = 0x02;
	static constexpr std::uint8_t CTRL_REG1_RST            = 0x04;
	static constexpr std::uint8_t CTRL_REG1_RAW            = 0x40;
	static constexpr std::uint8_t CTRL_REG1_ALT            = 0x80;
	static constexpr std::uint8_t CTRL_REG1_BAR            = 0x00;

	static constexpr std::uint8_t CTRL_REG1_OS1            = 0x00;
	static constexpr std::uint8_t CTRL_REG1_OS2            = 0x08;
	static constexpr std::uint8_t CTRL_REG1_OS4            = 0x10;
	static constexpr std::uint8_t CTRL_REG1_OS8            = 0x18;
	static constexpr std::uint8_t CTRL_REG1_OS16           = 0x20;
	static constexpr std::uint8_t CTRL_REG1_OS32           = 0x28;
	static constexpr std::uint8_t CTRL_REG1_OS64           = 0x30;
	static constexpr std::uint8_t CTRL_REG1_OS128          = 0x38;

	static constexpr std::uint8_t REGISTER_STARTCONVERSION = 0x12;

	int smbus;
	std::uint8_t address;

typedef union {
		struct {
			std::uint8_t SBYB : 1;
			std::uint8_t OST : 1;
			std::uint8_t RST : 1;
			std::uint8_t OS : 3;
			std::uint8_t RAW : 1;
			std::uint8_t ALT : 1;
		} bit;
		std::uint8_t reg;
	} ctrl_reg1;
	ctrl_reg1 _ctrl_reg1;

	std::uint8_t buffer[32];

public:
	MPL3115A2(int smbus = 1, std::uint8_t address = MPL3115A2::ADDRESS);
	MPL3115A2(const MPL3115A2&) = delete;
	~MPL3115A2();

	float pressure();
	float altitude();
	float temperature();

private:
	void _set_mode(std::uint8_t mode);
	void _one_shot(void);
	void _await_completion(std::uint8_t status = MPL3115A2::REGISTER_STATUS_PTDR);
};

#endif
