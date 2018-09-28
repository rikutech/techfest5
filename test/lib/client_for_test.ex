defmodule ClientForTest do
  def request do
    IO.puts "オンメモリから結果を返します"
    {:ok, {:result, 200}}
  end
end
