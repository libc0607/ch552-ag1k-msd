/*
 * ch552-ag1k-msd
 * Load bitstream to AG1K series FPGA using CH552
 * Github: libc0607/ch552-ag1k-msd
 * 
 * License: GNU Lesser General Public License v2.1 
 *  (same as DeqingSun/ch55xduino)
 * 
 * Very few features. No guarantees.
 * 
 * Hardware connection (all optional except hwspi): 
 * --------------------------------------------------
 *  AG1KLPQ48       |   CH552         |
 * --------------------------------------------------
 *  PIN_24_CDONE    |   P1.0          |
 *  PIN_27_SDO      |   P1.6_MISO     | fixed (hwspi)
 *  PIN_29_SS       |   P1.4          |
 *  PIN_31_SCK      |   P1.7_SCK      | fixed (hwspi)
 *  PIN_32_SDI      |   P1.5_MOSI     | fixed (hwspi)
 *  PIN_30_CRESET_B |   P1.1          | pull-up 
 *  -------------------------------------------------
 *  Note that CH552 should working under 3.3V.
 * 
 * Compile & download: 
 *  1. install https://github.com/DeqingSun/ch55xduino 
 *  2. open this project,
 *      "CH552 Boards", 
 *      "USER CODE w/ 148B USB ram", 
 *      "16MHz 3.3V"
 *  3. upload
 * 
 * Usage: 
 *  1. Compile your design <design_name> in Supra 
 *  2. Rename <design_name>.bin to SRAM.BIN 
 *    (should be 46,808 bytes; keep filename UPPERCASE)
 *  3. Plug your CH552 board to your PC's USB, 
 *    then drag it into USB Mass Storage <AG1K MSD>,
 *    choose overwrite
 *  4. when loading finished, pin CDONE will be 
 *    released (HIGH) by FPGA 
 * 
 * Refereneces:
 *  AG1K_Rev1_0.pdf
 *  MANUAL_AG1KLP.pdf
 */

#ifndef USER_USB_RAM
#error "This example needs to be compiled with a USER USB setting"
#endif
#ifndef CH552
#error "This program is designed running on CH552"
#endif
#if (F_CPU != 16000000)
#error "This program is designed running at 16MHz clock (3.3V)"
#endif

#include "src/userMassStorage/USBMassStorage.h"
#include <SPI.h>

#define AG1KBIT_SIZE_BYTE 46808   // 16'hb6d8
#define SPI_FREQ 2000000

#define PIN_SPI_SCK     17
#define PIN_SPI_MISO    16
#define PIN_SPI_MOSI    15
#define PIN_SPI_CS      14
#define PIN_AG1K_CRST   32
#define PIN_AG1K_CDONE  10

// Ensure the bitstream being written is 
// 1. from the beginning (offset=0): or we needs to reset ag1k
// 2. continuous: assume(offset[t1] = offset[t0] + BYTES_WRITTEN_DURING_T0)
// 3. complete (when sram_next_offset_required == AG1KBIT_SIZE_BYTE)
uint16_t sram_next_offset_required = 0;

// and, don't print string to serial in this func or it'll be weird
// just put one char a time for debugging

__code const uint8_t ReadmeFileContent[] = "AG1K MSD \r\nRename your bitstream to SRAM.BIN (keep UPPERCASE, 46,808 bytes each), drag it here and choose overwrite \r\n";
__code const uint8_t bitstream_read_inf[] = "DO NOT EDIT OR SAVE THIS FILE \r\n";

void file_sram_read_cb (uint16_t offset) {
  for (uint8_t i=0; i<BULK_MAX_PACKET_SIZE; i++) {
    if (offset < AG1KBIT_SIZE_BYTE) {
      if (offset < sizeof(bitstream_read_inf)) {
        BOT_Tx_Buf[i] = bitstream_read_inf[i];
      } else {
        BOT_Tx_Buf[i] = ' ';
      }
    } else {
      BOT_Tx_Buf[i] = 0;
    }
    offset++;
  }
}

void file_sram_write_cb (uint16_t offset) {
  
  uint16_t i=0;
  
  if (offset == 0) { 
    
    // 1.1.1 new file dropped in, reset chip 
    digitalWrite(PIN_SPI_CS, LOW);
    digitalWrite(PIN_AG1K_CRST, LOW); 
    delayMicroseconds(10);     // 200ns in datasheet
    digitalWrite(PIN_AG1K_CRST, HIGH); 
    delayMicroseconds(1600);   // 800us in datasheet

    // 1.1.2 write the first bulk
    for (i=0; i<BULK_MAX_PACKET_SIZE; i++) {
      SPI_transfer(BOT_Rx_Buf[i]);
      offset++;
    }
    sram_next_offset_required = offset;
    
  } else if (offset == sram_next_offset_required) {
       
      if (offset + BULK_MAX_PACKET_SIZE < AG1KBIT_SIZE_BYTE) {
        // 1.2.1.1 the full bulk is to be written
        for (i=0; i<BULK_MAX_PACKET_SIZE; i++) {
          SPI_transfer(BOT_Rx_Buf[i]);
          offset++;
        }
        sram_next_offset_required = offset;
        
      } else if (offset < AG1KBIT_SIZE_BYTE) {
        // 1.2.2.1 write the last bulk, then finish writing
        uint16_t last_packet_length = AG1KBIT_SIZE_BYTE - offset;
        for (i=0; i<last_packet_length; i++) {
          SPI_transfer(BOT_Rx_Buf[i]);
          offset++;
        }
        for (i=0; i<25; i++) {    // 200/8=25
          SPI_transfer(0xFF);     // 200 dummy bits
        }
        
        // 1.2.2.3 Now user bitstream is running
        digitalWrite(PIN_SPI_CS, HIGH);
        if (digitalRead(PIN_AG1K_CDONE) == HIGH) {
          // load bitstream finish
          // Serial0_println_s("done.");
        }
        sram_next_offset_required = AG1KBIT_SIZE_BYTE; // mark as finished
        
      } else { // offset >= AG1KBIT_SIZE_BYTE
        // 1.2.3 who tm care 
      }
      
  } else {
    // 1.3 It shouldn't be here, transfer not continuous
    //Serial0_print_s("[ERR]: USB transfer failed, ");
    //Serial0_print_u(offset);
    //Serial0_print_c(':');
    //Serial0_print_u(sram_next_offset_required);
  }
}

__code File_Entry filesOnDrive[] = {  //keep filename UPPERCASE
  {
    .filename = {'R', 'E', 'A', 'D', 'M', 'E', ' ', ' ', 'T', 'X', 'T'},
    .filetime = {TIME_LB(12, 34, 56), TIME_HB(12, 34, 56)},
    .filedate = {DATE_LB(2021, 1, 2), DATE_HB(2021, 1, 2)},
    .filesize = sizeof(ReadmeFileContent),
    .fileCallBackType = CONST_DATA_FILE,
    .fileReadHandler = {.constPtr = ReadmeFileContent},
    .fileWriteHandler = 0,  //NULL
  },
  {
    .filename = {'S', 'R', 'A', 'M', ' ', ' ', ' ', ' ', 'B', 'I', 'N'},
    .filetime = {TIME_LB(12, 34, 56), TIME_HB(12, 34, 56)},
    .filedate = {DATE_LB(2021, 1, 2), DATE_HB(2021, 1, 2)},
    .filesize = AG1KBIT_SIZE_BYTE,
    .fileCallBackType = FUNCTION_CALLBACK_FILE,
    .fileReadHandler = {.funcPtr = file_sram_read_cb},
    .fileWriteHandler = file_sram_write_cb,  
  }
};
__code uint8_t filesOnDriveCount = sizeof(filesOnDrive) / sizeof(File_Entry);

void setup() {

  //Serial0_begin(100000);
  //Serial0_println_s("startup");
  
  pinMode(PIN_SPI_CS, OUTPUT);
  pinMode(PIN_SPI_SCK, OUTPUT);
  pinMode(PIN_SPI_MISO, INPUT_PULLUP);
  pinMode(PIN_SPI_MOSI, OUTPUT);
  pinMode(PIN_AG1K_CRST, OUTPUT);
  pinMode(PIN_AG1K_CDONE, INPUT);
  digitalWrite(PIN_SPI_CS, HIGH);

  SPI_begin();
  SPI_beginTransaction(SPISettings(SPI_FREQ, MSBFIRST, SPI_MODE3));
  delay(10);
  
  USBInit();
  delay(10);
}

void loop() {
  // your code here
}
