// asf
#include "print_funcs.h"

// aleph-avr32
#include "app.h"
#include "bfin.h"
#include "flash.h"
#include "filesystem.h"
#include "screen.h"

// lppr
#include "files.h"

#define LPPR_LDR_PATH "/dsp/aleph-lines.ldr"

// search for our .ldr file in the filesystem
// load and store in internal flash if found, otherwise report failure

/* void files_init(void) { */
/* } */

// fread: no size arg
static void fake_fread(volatile u8* dst, u32 size, void* fp) {
  u32 n = 0;
  //  print_dbg("\r\n\r\n read: \r\n");
  while(n < size) {
    *dst = fl_fgetc(fp);
    /* print_dbg_ulong(n); */
    /* print_dbg(","); */
    /* print_dbg_hex( ((u32)dst) & 0xff ); */
    /* print_dbg(" \r\n"); */
    n++;
    dst++;
  }
}

u8 files_search_dsp(void) {
  void* fp;
  //  FL_DIR dirstat;
  u32 size;
  // open file pointer
  print_dbg("\r\n opening dsp file at path: ");
  print_dbg(LPPR_LDR_PATH);

  // we don't want to be handling UI interrupts while working with the filesystem
  app_pause();

  fp = fl_fopen(LPPR_LDR_PATH, "r");
  if( fp != NULL) {
    size = ((FL_FILE*)(fp))->filelength;
    print_dbg("\r\n opened file, size: ");
    print_dbg_ulong(size);
    print_dbg(" ; pointer: ");
    print_dbg_hex((u32)fp);

    screen_line(0, 1, "loading sdcard-> RAM...     ", 0x3f);
    screen_refresh();

    // read .ldr data to RAM buffer
    //    fl_fread((void*)bfinLdrData, 1, size, fp);
    fake_fread((void*)bfinLdrData, size, fp);

    fl_fclose(fp);

    print_dbg("\r\n finished reading .ldr file to RAM");
    bfinLdrSize = size;


    screen_line(0, 1, "writing RAM->flash...     ", 0x3f);
    screen_refresh();

    // write buf to flash
    flash_write_ldr();
    print_dbg("\r\n finished writing .ldr file to flash");
    // reboot the DSP from RAM
    print_dbg("\r\n booting DSP from RAM");


    screen_line(0, 1, "booting DSP from RAM...     ", 0x3f);
    screen_refresh();

    bfin_load_buf();
    app_resume();

    return 1; // ok
  } else {
    print_dbg("\r\n encountered an error opening .ldr file.");

    app_resume();
    return 0; // error opening
  }
}


