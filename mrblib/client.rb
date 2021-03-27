module SimpleTCP
  class AlreadyConnectedError < StandardError; end
  class FailedToConnectError < StandardError; end
  class FailedToDisconnectError < StandardError; end
  class UnexpectedReadError < StandardError; end

  class Client
    def initialize(blocking: true)
      @blocking = !!blocking
    end

    def connect(host, port)
      raise AlreadyConnectedError if @connection_id
      # TODO: convert hostname to ip
      @connection_id = Internal.connect(host, port)

      raise FailedToConnectError unless @connection_id

      unless @blocking
        Internal.disable_blocking(@connection_id)
      end

      true
    end

    def connected?
      !!@connection_id
    end

    def disconnect
      return true unless @connection_id

      if Internal.disconnect(@connection_id)
        @connection_id = nil
        true
      else
        raise FailedToDisconnectError
      end
    end

    def write(message)
      Internal.write(@connection_id, message)
    end

    def read
      result =
        if @blocking
          Internal.blocking_read(@connection_id)
        else
          Internal.non_blocking_read(@connection_id)
        end

      raise UnexpectedReadError if result == false

      result
    end
  end
end
