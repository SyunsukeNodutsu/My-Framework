-----------------------------------------------
-- �g�ݍ��݃e�X�g
-- ����.�V�t�gJIS�ŕۑ����Ȃ��Ɠ��{�ꂪ�\���ł��Ȃ�
-----------------------------------------------

-- �R���[�`�����g�p���� �������\�����Ă݂�
function step()
    coroutine.yield("�����͍L�ꂾ�����B")
    coroutine.yield("�����Ȋ���䂪�������B")
    coroutine.yield("�̂����ŗǂ��V�񂾎����v���o�����B")
end

-- �o�ߎ��Ԏ擾
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