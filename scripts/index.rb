def sum_to(n)
  raise "整数以外は使えないよ！" unless n.instance_of?(Integer)
  return 0 if n == 0

  if n > 0
    range = (0..n)
  elsif n < 0
    range = (n..0)
  end
  range.each {|m| sum += m}
  sum
end
