= RailsエンジニアはPhoenixの夢を見るか？

はじめまして、LIDDELL Inc.でリードエンジニア(自称)をしている今(@rikutech)といいます。

一応iOSチームのリーダーなんですが、あまりカッチリした境界があるわけではないのでVue.js、Rails、Laravelらへんも触ったりアレやコレやを節操なくワチャワチャしてます。エセフルスタックです。

さて、私からは最近個人的にお熱なElixir製Webフレームワーク「Phoenix」についてお話したいと思います。
弊社の一部サービスではバックエンドにRailsを導入していますが、ある程度規模が大きくなってきたときに抱えるつらさを感じつつあります。

「Modelが抱える責務が多すぎる…」「大量のテストコードにはもうウンザリ…」「色々な課題を解決するためにクラス構成拡張しまくったけどこれもうRailsじゃなくてよくね？」
あるあるですね。Railsというフレームワークはミニマムであまりスケール予定のないアプリケーションをサクッと作るには良いのですが、スケーラビリティという面ではいささか頼りない部分があります。

そこで奥さん、Phoenixですよ。
PhoenixはRailsに影響を受けつつも現代風に進化しているので、Railsのつらみを色々と解決してくれています。

== そもそもElixirってどんな言語よ

具体的なRailsとPhoenixの比較に入る前に、まずは簡単にElixirの説明からはじめましょう。
ElixirはErlang VM上で動作する動的型付け言語です。他の動的言語同様、スクリプトとして実行可能な一方、Erlangの実行バイナリにコンパイルすることができます。都度実行する書き捨てファイルやテストファイルはスクリプトとして、実用コードはコンパイルして使うのがセオリーです。
Elixirは関数型言語でもあります。関数型というと、オブジェクト指向エンジニアからは「なんか難しそう」といった印象だと思いますが、そんなことないんですよ。ここではとりあえず、「関数型言語には副作用がない(同じ関数に同じ引数を渡せば常に結果は同じになる)」ということだけ覚えておいてください。

さぁ、それではElixirのコードを見ていきましょう。

//list[1][index.exs][elixir]{
defmodule Main do def greet(name) do
    IO.puts "Hello, #{String.capitalize(name)}!"
  end
end

#実行結果
# > Main.greet("rikutech")
# Hello, Rikutech!
# :ok
//}

めっっっちゃRuby。めっちゃRubyです。特に説明をしなくても上記のコードが何をしているかは一目瞭然でしょう。

何を隠そう、このElixirという言語の作者であるJosé Valim氏はRailsのコアコミッターなのです。ということは、ElixirはRubyを溺愛する人間が新天地を求めて生み出した言語なわけです。これは期待が膨らんでしまいますね。
ちなみにElixirという名前はFINAL FANTASY由来です。José氏がFFファンだそう。僕はドラクエ派です。

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
end

# 実行結果
# > Main.sum_to(10)
# 55
//}

ここではパターンマッチ・ガード節・再帰というElixirの強力な武器をたっぷり使ってみました。1つずつ見ていきましょう。

Rubyに慣れた人が見ると、sum_toが複数定義されてるかのように見えるでしょう。
更に、引数に0という具体的な値や謎の_が定義されていることに奇妙さを覚えるのではないでしょうか。実は、これこそがパターンマッチです。
Elixirでは、関数が呼び出されたときに上から順番に該当する関数をチェックしていきます。
その中で最初に引数が条件にマッチしたものを目的の関数として呼び出します。上から見てみましょう。
1つ目のsum_to(0)は、引数が0のときに呼び出されます。0までの和は0と定義します。
2つ目のsum_to(n)は、ここだけを見れば0以外の全ての値にマッチしそうですが、whenの中でnがintegerであることを期待しています。これがガード節です。
次にsum_to(n)の中身を見ていきましょう。ここでは新たなsum_toを呼び出す再帰処理が行われています。
condはelse ifを並列に書ける文法です。ここではnが0より大きいときにはnより1小さい値を、小さいときには1大きい値を再びsum_toに渡しています。
sum_to(n)は内部で新しいsum_toを呼び続けますが、最終的にはsum_to(0)が呼び出され0を返します。全てのsum_toが実行された後、和が返されます。
具体的には、5を渡すとこのような結果になるのがおわかりでしょうか。

5 + (4 + (3 + (2 + (1 + (0)))))

再帰自体は関数型言語特有のものではありませんが、とりわけ関数型によく見られるテクニックです。というのも、たとえローカル変数であろうと状態を持たせると
他言語におけるループに取って代わるものが再帰といえるでしょう。

最後に、僕がとりわけお気に入りの文法、パイプ演算子を紹介しましょう。

//list[3][index.exs][elixir]{
(1..10) |> Enum.map(fn n -> n*n end) |> Enum.filter(fn n -> n < 40 end)
# 実行結果
# > [1, 4, 9, 16, 25, 36]
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
しかし、オブジェクト指向における「obj.method(arg)」の実態が「method(self, arg)」でしかないことを考えるとElixirの書き方はプログラムの実態に即していると言えないでしょうか。

Elixirの基本的な文法については以上です。
それでは、RailsとPhoenixの比較に入りましょう。

== Railsのここがつらいよ！ & Phoenixならこうできるよ！

Railsは既に14年の歴史があるWebフレームワークです。
Convention over Configuration(設定より規約)を掲げ、圧倒的に自由な記述ができるRubyにWeb開発のレール(規約)を敷き典型的なCRUDアプリケーションを高速開発できる、当時からしたら画期的なフレームワークでした。
Rubyの今日の地位は、Railsによるものと言っても過言ではないでしょう。

しかし、この十数年でWebの世界はより繁栄し、複雑化や多様化が進んできました。
多様な消費者ニーズに答えるため、複雑なビジネス要件をアプリケーションの文脈に落とし込む必要がでてきたのです。
それは私や、もしかしたらこれを読んでいる貴方が在籍しているかもしれない小〜中規模のベンチャーにとっても同様で、シンプルで簡単なプロダクトを扱うとは限らなくなってきたのです。
更に、膨大なリクエスト数を並列に捌いたり、高速なレスポンスが求められるようにもなっています。
つまり、RailsのようなモノリシックでシンプルなCRUDアプリケーションに特化したフレームワークは、ベンチャー企業であっても最適解とは言えなくなっているのです。
上記の事実を踏まえると、生産性と並列性、実行速度、スケーラビリティなどを兼ね備えたフレームワークが最強ということになりますが…そんなフレームワークがあるわけ…

あったわ。Phoenixがあったわ。
そうなんです。Phoenix、全部あるんです。1つずつ見ていきましょう。

=== Phoenixのイケてるところ①: Elixirでできている
はい。バカにしてんのかと思ったかもしれませんが、ぶっちゃけこれが一番のメリットです。
そもそもElixirって言語自体がWebに向いた特性を備えています。正確にはErlang VMが、です。

Erlang VMの最大の特徴は、なんといっても独自のプロセス実装にあります。
Erlang VMはOSレベルのプロセスを使用せず、自身のランタイム上で動作するプロセスを実装しています。
プロセスはとても軽量で、1つで2.5KBほどしかメモリを必要としません。
そして、プロセス同士は決して同じメモリ空間を使用しないことが保証されています。
更に更に、ガベージコレクションもプロセス単位なんです！
以上の特性から、共倒れさせたくない複数の処理を実行するときにはそれぞれ独立したプロセスをポンポン作ってしまう、なんてことが可能になるんです。
リクエストごとにプロセスを立てれば、あるユーザーからの1リクエストがコケたとしても他のリクエストには一切影響を及ぼしません。なんて素晴らしい。
要するに、非常に優れた対障害性を持っているのです。

更に、パフォーマンスも中々イケています。
Railsの10倍(！)の速度を誇るだけでなく、他の静的言語製フレームワークとも割といい勝負をします。
参考にPhoenixと他フレームワークのベンチマーク比較を貼っておきます。
TODO:URLはQRコードも貼りたい
https://github.com/mroth/phoenix-showdown
Erlang VM自体は特に動作速度に優れているわけではありませんが、プロセスを並列に走らせることができるおかげでGoや相手にもJava健闘しています。これは性能のいいマシン上で動かしているほど顕著になります。

普通にPhoenixに乗っかってWebアプリケーションを開発するだけでこれらの恩恵を受けられるヤバさがおわかりでしょうか。

=== Phoenixのイケてるところ②: 構造がモダン
ものづくりの世界ではえてして後発のものほど先発の反省をいかして優れた造りになるものです(Lisp教徒に怒られそうなのでLispは例外と言っておきます)。
Erlang VMの性能やElixirの言語構造が優れているだけでなく、Phoenix自体もモダンでイケイケな構造をしています。
RailsのScaffoldのようなコマンドがPhoenixにもあるので、それを利用して全体像をさらってみましょう。

//list[6][console][]{
$ mix phx.gen.html Accounts User users name:string age:integer
//以下のファイルが生成される
lib/app/accounts/user.ex
lib/app/accounts/accounts.ex
lib/app_web/controllers/user_controller.ex
lib/app_web/templates/user/edit.html.eex
lib/app_web/templates/user/form.html.eex
lib/app_web/templates/user/index.html.eex
lib/app_web/templates/user/new.html.eex
lib/app_web/templates/user/show.html.eex
lib/app_web/views/user_view.ex
test/app_web/controllers/user_controller_test.exs
test/app/accounts/accounts_test.exs
priv/repo/migrations/20180922061853_create_users.exs
//}

appとapp_webという二種類のディレクトリがあることにお気づきでしょうか。
Phoenixではビジネスロジック部とwebアプリケーション部を明確に切り離しています。
「webの仕組みってあくまでインターフェースでしかなくて、重要なのはアプリケーション固有のビジネスロジックのほうだよね」という考えは普遍的なものではありますが、ディレクトリ構成レベルでそれが実現されているのは気持ちのいいものがあります。

それでは、リクエストの流れに沿って見ていきましょう。
まずはrouterです。

//list[7][router.ex][elixir]{
defmodule AppWeb.Router do
  use AppWeb, :router

  pipeline :browser do
    plug :accepts, ["html"]
    plug :fetch_session
    plug :fetch_flash
    plug :protect_from_forgery
    plug :put_secure_browser_headers
  end

  pipeline :api do
    plug :accepts, ["json"]
  end

  scope "/", AppWeb do
    pipe_through :api

    resources "/users", UserController, except: [:new, :edit]
  end
end
//}

Railsのrouterに近いものの、なにやらpipelineとかplugとかゴチャゴチャ書いてありますね。
PhoenixはPlugというElixir公式の軽量HTTPサーバ実装をフルに利用しています。
RailsがRackを利用しているのと同じ関係と言えるでしょう。
パイプラインはルーティングに従ってControllerのactionを実行する前に、リクエスト内容を精査したり、actionに渡す情報を追加したりするフィルターとして機能します。browserパイプラインがhtmlだけを受け入れたり、セッションを取得したり、CSRFチェックをしたりしているのがなんとなく分かると思います。これら1つ1つがplug(ややこしい)です。
plugは自分で簡単に定義・使用することができるので、実用アプリでは認証機能やIP制限等が容易に実装できます。
Rackにもmiddlewareという名前で似たような機能がありますが、特定のルーティング単位で直感的に適用できるという点でPlugのほうが優れていると言えないでしょうか。もうcurrent_userやredirect_unless_authenticatedみたいなメソッドを親Controllerに実装して継承でつらい思いをしなくていいんです。
とはいえ、Laravel等の新興フレームワークのmiddlewareはPlugに近い実装になっているので、モダンなフレームワークなら持っている機能といえるでしょう。

次にControllerです。

//list[8][user_controller.ex][elixir]{
defmodule AppWeb.UserController do
  use AppWeb, :controller

  alias App.Accounts
  alias App.Accounts.User

  action_fallback AppWeb.FallbackController

  def index(conn, _params) do
    users = Accounts.list_users()
    render(conn, "index.json", users: users)
  end

  def create(conn, %{"user" => user_params}) do
    with {:ok, %User{} = user} <- Accounts.create_user(user_params) do
      conn
      |> put_status(:created)
      |> put_resp_header("location", user_path(conn, :show, user))
      |> render("show.json", user: user)
    end
  end

  def show(conn, %{"id" => id}) do
    user = Accounts.get_user!(id)
    render(conn, "show.json", user: user)
  end

  def update(conn, %{"id" => id, "user" => user_params}) do
    user = Accounts.get_user!(id)

    with {:ok, %User{} = user} <- Accounts.update_user(user, user_params) do
      render(conn, "show.json", user: user)
    end
  end

  def delete(conn, %{"id" => id}) do
    user = Accounts.get_user!(id)
    with {:ok, %User{}} <- Accounts.delete_user(user) do
      send_resp(conn, :no_content, "")
    end
  end
end
//}

connやwithといった見慣れないワードが出てきましたね。軽く説明を。
各actionが第一引数で受け取っているconnはconnectionの略で、リクエスト情報が詰まった構造体です。
先程説明したplugはこのconnをバケツリレーしてこれらのactionまで渡してくれるのです。
with文は「with パターン <- 式」のように使います。成功したらdoの中を、失敗したら式を返します。
do内では正常系を記述しています。
失敗した場合、action_fallbackで指定されているFallbackControllerに処理が落ちていき、404等の適切なステータスを返すようになっています。
その他の記述はなんとなく理解できるのではないでしょうか。RailsのControllerに比べると少しだけ記述が多いかもしれませんが、逆に言うと大事な部分を隠蔽化せずに明示的にしているということでもあります。

次はModelですが、Phoenixではversion1.3からModelの考え方が大きく変わっています。
Railsと対比しながら見ていきます。

RailsにおけるModel、ActiveRecordが中々つらいことには皆さんウンウンと頷いてくれることでしょう。
ActiveRecordを使うと、必然的にModelとテーブルが1対1になります。
この制約によりビジネスロジックがテーブル構造に依存し、えてして歪みを生みます。
更には、scopeやcallback、validationもModel内に書けてしまうため、何も考えずに処理を書いていくとすぐにFatModelになってしまいます。後からリファクタリングしようと思っても、オブジェクト指向ゆえの複雑な状態が邪魔をして中々難しいものがあります。南無三。

さぁ、Phoenixに話を戻しましょう。
Phoenixは元々Railsに近いModel構造を採用していましたが、version1.3からはContextという概念を採用しています。
ドメイン駆動設計を知っている方はContextという言葉に馴染みがあると思いますが、ここで言うContextは、DDDで言うところのContextとは別物であることに注意してください。
カタログと購入のシステムを分けるための仕組みというより、注文のデータと、それに紐づく商品データや決済データといったスキーマをまとめ上げて「購入」という形で取り扱うような、集約に近いものになります。
ここではAccounts.exがContext、user.exがスキーマになります。

user.exファイルから見ていきましょう。

//list[9][user.ex][elixir]{
defmodule App.Accounts.User do
  use Ecto.Schema
  import Ecto.Changeset

  schema "users" do
    field :age, :integer
    field :name, :string

    timestamps()
  end

  def changeset(user, attrs) do
    user
    |> cast(attrs, [:name, :age])
    |> validate_required([:name, :age])
  end
end
//}

Ectoというデータマッピングとクエリを扱えるライブラリを利用しています。
Ectoのschema関数を用いてdoブロック内でスキーマを定義しています。
Phoenixではコンテキストごとにスキーマを定義できるので、あるコンテキストからは不要なカラムを隠したり、意味合いが変わってくるカラムの名前を変更したりできます。イケイケですね。
schema関数の実態はマクロで、受け取ったfield情報を元にこのmodule内にuser構造体を定義してくれます。
ActiveRecordが内部でメタプログラミングを用いてメソッド定義しているのに近いですね。
ただEctoのほうが明示的で必要最小限の機能だけを提供してくれるところが私は気に入っています。

changeset関数はuser構造体とattributesを受け取りvalidateし、changeset(新規作成もしくは更新のための差分)構造体を返します。

スキーマはこれだけです。次は集約であるaccounts.exファイルを見てみましょう。

//list[10][accounts.ex][elixir]{
defmodule App.Accounts do
  import Ecto.Query, warn: false
  alias App.Repo

  alias App.Accounts.User

  def list_users do
    Repo.all(User)
  end

  def get_user!(id), do: Repo.get!(User, id)

  def create_user(attrs \\ %{}) do
    %User{}
    |> User.changeset(attrs)
    |> Repo.insert()
  end

  def update_user(%User{} = user, attrs) do
    user
    |> User.changeset(attrs)
    |> Repo.update()
  end

  def delete_user(%User{} = user) do
    Repo.delete(user)
  end

  def change_user(%User{} = user) do
    User.changeset(user, %{})
  end
end
//}

これだとCRUDだけのシンプルなものなので、Phoenixの利点は分かりづらいかもしれません。しかし、サービスを作れば必ずといっていいほど複数のスキーマ(テーブル)が関連を持つことになるので、最初からこの構造なのは嬉しいところ。
まぁ元も子もないこと言っちゃえばこの辺の構造は自分で好きにカスタマイズできるんですけどね。
一定の関数群にまとまった名前をつけてmoduleにしてしまうだけで切り出せるので。状態に依存しない関数型言語だからこそできることです。関数を分割したり纏めたり切り出したりする類のリファクタリングが非常に容易なので色々試行錯誤しやすいです。

さぁ、controllerを通してmodelを取得し、あとはviewを生成するだけです。user_view.exを見ていきましょう。

//list[11][user_view.ex][elixir]{
defmodule HelloWeb.UserView do
  use HelloWeb, :view
  alias HelloWeb.UserView

  def render("index.json", %{users: users}) do
    %{data: render_many(users, UserView, "user.json")}
  end

  def render("show.json", %{user: user}) do
    %{data: render_one(user, UserView, "user.json")}
  end

  def render("user.json", %{user: user}) do
    %{id: user.id,
      name: user.name,
      age: user.age}
  end
end
//}


=== Phoenixのイケてるところ③: リアルタイムWebも任せろ
近年、リアルタイムなチャット機能やユーザー同士のリアクションの送受信など、リアルタイムWebが求められる場面が増えているかと思います。
RailsでリアルタイムWebといえばActionCableですよね。
数行の記述でWebsocketがアッサリ使えて非常に便利ではあるのですが、パフォーマンス面では微妙なところです。
GoやErlang VMでのWebsocket実装との比較を見てみましょう。
https://evilmartians.com/chronicles/anycable-actioncable-on-steroids
ActionCableは1000接続の時点で既に少々怪しく、10000接続にもなると10秒のレイテンシが発生し「リアルタイムとは」というレベルのパフォーマンスになってしまいます。悲しい。
一方のErlang VM、やりおるマンですね。10000接続でもレイテンシは1秒ほどと、バツグンの安定感です。
PhoenixのWebsocket実装であるChannelsそのものとActionCableの良いベンチマーク比較が見つからなかったためErlang VMとの比較になってしまっていますが、ChannelsはErlang VM以外に依存していないので実際のパフォーマンスと大きく乖離していることはないでしょう。
なおかつChannelsの利用しやすさはActionCableと大差ありません。
もう(Channels使うしか)ないじゃん…

=== Phoenixのイケてるところ④: 圧倒的テスタビリティ
