// -----------------------------------------------------------------------------
// app.c - MPF 01/2017
// -----------------------------------------------------------------------------

// includes
#include "app.h"
#include "hal/halinc.h"
#include "lib/libinc.h"

// definitions
#define APP_COMM_BUFFER_SIZE	1024
#define APP_DEBUG_BUFFER_SIZE	1024

// module structure
struct app_struct {
	// comm buffers
	uint8_t rx_buffer[APP_COMM_BUFFER_SIZE];
	uint8_t tx_buffer[APP_COMM_BUFFER_SIZE];
	// debug buffer
	debug_t debug_buffer[APP_DEBUG_BUFFER_SIZE];
} app;

// prototypes
void app_led_blink(void);
void app_dummy(void);
void app_comm_handler(uint32_t rx_size);
void app_adc_trigger(void);
void app_adc_slow(void);
void app_adc_fast(void);

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
uint32_t app_init(void) {

	// initialize hardware
	if (sys_tick_init(1000, &app_systick) != 0)
		return 1;
	if (io_init() != 0)
		return 1;
	if (uart_init(115200) != 0)
		return 1;
	if (adc_init(&app_adc_slow, &app_adc_fast) != 0)
		return 1;
//	if (pwm_init(0) != 0)
//		return 1;
//	if (i2c_init()!=0)
//		return 1;

// initialize modules
	if (sch_init() != 0)
		return 1;
	if (comm_init(APP_COMM_BUFFER_SIZE, app.rx_buffer, app.tx_buffer,
			&app_comm_handler) != 0)
		return 1;
	if (debug_init(app.debug_buffer, APP_DEBUG_BUFFER_SIZE) != 0)
		return 1;
	if (fb_init() != 0)
		return 1;

	// initialize feedbacks
	fb_store_params(FB_FAST_CH1, 1, 0, INT32_MIN, INT32_MAX);
	fb_store_params(FB_FAST_CH2, 1, 0, INT32_MIN, INT32_MAX);
	fb_store_params(FB_FAST_CH3, 1, 0, INT32_MIN, INT32_MAX);
	fb_store_params(FB_FAST_CH4, 1, 0, INT32_MIN, INT32_MAX);
	fb_store_params(FB_FAST_CH5, 1, 0, INT32_MIN, INT32_MAX);
	fb_store_params(FB_FAST_CH6, 1, 0, INT32_MIN, INT32_MAX);
	fb_store_params(FB_FAST_CH7, 1, 0, INT32_MIN, INT32_MAX);
	fb_store_params(FB_FAST_CH8, 1, 0, INT32_MIN, INT32_MAX);
	fb_store_params(FB_SLOW_CH1, 1, 0, INT32_MIN, INT32_MAX);
	fb_store_params(FB_SLOW_CH2, 1, 0, INT32_MIN, INT32_MAX);
	fb_store_params(FB_SLOW_CH3, 1, 0, INT32_MIN, INT32_MAX);
	fb_store_params(FB_SLOW_CH4, 1, 0, INT32_MIN, INT32_MAX);

	// register scheduled functions
	if (sch_function_register(&app_led_blink, 500) == 0)
		return 1;
	if (sch_function_register(&app_adc_trigger,10) == 0)
		return 1;

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
void app_adc_trigger(void){
	// trigger fast conversion
	adc_trigger_fast();
	// trigger slow conversion
	adc_trigger_slow();
}

// -----------------------------------------------------------------------------
// slow adc
// -----------------------------------------------------------------------------
void app_adc_slow(void) {
	// process slow adc data
	fb_process(FB_SLOW_CH1, adc_read_slow(ADC_SLOW_CH1));
	fb_process(FB_SLOW_CH2, adc_read_slow(ADC_SLOW_CH2));
	fb_process(FB_SLOW_CH3, adc_read_slow(ADC_SLOW_CH3));
	fb_process(FB_SLOW_CH4, adc_read_slow(ADC_SLOW_CH4));
}

// -----------------------------------------------------------------------------
// fast adc
// -----------------------------------------------------------------------------
void app_adc_fast(void) {
	// process slow adc data
	fb_process(FB_FAST_CH1, adc_read_fast(ADC_FAST_CH1));
	fb_process(FB_FAST_CH2, adc_read_fast(ADC_FAST_CH2));
	fb_process(FB_FAST_CH3, adc_read_fast(ADC_FAST_CH3));
	fb_process(FB_FAST_CH4, adc_read_fast(ADC_FAST_CH4));
	fb_process(FB_FAST_CH5, adc_read_fast(ADC_FAST_CH5));
	fb_process(FB_FAST_CH6, adc_read_fast(ADC_FAST_CH6));
	fb_process(FB_FAST_CH7, adc_read_fast(ADC_FAST_CH7));
	fb_process(FB_FAST_CH8, adc_read_fast(ADC_FAST_CH8));
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
		comm_write16(adc_read_fast(ADC_FAST_CH1));
		comm_write16(adc_read_fast(ADC_FAST_CH2));
		comm_write16(adc_read_fast(ADC_FAST_CH3));
		comm_write16(adc_read_fast(ADC_FAST_CH4));
		comm_write16(adc_read_fast(ADC_FAST_CH5));
		comm_write16(adc_read_fast(ADC_FAST_CH6));
		comm_write16(adc_read_fast(ADC_FAST_CH7));
		comm_write16(adc_read_fast(ADC_FAST_CH8));
		comm_write16(adc_read_slow(ADC_SLOW_CH1));
		comm_write16(adc_read_slow(ADC_SLOW_CH2));
		comm_write16(adc_read_slow(ADC_SLOW_CH3));
		comm_write16(adc_read_slow(ADC_SLOW_CH4));
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
		comm_write16(fb_get(FB_SLOW_CH1));
		comm_write16(fb_get(FB_SLOW_CH2));
		comm_write16(fb_get(FB_SLOW_CH3));
		comm_write16(fb_get(FB_SLOW_CH4));
		break;

	default:
		// command not recognized
		comm_write8(0xFF);
	}
}

