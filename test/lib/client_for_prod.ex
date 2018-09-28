defmodule ClientForProd do
  def request do
    IO.puts "APIサーバーにリクエストします"
    {:ok, {:result, 200}}
  end
end
