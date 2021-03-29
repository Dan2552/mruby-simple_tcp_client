#include "mruby.h"
#include "mruby/string.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

static mrb_value mrb_simple_tcp_client_connect(mrb_state *mrb, mrb_value self) {
  const char *host;
  mrb_int port;
  mrb_int verbose;

  mrb_get_args(mrb, "zii", &host, &port, &verbose);

  int connection_file_descriptor = -1;
  struct sockaddr_in server_addr = { 0 };
  server_addr.sin_port = htons(port);
  server_addr.sin_family = AF_INET; // IPV4

  // Set the address
  int is_success = inet_pton(AF_INET, host, &server_addr.sin_addr);
  if (is_success != 1) {
    if (is_success == -1) {
      if (verbose) {
        perror("inet_pton");
      }
    }
    fprintf(stderr, "Invalid address %s\n", host);
    return mrb_false_value();
  }

  // Create the local socket
  connection_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
  if (connection_file_descriptor == -1) {
    if (verbose) {
      perror("socket");
    }
    return mrb_false_value();
  }

  // Connect
  is_success = connect(connection_file_descriptor, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if (is_success == -1) {
    if (verbose) {
      perror("connect");
    }
    close(connection_file_descriptor);
    return mrb_false_value();
  }

  return mrb_fixnum_value(connection_file_descriptor);
}

static mrb_value mrb_simple_tcp_client_disconnect(mrb_state *mrb, mrb_value self) {
  mrb_int connection_file_descriptor;
  mrb_int verbose;
  mrb_get_args(mrb, "ii", &connection_file_descriptor, &verbose);

  // Disconnect from the server
  int is_success = shutdown(connection_file_descriptor, SHUT_RDWR);
  if (is_success == -1) {
    if (verbose) {
      perror("shutdown");
    }
  }

  // Close the file descriptor
  is_success = close(connection_file_descriptor);
  if (is_success == -1) {
    if (verbose) {
      perror("close");
    }
    return mrb_false_value();
  }

  return mrb_true_value();
}

static mrb_value mrb_simple_tcp_client_write(mrb_state *mrb, mrb_value self) {
  mrb_int connection_file_descriptor;
  const char *message;
  mrb_int verbose;

  mrb_get_args(mrb, "izi", &connection_file_descriptor, &message, &verbose);

  int is_success;
  is_success = send(connection_file_descriptor, message, strlen(message), MSG_NOSIGNAL);

  if (is_success == -1) {
    if (verbose) {
      perror("write");
    }
    return mrb_false_value();
  }

  return mrb_true_value();
}

static mrb_value mrb_simple_tcp_client_read(mrb_state *mrb, mrb_value self) {
  mrb_int connection_file_descriptor;

  mrb_get_args(mrb, "i", &connection_file_descriptor);

  char buffer[255];
  int length = 0;

  if ((length = recvfrom(connection_file_descriptor, buffer, 255, 0, NULL, 0)) < 0) {
    if (errno != EWOULDBLOCK) {
      return mrb_false_value();
    }
  } else {
    buffer[length] = '\0';
    return mrb_str_new_cstr(mrb, buffer);
  }

  return mrb_nil_value();
}

static mrb_value mrb_simple_tcp_client_set_non_blocking(mrb_state *mrb, mrb_value self) {
  mrb_int connection_file_descriptor;

  mrb_get_args(mrb, "i", &connection_file_descriptor);

  fcntl(connection_file_descriptor, F_SETFL, fcntl(connection_file_descriptor, F_GETFL, 0) | O_NONBLOCK);

  return mrb_nil_value();
}

void mrb_mruby_simple_tcp_client_gem_init(mrb_state *mrb) {
  struct RClass *lib_module = mrb_define_module(mrb, "SimpleTCP");
  struct RClass *internal = mrb_define_module_under(mrb, lib_module, "Internal");

  mrb_define_class_method(mrb, internal, "connect", mrb_simple_tcp_client_connect, MRB_ARGS_REQ(3));
  mrb_define_class_method(mrb, internal, "disconnect", mrb_simple_tcp_client_disconnect, MRB_ARGS_REQ(2));
  mrb_define_class_method(mrb, internal, "write", mrb_simple_tcp_client_write, MRB_ARGS_REQ(3));
  mrb_define_class_method(mrb, internal, "read", mrb_simple_tcp_client_read, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, internal, "disable_blocking", mrb_simple_tcp_client_set_non_blocking, MRB_ARGS_REQ(1));
}

void mrb_mruby_simple_tcp_client_gem_final(mrb_state *mrb) {
}
