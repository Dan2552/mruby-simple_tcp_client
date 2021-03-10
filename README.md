# Simple TCP mruby gem (mrbgem)

Plain and simple mruby TCP client.

# Installation

Put the following into your `Mundlefile`([?](https://github.com/Dan2552/mundler)) or `build_config.rb`:
``` ruby
conf.gem :git => 'https://github.com/Dan2552/mruby-simple_tcp_client.git', :branch => 'main'
```

# Usage

``` ruby
client = SimpleTCP::Client.new
client.connect("127.0.0.1", 3000)
client.write("hello\n")
message = client.read
client.disconnect
```

The client can also be initialized to be non-blocking. This is useful if your application cannot wait for a response, without needing to utilize threads (i.e. `#read` will become non-blocking).

``` ruby
client = SimpleTCP::Client.new(blocking: false)
```
