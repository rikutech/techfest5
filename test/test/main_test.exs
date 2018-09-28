defmodule TestTest do
  use ExUnit.Case

  test "リクエスト結果として200を返す" do
    assert {:ok, {:result, 200}} = Main.main()
  end
end
