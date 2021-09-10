
module SimpleTCP
  class AlreadyConnectedError < StandardError; end
  class FailedToConnectError < StandardError; end
  class FailedToDisconnectError < StandardError; end
  class UnexpectedReadError < StandardError; end
  class UnexpectedWriteError < StandardError; end
  class DisconnectedError < StandardError; end
  class ReadWhenDisconnectedError < DisconnectedError; end
  class WriteWhenDisconnectedError < DisconnectedError; end

  class Client
    def initialize(blocking: true, verbose: false)
      @blocking = !!blocking
      @verbose = verbose ? 1 : 0
    end

    def connect(host, port)
      raise AlreadyConnectedError if @connection_id

      if host[0].to_i > 0
        host = Internal.resolve_host(host)
      end
      @connection_id = Internal.connect(host, port, @verbose)

      raise FailedToConnectError unless @connection_id

      unless @blocking
        Internal.disable_blocking(@connection_id)
      end

      true
    end

    def connected?
      !!@connection_id
    end

    def disconnect(error_on_failure = true)
      return true unless @connection_id

      if Internal.disconnect(@connection_id, @verbose)
        true
      elsif error_on_failure
        raise FailedToDisconnectError
      else
        false
      end
    ensure
      @connection_id = nil
    end

    def write(message)
      raise WriteWhenDisconnectedError unless connected?

      result = Internal.write(@connection_id, message, @verbose)

      if result == false
        disconnect(false)
        raise UnexpectedWriteError
      end

      result
    end

    def read
      raise ReadWhenDisconnectedError unless connected?

      result = Internal.read(@connection_id)

      if result == false
        disconnect(false)
        raise UnexpectedReadError
      end

      result
    end
  end
end
