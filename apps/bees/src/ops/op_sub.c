#include "net_protected.h"
#include "op_sub.h"

//-------------------------------------------------
//---- static func declare
static void op_sub_in_a(op_sub_t* sub, const io_t v);
static void op_sub_in_b(op_sub_t* sub, const io_t v);
static void op_sub_in_btrig(op_sub_t* sub, const io_t v);
static void op_sub_inc_input(op_sub_t* mul, const s16 idx, const io_t inc);

// pickle / unpickle
static u8* op_sub_pickle(op_sub_t* op, u8* dst);
static const u8* op_sub_unpickle(op_sub_t* op, const u8* src);

//-------------------------------------------------
//---- static vars
static const char* op_sub_instring = "A       B       B_TRIG  ";
static const char* op_sub_outstring = "DIF     ";
static const char* op_sub_opstring = "SUB";

static op_in_fn op_sub_in_fn[3] = {
  (op_in_fn)&op_sub_in_a,
  (op_in_fn)&op_sub_in_b, 
  (op_in_fn)&op_sub_in_btrig
};

//-------------------------------------------------
//---- external func define
void op_sub_init(void* mem) {
  op_sub_t* sub = (op_sub_t*)mem;
  sub->super.numInputs = 3;
  sub->super.numOutputs = 1;
  sub->outs[0] = -1;

  sub->super.inc_fn = (op_inc_fn)op_sub_inc_input;
  sub->super.in_fn = op_sub_in_fn;
  sub->super.in_val = sub->in_val;
  sub->super.pickle = (op_pickle_fn) (&op_sub_pickle);
  sub->super.unpickle = (op_unpickle_fn) (&op_sub_unpickle);

  sub->super.out = sub->outs;
  sub->super.opString = op_sub_opstring;
  sub->super.inString = op_sub_instring;
  sub->super.outString = op_sub_outstring;
  sub->super.type = eOpSub;  

  sub->in_val[0] = &(sub->a);
  sub->in_val[1] = &(sub->b);
  sub->in_val[2] = &(sub->btrig);

  sub->a = 0;
  sub->b = 0;
  sub->btrig = 0;
}

//-------------------------------------------------
//---- static func define

static void op_sub_in_a(op_sub_t* sub, const io_t v) {
  sub->a = v;
  sub->val = op_ssub(sub->a, sub->b);
  net_activate(sub->outs[0], sub->val, sub);
}

static void op_sub_in_b(op_sub_t* sub, const io_t v) {
  sub->b = v;
  sub->val = op_ssub(sub->a, sub->b);
  if(sub->btrig) {
    net_activate(sub->outs[0], sub->val, sub);
  }
}

static void op_sub_in_btrig(op_sub_t* sub, const io_t v) {
  if(v > 0) { sub->btrig = OP_ONE; } { sub->btrig = 0; }
}

//===== UI input
static void op_sub_inc_input(op_sub_t* sub, const s16 idx, const io_t inc) {
  io_t val;
  switch(idx) {
  case 0:  // a
    val = op_ssub(sub->a, inc);
    op_sub_in_a(sub, val);
    break; 
  case 1:  // b
    val = op_ssub(sub->b, inc);
    op_sub_in_b(sub, val);
    break;
  case 2:  // trig
    op_sub_in_btrig(sub, inc);
    break;
  }
}


// pickle / unpickle
u8* op_sub_pickle(op_sub_t* op, u8* dst) {
  dst = pickle_io(op->a, dst);
  dst = pickle_io(op->b, dst);
  dst = pickle_io(op->btrig, dst);
  return dst;
}

const u8* op_sub_unpickle(op_sub_t* op, const u8* src) {
  src = unpickle_io(src, &(op->a));
  src = unpickle_io(src, &(op->b));
  src = unpickle_io(src, &(op->btrig));
  return src;
}
