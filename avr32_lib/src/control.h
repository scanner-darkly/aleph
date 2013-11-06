
/*
  control.h
  avr32
  aleph
 
  FIXME:  
  duplicating param change queue from the blackfin.
  this is decidedly sub-optimal but lets get things working.

  parameter changes are stored in a set structure.
  maximum one request per parameter enters the queue
  
 */

#ifndef _ALEPH_BFIN_CONTROL_H_
#define _ALEPH_BFIN_CONTROL_H_

//#include "module_custom.h"
#include "param_common.h"
#include "types.h"

//---------------------------------------------
//---- definitions / types

#define MAX_PARAMS 256

// depth of FIFO
#define CTL_BUF_SIZE 256
#define CTL_BUF_SIZE_1 255

// process at most 1 control change per audio sample
// so keep (buf size < (samplerate / control rate))
// frame divisor (sr / kr)
#define CTL_FRAME_DIV 48

// byes in the dirty-flag bitfield
//#define DIRTY_BYTES (BITNSLOTS(NUM_PARAMS))
#define DIRTY_BYTES MAX_PARAMS

//-- return values
// added request
#define CTL_REQUEST_OK   0
// buffer was full
#define CTL_REQUEST_FULL 1

// parameter change event type
typedef struct _ctlEvent {
  u8 idx;
  pval val;
} ctlEvent_t;


// add param change to buffer
/// FIXME: uh will this work for params < 0 ?
extern u8 ctl_param_change(u32 param, u32 value);
// execute the last param change
extern void ctl_perform_last_change(void);
// attempt to execute all pending param changes
extern void ctl_perform_all_changes(void);
// tick every audio frame
extern void ctl_next_frame(void);

#endif // h guard