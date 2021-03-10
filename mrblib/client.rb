module SimpleTCP
  class Client
    def initialize(blocking: true)
      @blocking = !!blocking
    end

    def connect(host, port)
      raise "Already connected" if @connection_id
      # TODO: convert hostname to ip
      @connection_id = Internal.connect(host, port)

      unless @blocking
        Internal.disable_blocking(@connection_id)
      end

      raise "Failed to connect" unless @connection_id

      true
    end

    def disconnect
      return true unless @connection_id

      if Internal.disconnect(@connection_id)
        @connection_id = nil
        true
      else
        raise "Disconnecting failed"
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

      raise "Unexpected read error" if result == false

      result
    end
  end
end
