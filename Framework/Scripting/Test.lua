-----------------------------------------------
-- 組み込みテスト
-- 注意.シフトJISで保存しないと日本語が表示できない
-----------------------------------------------

-- コルーチンを使用して 文字列を表示してみる
function step()
    coroutine.yield("そこは広場だった。")
    coroutine.yield("小さな滑り台があった。")
    coroutine.yield("昔ここで良く遊んだ事を思い出した。")
end

-- 経過時間取得
function cy()
    local T = os.time()
    coroutine.yield(coroutine.resume(coroutine.create(function() end)))
    return os.time() - T
end

-- System Sleep
function sleep(time)
    if not time or time == 0 then
        time = cy()
    end

    local t = 0
    repeat
        local T = os.time()
        coroutine.yield(coroutine.resume(coroutine.create(function() end)))
        t = t + (os.time() - T)
    until t >= time
end