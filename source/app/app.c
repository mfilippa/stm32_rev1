// -----------------------------------------------------------------------------
// app.c - MPF 01/2017
// -----------------------------------------------------------------------------

// includes
#include "hal/halinc.h"
#include "lib/libinc.h"
#include "app/appinc.h"
#ifdef MATLAB
#include "matlab/wrapper.h"
#endif

// definitions
#define APP_COMM_BUFFER_SIZE	1024
#define APP_DEBUG_BUFFER_SIZE	1024

// ADC channels
enum adc_channels_enum {
	// fast channels
	ADC_FAST_CH1=0,
	ADC_FAST_CH2,
	ADC_FAST_CH3,
	ADC_FAST_CH4,
	ADC_FAST_CH5,
	ADC_FAST_CH6,
	ADC_FAST_CH7,
	ADC_FAST_CH8,
	ADC_FAST_CH9,
	ADC_FAST_CH10,
	// slow channels
	ADC_SLOW_CH1,
	ADC_SLOW_CH2,
	ADC_SLOW_CH3,
	ADC_SLOW_CH4,
	ADC_SLOW_CH5,
	ADC_SLOW_CH6,
	ADC_SLOW_CH7,
	ADC_SLOW_TEMP,
	ADC_CH_COUNT,
};

// ADC configuration
#ifdef MATLAB
adc_config_t adc_config = {
	// pin start
	0,
	// nr of channels
	ADC_CH_COUNT,
};
#else
adc_config_t adc_config = {
	// fast channel sequence:
	// 0-17 for ADC1, 0-3 and 10-13 for ADC2-ADC3
	{ { 0, 0, 0, 0 },	// ADC1
	  { 0, 0, 0, 0 }, 	// ADC2
	  { 0, 0, 0, 0 } }, // ADC3
	// fast count: zero to disable, ADC1 count > ADC2-ADC3 count
	{ 0, 0, 0 },
	// trigger: 1 for pwm, 0 for sw
	0,
	// slow channel sequence: 0-15,16,17
	{ 0, 1, 2, 3, 4, 5, 6, 16 },
	// slow channel count, zero to disable
	8,
	// trigger: 1 for pwm, 0 for sw
	0,
};
#endif

#ifdef MATLAB
// io config
io_config_t io_config[3] = {
	// LED: pout[20]
	{20,1,0},
	// DEBUG1: pout[21]
	{21,1,0},
	// DEBUG2: pout[22]
	{22,1,0},
};
#else
// io config
io_config_t io_config[3] = {
	// LED: PC13
	{2,13,1,1,0},
	// DEBUG1: C6
	{2,6,1,1,0},
	// DEBUG2: C7
	{2,7,1,1,0},
};
#endif

// module structure
struct app_struct {
	// comm buffers
	uint8_t rx_buffer[APP_COMM_BUFFER_SIZE];
	uint8_t tx_buffer[APP_COMM_BUFFER_SIZE];
	// debug buffer
	debug_t debug_buffer[APP_DEBUG_BUFFER_SIZE];
	// fb table
	fb_table_t fb_table[ADC_CH_COUNT];
} app;

// prototypes
void app_led_blink(void);
void app_dummy(void);
void app_comm_handler(uint32_t rx_size);
void app_adc_trigger(void);
void app_adc_process_slow(void);
void app_adc_process_fast(void);

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
uint32_t app_init(void) {

	// initialize hardware
	if (sys_tick_init(1000, &app_systick) != 0) return 1;
	if (io_init(io_config, IO_CH_COUNT) != 0) return 1;
	if (uart_init(UART0, UART_BAUD_115200) != 0) return 1;
//	if (uart_init(UART1, UART_BAUD_115200) != 0) return 1;
	if (adc_init(&adc_config, &app_adc_process_slow, &app_adc_process_fast)
			!= 0) return 1;
//	if (pwm_init(0) != 0) return 1;
//	if (i2c_init()!=0) return 1;

	// initialize modules
	if (sch_init() != 0) return 1;
	if (comm_init(UART0, APP_COMM_BUFFER_SIZE, app.rx_buffer,
			app.tx_buffer, &app_comm_handler) != 0) return 1;
	if (debug_init(app.debug_buffer, APP_DEBUG_BUFFER_SIZE) != 0) return 1;
	if (fb_init(app.fb_table, FB_CH_COUNT) != 0) return 1;

	// initialize feedbacks
	fb_store_params(FB_FAST_CH1, 1, 0, INT32_MIN, INT32_MAX);
	fb_store_params(FB_FAST_CH2, 1, 0, INT32_MIN, INT32_MAX);
	fb_store_params(FB_FAST_CH3, 1, 0, INT32_MIN, INT32_MAX);
	fb_store_params(FB_FAST_CH4, 1, 0, INT32_MIN, INT32_MAX);
	fb_store_params(FB_FAST_CH5, 1, 0, INT32_MIN, INT32_MAX);
	fb_store_params(FB_FAST_CH6, 1, 0, INT32_MIN, INT32_MAX);
	fb_store_params(FB_FAST_CH7, 1, 0, INT32_MIN, INT32_MAX);
	fb_store_params(FB_FAST_CH8, 1, 0, INT32_MIN, INT32_MAX);
	fb_store_params(FB_FAST_CH9, 1, 0, INT32_MIN, INT32_MAX);
	fb_store_params(FB_FAST_CH10, 1, 0, INT32_MIN, INT32_MAX);
	fb_store_params(FB_SLOW_CH1, 1, 0, INT32_MIN, INT32_MAX);
	fb_store_params(FB_SLOW_CH2, 1, 0, INT32_MIN, INT32_MAX);
	fb_store_params(FB_SLOW_CH3, 1, 0, INT32_MIN, INT32_MAX);
	fb_store_params(FB_SLOW_CH4, 1, 0, INT32_MIN, INT32_MAX);
	fb_store_params(FB_SLOW_CH5, 1, 0, INT32_MIN, INT32_MAX);
	fb_store_params(FB_SLOW_CH6, 1, 0, INT32_MIN, INT32_MAX);
	fb_store_params(FB_SLOW_CH7, 1, 0, INT32_MIN, INT32_MAX);
	// temperature
	// scaling = 3.3/4096 * (1<<Q_TEMP)/AVG_SLOPE,
	// offset = 1/scaling *(V25*(1<<Q_TEMP)/AVG_SLOPE - 25*(1<<Q_TEMP))
	// AVG_SLOPE = 0.0025, Q_TEMP = 8 (-128C to 128C)
	// V25 = 0.76
	fb_store_params(FB_TEMP, 21120, 8, INT32_MIN, INT32_MAX);
	fb_store_offset(FB_TEMP, 866);

	// register scheduled functions
	if (sch_function_register(&app_led_blink, 500) == 0) return 1;
	if (sch_function_register(&app_adc_trigger, 10) == 0) return 1;

	// success
	return 0;
}

// -----------------------------------------------------------------------------
// background
// -----------------------------------------------------------------------------
void app_background(void) {
	// comm step
	comm_step();
	// run scheduled tasks
	sch_step();
}

// -----------------------------------------------------------------------------
// systick
// -----------------------------------------------------------------------------
void app_systick(void) {
	// scheduler tick
	sch_tick();
}

// -----------------------------------------------------------------------------
// led blink
// -----------------------------------------------------------------------------
void app_led_blink(void) {
	io_toggle(IO_LED);
}

// -----------------------------------------------------------------------------
// dummy
// -----------------------------------------------------------------------------
void app_dummy(void) {

}
// -----------------------------------------------------------------------------
// adc trigger
// -----------------------------------------------------------------------------
void app_adc_trigger(void) {
	// trigger fast conversion
	adc_sw_trigger_fast();
	// trigger slow conversion
	adc_sw_trigger_slow();
}

// -----------------------------------------------------------------------------
// slow adc
// -----------------------------------------------------------------------------
void app_adc_process_slow(void) {
	// process slow adc data
	fb_process(FB_SLOW_CH1, adc_read(ADC_SLOW_CH1));
	fb_process(FB_SLOW_CH2, adc_read(ADC_SLOW_CH2));
	fb_process(FB_SLOW_CH3, adc_read(ADC_SLOW_CH3));
	fb_process(FB_SLOW_CH4, adc_read(ADC_SLOW_CH4));
	fb_process(FB_SLOW_CH5, adc_read(ADC_SLOW_CH5));
	fb_process(FB_SLOW_CH6, adc_read(ADC_SLOW_CH6));
	fb_process(FB_SLOW_CH7, adc_read(ADC_SLOW_CH7));
	fb_process(FB_TEMP, adc_read(ADC_SLOW_TEMP));
}

// -----------------------------------------------------------------------------
// fast adc
// -----------------------------------------------------------------------------
void app_adc_process_fast(void) {
	// process slow adc data
	fb_process(FB_FAST_CH1, adc_read(ADC_FAST_CH1));
	fb_process(FB_FAST_CH2, adc_read(ADC_FAST_CH2));
	fb_process(FB_FAST_CH3, adc_read(ADC_FAST_CH3));
	fb_process(FB_FAST_CH4, adc_read(ADC_FAST_CH4));
	fb_process(FB_FAST_CH5, adc_read(ADC_FAST_CH5));
	fb_process(FB_FAST_CH6, adc_read(ADC_FAST_CH6));
	fb_process(FB_FAST_CH7, adc_read(ADC_FAST_CH7));
	fb_process(FB_FAST_CH8, adc_read(ADC_FAST_CH8));
	fb_process(FB_FAST_CH9, adc_read(ADC_FAST_CH9));
	fb_process(FB_FAST_CH10, adc_read(ADC_FAST_CH10));
}

// -----------------------------------------------------------------------------
// comm handler
// -----------------------------------------------------------------------------
void app_comm_handler(uint32_t rx_size) {

	uint32_t cmd, temp;

	// first byte is cmd, echo it
	cmd = comm_read8();
	comm_write8(cmd);

	// comm machine
	switch (cmd) {

	case 0x00:
		// echo
		for (temp = 1; temp < rx_size; temp++) {
			comm_write8(comm_read8());
		}
		break;
	case 0x01:
		break;
	case 0x02:
		break;
	case 0x03:
		break;
	case 0x04:
		break;
	case 0x05:
		break;
	case 0x06:
		break;
	case 0x07:
		sys_reset();
		break;

	case 0xD0:
		// get debug vars
		comm_write32(out1);
		comm_write32(out2);
		comm_write32(out3);
		break;
	case 0xD1:
		// set input debug vars
		temp = comm_read8();
		if (temp == 1)
			inp1 = comm_read32();
		else if (temp == 2)
			inp2 = comm_read32();
		else if (temp == 3)
			inp3 = comm_read32();
		break;
	case 0xD2:
		// raw ADC data
		for (temp = 0; temp < (ADC_CH_COUNT); temp++) {
			comm_write16(adc_read((uint32_t)temp));
		}
		break;
	case 0xD3:
		// fb data
		comm_write16(fb_get(FB_FAST_CH1));
		comm_write16(fb_get(FB_FAST_CH2));
		comm_write16(fb_get(FB_FAST_CH3));
		comm_write16(fb_get(FB_FAST_CH4));
		comm_write16(fb_get(FB_FAST_CH5));
		comm_write16(fb_get(FB_FAST_CH6));
		comm_write16(fb_get(FB_FAST_CH7));
		comm_write16(fb_get(FB_FAST_CH8));
		comm_write16(fb_get(FB_FAST_CH9));
		comm_write16(fb_get(FB_FAST_CH10));
		comm_write16(fb_get(FB_SLOW_CH1));
		comm_write16(fb_get(FB_SLOW_CH2));
		comm_write16(fb_get(FB_SLOW_CH3));
		comm_write16(fb_get(FB_SLOW_CH4));
		comm_write16(fb_get(FB_SLOW_CH5));
		comm_write16(fb_get(FB_SLOW_CH6));
		comm_write16(fb_get(FB_SLOW_CH7));
		comm_write16(fb_get(FB_TEMP));
		break;

	default:
		// command not recognized
		comm_write8(0xFF);
	}
}

