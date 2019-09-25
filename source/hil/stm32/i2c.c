// -----------------------------------------------------------------------------
// i2c.c - MPF 01/2017
// -----------------------------------------------------------------------------

// includes
#include "hal/i2c.h"
#include "stm32f4xx.h"

// i2c state machine states
typedef enum i2c_state_enum {
    I2C_STATE_IDLE,
    I2C_STATE_TX_START,
    I2C_STATE_TX_ADDRESS,
    I2C_STATE_TX_DATA,
    I2C_STATE_RX_START,
    I2C_STATE_RX_ADDRESS,
    I2C_STATE_RX_DATA,
    I2C_STATE_STOP,
    I2C_STATE_WAIT_BUSY,
    I2C_STATE_WAIT_START,
    I2C_STATE_WAIT_TX_ADDRESS,
    I2C_STATE_WAIT_TX_DATA,
    I2C_STATE_WAIT_RX_ADDRESS,
    I2C_STATE_WAIT_RX_DATA,
} i2c_state_t;

// module structure
struct i2c_struct {
    // state machine vars
    i2c_state_t state;
    i2c_state_t next_state;
    uint32_t start_generated;

    // buffers
    uint32_t tx_address;
    uint32_t * tx_buffer;
    uint32_t tx_index;
    uint32_t tx_size;
    uint32_t rx_address;
    uint32_t * rx_buffer;
    uint32_t rx_index;
    uint32_t rx_size;
    void (*callback)(void);
} i2c;

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
uint32_t i2c_init(void){

    GPIO_InitTypeDef gpio_init;
    I2C_InitTypeDef i2c_init;

    // config:
    // PB10 - I2C2-SCL
    // PB11 - I2C2-SDA

    // enable clocks
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);

    // configure pins
    gpio_init.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
    gpio_init.GPIO_Mode = GPIO_Mode_AF;
    gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
    gpio_init.GPIO_OType = GPIO_OType_OD;
    GPIO_Init(GPIOB, &gpio_init);

    // configure alternate function
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_I2C2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_I2C2);

    // configure I2C module
    i2c_init.I2C_ClockSpeed = 100000;
    i2c_init.I2C_OwnAddress1 = 0x00;
    i2c_init.I2C_Mode = I2C_Mode_I2C;
    i2c_init.I2C_Ack = I2C_Ack_Enable;
    i2c_init.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    i2c_init.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_Init(I2C2, &i2c_init);

    // Configure I2C Filters
    I2C_AnalogFilterCmd(I2C2,ENABLE);
    I2C_DigitalFilterConfig(I2C2,0x0F);

    // enable clock stretching
    I2C_StretchClockCmd(I2C2, ENABLE);

    // enable
    I2C_Cmd(I2C2, ENABLE);

    // init state machine
    i2c.state = i2c.next_state = I2C_STATE_IDLE;
    i2c.start_generated = 0;

    return 0;
}

// -----------------------------------------------------------------------------
// write
// -----------------------------------------------------------------------------
void i2c_write(uint32_t address, uint32_t * data, uint32_t nr_bytes,
        void (*callback)(void)){
    // save data
    if((nr_bytes!=0)&&(i2c.state == I2C_STATE_IDLE)){
        i2c.tx_address = address;
        i2c.tx_buffer = data;
        i2c.tx_size = nr_bytes;
        i2c.callback = callback;
    }
}

// -----------------------------------------------------------------------------
// read
// -----------------------------------------------------------------------------
void i2c_read(uint32_t address, uint32_t * data, uint32_t nr_bytes,
        void (*callback)(void)){
    // save data
    if((nr_bytes!=0)&&(i2c.state == I2C_STATE_IDLE)){
        i2c.rx_address = address;
        i2c.rx_buffer = data;
        i2c.rx_size = nr_bytes;
        i2c.callback = callback;
    }
}

// -----------------------------------------------------------------------------
// get state
// -----------------------------------------------------------------------------
uint32_t i2c_get_state(void){
    return i2c.state;
}

// -----------------------------------------------------------------------------
// step
// -----------------------------------------------------------------------------
void i2c_step(void){
    // step through state machine
    switch(i2c.state){
    //-------------------------------------------------------
    case I2C_STATE_IDLE:
        // EVENT: TX requested
        if (i2c.tx_size!=0){
            // enable ACK
            I2C_AcknowledgeConfig(I2C2, ENABLE);
            // start TX
            i2c.next_state = I2C_STATE_TX_START;
            i2c.state = I2C_STATE_WAIT_BUSY;
        }
        // EVENT: RX requested
        else if (i2c.rx_size!=0){
            // enable ACK
            I2C_AcknowledgeConfig(I2C2, ENABLE);
            // start RX
            i2c.next_state = I2C_STATE_RX_START;
            i2c.state = I2C_STATE_WAIT_BUSY;
        }
        break;
    //-------------------------------------------------------
    case I2C_STATE_TX_START:
        // generate start condition
        I2C_GenerateSTART(I2C2, ENABLE);
        i2c.next_state = I2C_STATE_TX_ADDRESS;
        i2c.state = I2C_STATE_WAIT_START;
        break;
    //-------------------------------------------------------
    case I2C_STATE_RX_START:
        // generate start condition
        I2C_GenerateSTART(I2C2, ENABLE);
        i2c.next_state = I2C_STATE_RX_ADDRESS;
        i2c.state = I2C_STATE_WAIT_START;
        break;
    //-------------------------------------------------------
    case I2C_STATE_TX_ADDRESS:
        // send address and request write
        I2C_Send7bitAddress(I2C2, i2c.tx_address, I2C_Direction_Transmitter);
        i2c.tx_index = 0;
        i2c.next_state = I2C_STATE_TX_DATA;
        i2c.state = I2C_STATE_WAIT_TX_ADDRESS;
        break;
    //-------------------------------------------------------
    case I2C_STATE_RX_ADDRESS:
        // send address and request read
        I2C_Send7bitAddress(I2C2, i2c.rx_address, I2C_Direction_Receiver);
        i2c.rx_index = 0;
        i2c.next_state = I2C_STATE_RX_DATA;
        i2c.state = I2C_STATE_WAIT_RX_ADDRESS;
        break;
    //-------------------------------------------------------
    case I2C_STATE_TX_DATA:
        // send data
        I2C_SendData(I2C2, i2c.tx_buffer[i2c.tx_index]);
        // check if more data to send
        i2c.tx_size--;
        i2c.tx_index++;
        if (i2c.tx_size!=0){
            i2c.next_state = I2C_STATE_TX_DATA;
        }
        // no more data to send
        else {
            // check if there is data to RX, restart comm if so
            if (i2c.rx_size!=0){
                i2c.next_state = I2C_STATE_RX_START;
            }
            // else stop comm
            else {
                i2c.next_state = I2C_STATE_STOP;
            }
        }
        i2c.state = I2C_STATE_WAIT_TX_DATA;
        break;
    //-------------------------------------------------------
    case I2C_STATE_RX_DATA:
        // read data (if last data, disable ACK)
        if(i2c.rx_size==1) I2C_AcknowledgeConfig(I2C2, DISABLE);
        i2c.rx_buffer[i2c.rx_index] = (uint32_t) I2C_ReceiveData(I2C2);
        // check if more data to receive
        i2c.rx_size--;
        i2c.rx_index++;
        if (i2c.rx_size!=0){
            i2c.next_state = I2C_STATE_RX_DATA;
        }
        // no more data to send, stop comm
        else {
            i2c.next_state = I2C_STATE_STOP;
        }
        i2c.state = I2C_STATE_WAIT_RX_DATA;
        break;
    //-------------------------------------------------------
    case I2C_STATE_STOP:
        I2C_GenerateSTOP(I2C2, ENABLE);
        i2c.state = I2C_STATE_IDLE;
        // callback if requested
        if (i2c.callback!=0) i2c.callback();
        break;
    //-------------------------------------------------------
    case I2C_STATE_WAIT_BUSY:
        // check if busy flag is cleared, move to next state
        if(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY)==RESET){
            i2c.state = i2c.next_state;
        }
        break;
    //-------------------------------------------------------
    case I2C_STATE_WAIT_START:
        // check if start has been successfully generated (EV5)
        if(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)==SUCCESS){
            i2c.state = i2c.next_state;
        }
        break;
    //-------------------------------------------------------
    case I2C_STATE_WAIT_TX_ADDRESS:
        // check if address is set and acknowledged (EV6)
        if (I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)==SUCCESS){
            i2c.state = i2c.next_state;
        }
        break;
    //-------------------------------------------------------
    case I2C_STATE_WAIT_RX_ADDRESS:
        // check if address is set and acknowledged (EV6)
        if (I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)==SUCCESS){
            i2c.state = i2c.next_state;
        }
        break;
    //-------------------------------------------------------
    case I2C_STATE_WAIT_TX_DATA:
        // wait for byte transmitted (EV8_2)
        if(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED)==SUCCESS){
            i2c.state = i2c.next_state;
        }
        break;
    //-------------------------------------------------------
    case I2C_STATE_WAIT_RX_DATA:
        // wait for byte to be received
        if(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED)==SUCCESS){
            i2c.state = i2c.next_state;
        }
        break;
    //-------------------------------------------------------
    default:
        // should not be here
        while(1);
    }
}

