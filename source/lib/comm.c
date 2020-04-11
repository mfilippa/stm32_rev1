// -----------------------------------------------------------------------------
// SerialComm 3.0 - MPF 11/2019
// -----------------------------------------------------------------------------


// includes
#include "error.h"
#include "comm.h"
#include "sch.h"

// comm state machine states
typedef enum comm_state_enum {
    COMM_STATE_IDLE,
    COMM_STATE_RX_START,
    COMM_STATE_RX_HB,
    COMM_STATE_RX_DATA,
    COMM_STATE_TX_HB,
    COMM_STATE_TX_DATA,
    COMM_STATE_TX_END,
} comm_state_t;

// module structure
struct comm_struct {

    // uart
    uart_t uart;

    // buffer size
    uint32_t buffer_size;

    // serial packet
    uint8_t * rx_buffer;
    uint8_t * tx_buffer;
    uint32_t rx_size;
    uint32_t rx_index;
    uint32_t tx_size;
    uint32_t tx_index;

    // handler
    comm_handler_t * handler;

    // state machine stuff
    comm_state_t state;
    uint32_t timeout;
    uint32_t half_byte;
    uint32_t index;
    uint32_t sch_timer_handle;

} comm;

// macro code
#define IS_DIGIT(c)    ((c>='0')&&(c<='9'))
#define IS_AF(c)     ((c>='A')&&(c<='F'))
#define IS_af(c)    ((c>='a')&&(c<='f'))
#define IS_HEX(c)    (IS_DIGIT(c)||IS_AF(c)||IS_af(c))

// private prototypes
uint32_t comm_ascii_to_byte(uint32_t high, uint32_t low);
uint32_t comm_byte_to_ascii_high(uint32_t value);
uint32_t comm_byte_to_ascii_low(uint32_t value);

// -----------------------------------------------------------------------------
// initialize
// -----------------------------------------------------------------------------
void comm_init(uart_t uart, uint32_t buffer_size, uint8_t * rx_buffer,
        uint8_t * tx_buffer, comm_handler_t * handler){
    // save uart
    if (uart>=UART_COUNT) error_raise(ERROR_COMM_INIT);
    else comm.uart = uart;
    // save buffer data
    comm.buffer_size = buffer_size;
    comm.rx_buffer = rx_buffer;
    comm.tx_buffer = tx_buffer;
    // save handler
    comm.handler = handler;
    // get a timer handle
    comm.sch_timer_handle = sch_timer_register();
    if(comm.sch_timer_handle==0) error_raise(ERROR_COMM_INIT);
    // init state machine
    comm.state = COMM_STATE_IDLE;
}

// -----------------------------------------------------------------------------
// packet state machine
// -----------------------------------------------------------------------------
void comm_step(void){
    // step through state machine
    switch(comm.state){
    //-------------------------------------------------------
    case COMM_STATE_IDLE:
        // EVENT: received a char
        if (uart_read_ready(comm.uart)==1){
            uint32_t temp = (uint32_t) uart_read(comm.uart);
            // SUBEVENT: received start char, start rx
            if (temp == COMM_FRAME_START) {
                comm.index = 0;
                sch_timer_set(comm.sch_timer_handle,COMM_FRAME_TIMEOUT);
                comm.state = COMM_STATE_RX_START;
            }
        }
        // EVENT: tx requested
        else if (comm.tx_size != 0){
            // SUBEVENT: uart is ready
            if (uart_write_ready(comm.uart)==1){
                // send start char
                comm.index = 0;
                uart_write(comm.uart,(uint8_t)COMM_FRAME_START);
                sch_timer_set(comm.sch_timer_handle,COMM_FRAME_TIMEOUT);
                comm.state = COMM_STATE_TX_HB;
            }
        }
        break;
    //-------------------------------------------------------
    case COMM_STATE_RX_START:    // start char received
        // EVENT: received a char
        if (uart_read_ready(comm.uart)){
            uint32_t temp = (uint32_t) uart_read(comm.uart);
            // SUBEVENT: it is a HEX, store half byte, wait for other half
            if (IS_HEX(temp)){
                comm.half_byte = temp;
                sch_timer_set(comm.sch_timer_handle,COMM_FRAME_TIMEOUT);
                comm.state = COMM_STATE_RX_HB;
            }
            // else got something else, go to idle
            else {
                comm.state = COMM_STATE_IDLE;
            }
        }
        // handle timeout
        if (sch_timer_has_expired(comm.sch_timer_handle)) comm.state = COMM_STATE_IDLE;
        break;
    //-------------------------------------------------------
    case COMM_STATE_RX_HB:    // half byte received
        // EVENT: received a char
        if (uart_read_ready(comm.uart)==1){
            uint32_t temp = (uint32_t) uart_read(comm.uart);
            // SUBEVENT: it is a HEX, store in buffer
            if (IS_HEX(temp)){
                comm.rx_buffer[comm.index] =
                        (uint8_t)(comm_ascii_to_byte(comm.half_byte,temp)&0xFF);
                comm.index++;
                sch_timer_set(comm.sch_timer_handle,COMM_FRAME_TIMEOUT);
                comm.state = COMM_STATE_RX_DATA;
            }
            // else got something else, go to idle
            else {
                comm.state = COMM_STATE_IDLE;
            }
        }
        // handle timeout
        if (sch_timer_has_expired(comm.sch_timer_handle)) comm.state = COMM_STATE_IDLE;
        break;
    //-------------------------------------------------------
    case COMM_STATE_RX_DATA:    // data byte received
        // EVENT: received a char
        if (uart_read_ready(comm.uart)==1){
            uint32_t temp = (uint32_t) uart_read(comm.uart);
            // SUBEVENT: it is a HEX
            if (IS_HEX(temp)){
                // index is less than buffer size, store and continue
                if (comm.index<comm.buffer_size){
                    comm.half_byte = temp;
                    sch_timer_set(comm.sch_timer_handle,COMM_FRAME_TIMEOUT);
                    comm.state = COMM_STATE_RX_HB;
                }
                // else discard
                else {
                    comm.state = COMM_STATE_IDLE;
                }
            }
            // SUBEVENT: is it an end frame, finish transmission
            else if (temp == COMM_FRAME_END){
                comm.rx_size = comm.index;
                comm.rx_index = comm.tx_index = 0;
                comm.handler(comm.rx_size);
                comm.state = COMM_STATE_IDLE;
            }
            // else got something else, go to idle
            else {
                comm.state = COMM_STATE_IDLE;
            }
        }
        // handle timeout
        if (sch_timer_has_expired(comm.sch_timer_handle)) comm.state = COMM_STATE_IDLE;
        break;
    //-------------------------------------------------------
    case COMM_STATE_TX_HB:    // waiting to send half byte
        // EVENT: iface is ready
        if (uart_write_ready(comm.uart)==1){
            // send half byte and wait
            uart_write(comm.uart,(uint8_t)comm_byte_to_ascii_high((uint32_t)comm.tx_buffer[comm.index]));
            sch_timer_set(comm.sch_timer_handle,COMM_FRAME_TIMEOUT);
            comm.state = COMM_STATE_TX_DATA;
        }
        // handle timeout
        if (sch_timer_has_expired(comm.sch_timer_handle)) {
            comm.tx_size = 0;
            comm.state = COMM_STATE_IDLE;
        }
        break;
    //-------------------------------------------------------
    case COMM_STATE_TX_DATA:    // waiting to send other half byte
        // EVENT: iface is ready
        if (uart_write_ready(comm.uart)==1){
            // send other half byte
            uart_write(comm.uart,(uint8_t)comm_byte_to_ascii_low((uint32_t)comm.tx_buffer[comm.index]));
            comm.index++;
            comm.tx_size--;
            // SUBEVENT: more data to send
            if (comm.tx_size!=0){
                sch_timer_set(comm.sch_timer_handle,COMM_FRAME_TIMEOUT);
                comm.state = COMM_STATE_TX_HB;
            }
            // else done with data, send end char
            else {
                sch_timer_set(comm.sch_timer_handle,COMM_FRAME_TIMEOUT);
                comm.state = COMM_STATE_TX_END;
            }
        }
        // handle timeout
        if (sch_timer_has_expired(comm.sch_timer_handle)) {
            comm.tx_size = 0;
            comm.state = COMM_STATE_IDLE;
        }
        break;
    //-------------------------------------------------------
    case COMM_STATE_TX_END:    // waiting to end char
        // EVENT: iface is ready
        if (uart_write_ready(comm.uart)==1){
            // send end char
            uart_write(comm.uart,(uint8_t)COMM_FRAME_END);
            comm.state = COMM_STATE_IDLE;
        }
        // handle timeout
        if (sch_timer_has_expired(comm.sch_timer_handle)) {
            comm.tx_size = 0;
            comm.state = COMM_STATE_IDLE;
        }
        break;
    }
}

// -----------------------------------------------------------------------------
// ASCII to byte
// -----------------------------------------------------------------------------
uint32_t comm_ascii_to_byte(uint32_t high, uint32_t low){
    uint32_t temp;
    // process high
    if (IS_DIGIT(high)){
        temp = (high - '0')<<4;
    } else if (IS_af(high)){
        temp = ((high - 'a')<<4)+0xA0;
    } else if (IS_AF(high)){
        temp = ((high - 'A')<<4)+0xA0;
    } else {
        // should not be here
        temp = 0;
    }
    // process low
    if (IS_DIGIT(low)){
        temp += (low - '0');
    } else if (IS_af(low)){
        temp += (low - 'a')+0x0A;
    } else if (IS_AF(low)){
        temp += (low - 'A')+0x0A;
    }
    return temp;
}

// -----------------------------------------------------------------------------
// byte to ASCII
// -----------------------------------------------------------------------------
uint32_t comm_byte_to_ascii_high(uint32_t value){
    uint32_t temp = value;
    temp = (temp>>4)&0x0F;
    if (temp<0x0A) temp += '0';
    else temp += 'A' - 0x0A;
    return temp;
}
uint32_t comm_byte_to_ascii_low(uint32_t value){
    uint32_t temp = value;
    temp = temp & 0x0F;
    if (temp<0x0A) temp += '0';
    else temp += 'A' - 0x0A;
    return temp;
}

// -----------------------------------------------------------------------------
// read8
// -----------------------------------------------------------------------------
uint32_t comm_read8(void){
    uint32_t data = 0;
    // check if there are enough bytes left in buffer to read
    if (comm.rx_index+1<=comm.rx_size){
        data = (uint32_t)comm.rx_buffer[comm.rx_index];
        comm.rx_index++;
    }
    return data;
}

// -----------------------------------------------------------------------------
// read16
// -----------------------------------------------------------------------------
uint32_t comm_read16(void){
    uint32_t data = 0;
    // check if there are enough bytes left in buffer to read
    if (comm.rx_index+2<=comm.rx_size){
        data = ((uint32_t)comm.rx_buffer[comm.rx_index])<<8;
        comm.rx_index++;
        data += (uint32_t)comm.rx_buffer[comm.rx_index];
        comm.rx_index++;
    }
    return data;
}

// -----------------------------------------------------------------------------
// read32
// -----------------------------------------------------------------------------
uint32_t comm_read32(void){
    uint32_t data = 0;
    // check if there are enough bytes left in buffer to read
    if (comm.rx_index+4<=comm.rx_size){
        data = ((uint32_t)comm.rx_buffer[comm.rx_index])<<24;
        comm.rx_index++;
        data += ((uint32_t)comm.rx_buffer[comm.rx_index])<<16;
        comm.rx_index++;
        data += ((uint32_t)comm.rx_buffer[comm.rx_index])<<8;
        comm.rx_index++;
        data += (uint32_t)comm.rx_buffer[comm.rx_index];
        comm.rx_index++;
    }
    return data;
}

// -----------------------------------------------------------------------------
// readf
// -----------------------------------------------------------------------------
float comm_readf(void){
    uint32_t ivalue;
    float * fvalue = (float *)&ivalue;
    ivalue = comm_read32();
    return (*fvalue);
}

// -----------------------------------------------------------------------------
// write8
// -----------------------------------------------------------------------------
void comm_write8(uint32_t data){
    // check if there are enough bytes left in buffer to write
    if (comm.tx_index+1<=comm.buffer_size){
        comm.tx_buffer[comm.tx_index] = (uint8_t)(data&0xFF);
        comm.tx_index++;
        comm.tx_size+=1;
    }
}

// -----------------------------------------------------------------------------
// write16
// -----------------------------------------------------------------------------
void comm_write16(uint32_t data){
    // check if there are enough bytes left in buffer to write
    if (comm.tx_index+2<=comm.buffer_size){
        comm.tx_buffer[comm.tx_index] = (uint8_t)((data&0xFF00)>>8);
        comm.tx_index++;
        comm.tx_buffer[comm.tx_index] = (uint8_t)(data&0xFF);
        comm.tx_index++;
        comm.tx_size+=2;
    }
}

// -----------------------------------------------------------------------------
// write32
// -----------------------------------------------------------------------------
void comm_write32(uint32_t data){
    // check if there are enough bytes left in buffer to write
    if (comm.tx_index+4<=comm.buffer_size){
        comm.tx_buffer[comm.tx_index] = (uint8_t)((data&0xFF000000)>>24);
        comm.tx_index++;
        comm.tx_buffer[comm.tx_index] = (uint8_t)((data&0xFF0000)>>16);
        comm.tx_index++;
        comm.tx_buffer[comm.tx_index] = (uint8_t)((data&0xFF00)>>8);
        comm.tx_index++;
        comm.tx_buffer[comm.tx_index] = (uint8_t)(data&0xFF);
        comm.tx_index++;
        comm.tx_size+=4;
    }
}

// -----------------------------------------------------------------------------
// writef
// -----------------------------------------------------------------------------
void comm_writef(float data){
    uint32_t * ivalue = (uint32_t *)&data;
    comm_write32(*ivalue);
}


