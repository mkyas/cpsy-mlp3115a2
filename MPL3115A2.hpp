
#ifndef MPL3115A2_INCLUDED
#define MPL3115A2_INCLUDED

#include <cstdint>

class MPL3115A2
{
	static constexpr std::uint8_t ADDRESS         = 0x60U;

	static constexpr std::uint8_t STATUS          = 0x00U;
	static constexpr std::uint8_t OUT_P_MSB       = 0x01U;
	static constexpr std::uint8_t OUT_P_CSB       = 0x02U;
	static constexpr std::uint8_t OUT_P_LSB       = 0x03U;
	static constexpr std::uint8_t OUT_T_MSB       = 0x04U;
	static constexpr std::uint8_t OUT_T_LSB       = 0x05U;
	static constexpr std::uint8_t DR_STATUS       = 0x06U;
	static constexpr std::uint8_t OUT_P_DELTA_MSB = 0x07U;
	static constexpr std::uint8_t OUT_P_DELTA_CSB = 0x08U;
	static constexpr std::uint8_t OUT_P_DELTA_LSB = 0x09U;
	static constexpr std::uint8_t OUT_T_DELTA_MSB = 0x0AU;
	static constexpr std::uint8_t OUT_T_DELTA_LSB = 0x0BU;
	static constexpr std::uint8_t WHO_AM_I        = 0x0CU;
	static constexpr std::uint8_t BAR_IN_MSB      = 0x14U;
	static constexpr std::uint8_t BAR_IN_LSB      = 0x15U;
	static constexpr std::uint8_t OFF_H           = 0x2DU;

	static constexpr std::uint8_t REGISTER_STATUS_TDR      = 0x02U;
	static constexpr std::uint8_t REGISTER_STATUS_PDR      = 0x04U;
	static constexpr std::uint8_t REGISTER_STATUS_PTDR     = 0x08U;

	static constexpr std::uint8_t PT_DATA_CFG              = 0x13U;

	static constexpr std::uint8_t PT_DATA_CFG_TDEFE        = 0x01U;
	static constexpr std::uint8_t PT_DATA_CFG_PDEFE        = 0x02U;
	static constexpr std::uint8_t PT_DATA_CFG_DREM         = 0x04U;

	static constexpr std::uint8_t CTRL_REG1                = 0x26U;
	static constexpr std::uint8_t CTRL_REG2                = 0x27U;
	static constexpr std::uint8_t CTRL_REG3                = 0x28U;
	static constexpr std::uint8_t CTRL_REG4                = 0x29U;
	static constexpr std::uint8_t CTRL_REG5                = 0x2AU;

	static constexpr std::uint8_t CTRL_REG1_SBYB           = 0x01U;
	static constexpr std::uint8_t CTRL_REG1_OST            = 0x02U;
	static constexpr std::uint8_t CTRL_REG1_RST            = 0x04U;
	static constexpr std::uint8_t CTRL_REG1_RAW            = 0x40U;
	static constexpr std::uint8_t CTRL_REG1_ALT            = 0x80U;
	static constexpr std::uint8_t CTRL_REG1_BAR            = 0x00U;

	static constexpr std::uint8_t CTRL_REG1_OS1            = 0x00U;
	static constexpr std::uint8_t CTRL_REG1_OS2            = 0x08U;
	static constexpr std::uint8_t CTRL_REG1_OS4            = 0x10U;
	static constexpr std::uint8_t CTRL_REG1_OS8            = 0x18U;
	static constexpr std::uint8_t CTRL_REG1_OS16           = 0x20U;
	static constexpr std::uint8_t CTRL_REG1_OS32           = 0x28U;
	static constexpr std::uint8_t CTRL_REG1_OS64           = 0x30U;
	static constexpr std::uint8_t CTRL_REG1_OS128          = 0x38U;

	static constexpr std::uint8_t REGISTER_STARTCONVERSION = 0x12U;

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
	explicit MPL3115A2(int smbus = 1, std::uint8_t address = MPL3115A2::ADDRESS);
	MPL3115A2(const MPL3115A2&) = delete;
	~MPL3115A2();

	float pressure(void) noexcept;
	float altitude(void) noexcept;
	float temperature(void) noexcept;

private:
	void _set_mode(std::uint8_t mode) noexcept;
	void _one_shot(void) noexcept;
	void _await_completion(std::uint8_t status = MPL3115A2::REGISTER_STATUS_PTDR) noexcept;
};

#endif
