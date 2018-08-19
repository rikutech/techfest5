defmodule Main do
  def greet(name) do
    IO.puts "Hello, #{String.capitalize(name)}!"
  end

  def sum_to(0), do: 0
  def sum_to(n) when is_integer(n) do
    cond do
      n > 0 -> n + sum_to(n - 1)
      n < 0 -> n + sum_to(n + 1)
    end
  end
  def sum_to(_), do: raise "整数以外は使えないよ！"

  def main do
    (1..10) |> Enum.map(&(&1*&1)) |> Enum.filter(&(&1 < 40))
  end
end

