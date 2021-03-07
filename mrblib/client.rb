module ThreadedTCP
  class Client
    def connect(host, port)
      raise "Already connected" if @connection_id
      # TODO: convert hostname to ip
      @connection_id = Internal.connect(host, port)
      raise "Failed to connect" unless @connection_id
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

    # TODO: fix longer messages e.g. "hello world\n" doesn't work
    def write(message)
      Internal.write(@connection_id, message)
    end

    def read(&blk)
      Internal.read(@connection_id, &blk)
    end
  end
end
