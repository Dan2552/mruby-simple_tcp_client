#include "mruby.h"
#include "mruby/string.h"

static mrb_value mrb_threaded_tcp_client_connect(mrb_state *mrb, mrb_value self) {
	const char *host;
	mrb_int port;

	mrb_get_args(
		mrb,
		"zi",
		&host,
		&port
	);

	// TODO: connect

	return mrb_nil_value();
}

static mrb_value mrb_threaded_tcp_client_write(mrb_state *mrb, mrb_value self) {
	const char *message;

	mrb_get_args(
		mrb,
		"z",
		&message
	);

	// add message to list to write

	return mrb_nil_value();
}

static mrb_value mrb_threaded_tcp_client_read(mrb_state *mrb, mrb_value self) {
	mrb_value blk;
	mrb_get_args(mrb, "&", &blk);

	// for each message, yield the message
	mrb_yield(mrb, blk, mrb_fixnum_value(1));

	return mrb_nil_value();
}

void mrb_mruby_threaded_tcp_client_gem_init(mrb_state *mrb) {
	struct RClass *threaded_tcp_client = mrb_define_class(mrb, "ThreadedTCPClient", mrb->object_class);

	mrb_define_method(mrb, threaded_tcp_client, "connect", mrb_threaded_tcp_client_connect, MRB_ARGS_REQ(2));
	mrb_define_method(mrb, threaded_tcp_client, "write", mrb_threaded_tcp_client_write, MRB_ARGS_REQ(1));
	mrb_define_method(mrb, threaded_tcp_client, "read", mrb_threaded_tcp_client_read, MRB_ARGS_REQ(1));
}

void mrb_mruby_threaded_tcp_client_gem_final(mrb_state *mrb) {
  // disconnect / cleanup
}
