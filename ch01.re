= RailsエンジニアはPhoenixの夢を見るか？

はじめまして、LIDDELL Inc.でリードエンジニア(自称)をしている今(コン)といいます。

一応iOSチームのリーダーなんですが、あまりカッチリした境界があるわけではないのでVue.js書いたりRails書いたりアレやコレやを節操なくワチャワチャしてます。エセフルスタックです。


さて、私からは最近個人的にお熱なElixir製Webフレームワーク「Phoenix」についてお話したいと思います。
弊社ではバックエンドに主としてRailsを導入していますが、ある程度規模が大きくなってきたときに抱えるつらさを感じています。

「Modelが抱える責務が多すぎる…」「大量のテストコードにはウンザリ…」「色々な課題を解決するためにクラス構成拡張しまくったけどこれもうRailsじゃなくてよくね？」
あるあるですね。Railsというフレームワークはミニマムであまりスケール予定のないアプリケーションをサクッと作るには良いのですが、スケーラビリティという面ではいささか頼りない部分があります。
そこで奥さん、Phoenixですよ。
PhoenixはRailsに影響を受けつつも現代風に進化しているので、Railsのつらみを色々と解決してくれています。

== そもそもElixirってどんな言語よ

具体的なRailsとPhoenixの比較に入る前に、まずは簡単にElixirの説明からはじめましょう。
Elixirは動的型付け言語です。動的型付けではありますが、Erlangの実行バイナリににコンパイルすることができます。JVM上で動作するGroovyのようなものですね。
Elixirは関数型言語でもあります。関数型というと、オブジェクト指向エンジニアからは「名前は聞いたことある」「なんか難しそう」といった印象だと思いますが、そんなことないんですよ。ここではとりあえず、「関数型言語には副作用がない(同じ関数に同じ引数を渡せば常に結果は同じになる)」ということだけ覚えておいてください。

さぁ、それではElixirのコードを見ていきましょう。

//list[1][index.exs][elixir]{
defmodule Main do
  def greet(name) do
    IO.puts "Hello, #{String.capitalize(name)}!"
  end
end

#実行結果
> Main.greet("kon")
Hello, Kon!
:ok
//}

めっっっちゃRuby。めっちゃRubyです。特に説明をしなくても上記のコードが何をしているかは一目瞭然でしょう。

何を隠そう、このElixirという言語の作者であるJosé Valim氏はRailsのコアコミッターなのです。ということは、ElixirはRubyを溺愛する人間が新天地を求めて生み出した言語なわけです。これは期待が膨らんでしまいますね。

もう少しElixirらしいコードを見ていきましょう。

//list[2][index.exs][elixir]{

defmodule Main do
  def sum_to(0), do: 0
  def sum_to(n) when is_integer(n) do
    cond do
      n > 0 -> n + sum_to(n - 1)
      n < 0 -> n + sum_to(n + 1)
    end
  end
  def sum_to(_), do: raise "整数以外は使えないよ！"
end

#実行結果
> Main.sum_to(10)
55
//}

ここではパターンマッチ・ガード節・再帰というElixirの強力な武器をたっぷり使ってみました。1つずつ見ていきましょう。

Rubyに慣れた人が見ると、sum_toが複数定義されてるかのように見えるでしょう。
更に、引数に0という具体的な値や謎の_が定義されていることに奇妙さを覚えるのではないでしょうか。実は、これこそがパターンマッチです。
Elixirでは、関数が呼び出されたときに上から順番に該当する関数をチェックしていきます。
その中で最初に引数が条件にマッチしたものを目的の関数として呼び出します。上から見てみましょう。
1つ目のsum_to(0)は、引数が0のときに呼び出されます。0までの和は0と定義します。
2つ目のsum_to(n)は、ここだけを見れば0以外の全ての値にマッチしそうですが、whenの中でnがintegerであることを期待しています。これがガード節です。
3つ目のsum_to(_)は、上2つにマッチしなかった場合、つまり整数以外の全ての値にマッチします。_はあらゆる値にマッチしますが、その値が何にも使われないことを明示的にします。
ここでは整数以外の数に対する処理を定義しないため、引数の値にかかわらず例外を発生させています。
次にsum_to(n)の中身を見ていきましょう。ここでは新たなsum_toを呼び出す再帰処理が行われています。
condはelse ifを並列に書ける文法です。ここではnが0より大きいときにはnより1小さい値を、小さいときには1大きい値を再びsum_toに渡しています。
sum_to(n)は内部で新しいsum_toを呼び続けますが、最終的にはsum_to(0)が呼び出され0を返します。全てのsum_toが実行された後、和が返されます。
具体的には、5を渡すとこのような結果になるのがおわかりでしょうか。

5 + (4 + (3 + (2 + (1 + (0)))))

再帰自体は関数型言語特有のものではありませんが、とりわけ関数型によく見られるテクニックです。というのも、関数型言語には一時変数という概念がなかったり、非推奨だったりするからです。
他言語におけるループに取って代わるものが再帰といえるでしょう。

最後に、僕がとりわけお気に入りの文法、パイプ演算子を紹介しましょう。

//list[3][index.exs][elixir]{
(1..10) |> Enum.map(fn n -> n*n end) |> Enum.filter(fn n -> n < 40 end)
//}

いやぁ、素晴らしいですね。パイプ演算子(|>)は左側の値を右側の関数の第1引数として渡します。まさにUNIX哲学で言うところの「すべてのプログラムをフィルタとして設計する」を体現しているようです。fnはラムダ式を定義しますが、シンタックスシュガーを用いると以下のようにも書けます。
//list[4][index.exs][elixir]{
(1..10) |> Enum.map(&(&1*&1)) |> Enum.filter(&(&1 < 40))
//}

Rubyだったら以下のように書くでしょうか。

//list[5][index.rb][ruby]{
(1..10).map {|n| n*n}.select {|n| n < 40}
//}

書き方としては、Rubyのほうがスッキリしているような印象を受けるかもしれませんね。
しかし、オブジェクト指向における「obj.method(arg)」の実態が「method(self, arg)」であることを考えるとElixirの書き方はプログラムの実態に即していると言えないでしょうか。

== Railsのここがつらいよ！ & Phoenixならこうできるよ！
