defmodule Main do
  @client Application.get_env(:app, :client)

  def main do
    IO.inspect @client.request
  end
end
