#include "mruby.h"
#include "mruby/string.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

static mrb_value mrb_threaded_tcp_client_connect(mrb_state *mrb, mrb_value self) {
  int connection_file_descriptor = -1;
  if (connection_file_descriptor != -1) {
    fprintf(stderr, "Already connected\n");
    return mrb_false_value();
  }

  const char *host;
  mrb_int port;

  mrb_get_args(
    mrb,
    "zi",
    &host,
    &port
  );

  struct sockaddr_in server_addr = { 0 };
  server_addr.sin_port = htons(port);
  server_addr.sin_family = AF_INET; // IPV4

  // Set the address
  int is_success = inet_pton(AF_INET, host, &server_addr.sin_addr);
  if (is_success != 1) {
    if (is_success == -1) {
      perror("inet_pton");
    }
    fprintf(stderr, "Invalid address %s\n", host);
    return mrb_false_value();
  }

  fprintf(stdout, "CONNECTING: address=%s port=%d\n", host, port);

  // Create the local socket
  connection_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
  if (connection_file_descriptor == -1) {
    perror("socket");
    return mrb_false_value();
  }

  is_success = connect(connection_file_descriptor, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if (is_success == -1) {
    perror("connect");
    return mrb_false_value();
  }

  return mrb_fixnum_value(connection_file_descriptor);
}

static mrb_value mrb_threaded_tcp_client_disconnect(mrb_state *mrb, mrb_value self) {
  mrb_int connection_file_descriptor;

  mrb_get_args(
    mrb,
    "i",
    &connection_file_descriptor
  );

  // Disconnect from the server
  int is_success = shutdown(connection_file_descriptor, SHUT_RDWR);
  if (is_success == -1) {
    perror("shutdown");
    return mrb_false_value();
  }

  // Close the file descriptor
  is_success = close(connection_file_descriptor);
  if (is_success == -1) {
    perror("close");
    return mrb_false_value();
  }

  return mrb_true_value();
}

static mrb_value mrb_threaded_tcp_client_write(mrb_state *mrb, mrb_value self) {
  mrb_int connection_file_descriptor;
  const char *message;

  mrb_get_args(
    mrb,
    "iz",
    &connection_file_descriptor,
    &message
  );

  write(connection_file_descriptor, message, sizeof(message));
  // add message to list to write

  return mrb_nil_value();
}

static mrb_value mrb_threaded_tcp_client_read(mrb_state *mrb, mrb_value self) {
  mrb_int connection_file_descriptor;
  mrb_value blk;

  mrb_get_args(
  	mrb,
  	"i&",
  	&connection_file_descriptor,
  	&blk
  );

  char buffer[255];
  read(connection_file_descriptor, buffer, 255);

  // for each message, yield the message
  mrb_yield(mrb, blk, mrb_str_new_cstr(mrb, buffer));

  return mrb_nil_value();
}

void mrb_mruby_threaded_tcp_client_gem_init(mrb_state *mrb) {
  struct RClass *lib_module = mrb_define_module(mrb, "ThreadedTCP");
  struct RClass *internal = mrb_define_module_under(mrb, lib_module, "Internal");

  mrb_define_class_method(mrb, internal, "connect", mrb_threaded_tcp_client_connect, MRB_ARGS_REQ(2));
  mrb_define_class_method(mrb, internal, "disconnect", mrb_threaded_tcp_client_disconnect, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, internal, "write", mrb_threaded_tcp_client_write, MRB_ARGS_REQ(2));
  mrb_define_class_method(mrb, internal, "read", mrb_threaded_tcp_client_read, MRB_ARGS_REQ(2));
}

void mrb_mruby_threaded_tcp_client_gem_final(mrb_state *mrb) {
  // disconnect / cleanup
}
