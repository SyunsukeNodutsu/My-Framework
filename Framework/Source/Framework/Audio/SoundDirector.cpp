#include "SoundDirector.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
SoundDirector::SoundDirector()
    : m_spSoundList()
{
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void SoundDirector::Finalize()
{
    AllStop();

    for (auto&& sound : m_spSoundList)
        sound->Release();

    m_spSoundList.clear();
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void SoundDirector::Update()
{
    if (!g_audioDevice->g_xAudio2) return;
    if (!g_audioDevice->g_pMasteringVoice) return;

    for (auto itr = m_spSoundList.begin(); itr != m_spSoundList.end();)
    {
        if ((*itr) == nullptr) continue;

        (*itr)->Update();

        // 除外？
        if (!(*itr)->IsPlaying())
        {
            (*itr)->Stop();// TODO: フェードさせる
            itr = m_spSoundList.erase(itr);
        }
        else
            ++itr;
    }
}

//-----------------------------------------------------------------------------
// 全停止
//-----------------------------------------------------------------------------
void SoundDirector::AllStop()
{
    for (auto&& sound : m_spSoundList)
        sound->Stop();
}

//-----------------------------------------------------------------------------
// 再生
//-----------------------------------------------------------------------------
bool SoundDirector::Play(const std::string& filepath, DWORD delay, float volume, bool loop, bool useFilter)
{
    auto sound = std::make_shared<SoundWork>();
    if (!sound) return false;

    if (sound->Load(filepath, loop, useFilter) == false) {
        DebugLog("読み込み失敗.\n");
        return false;
    }

    AddSoundList(sound);

    sound->Play(delay);
    sound->SetVolume(volume);

    return true;
}

//-----------------------------------------------------------------------------
// 3D再生
//-----------------------------------------------------------------------------
bool SoundDirector::Play3D(const std::string& filepath, const float3 pos, DWORD delay, float volume, bool loop, bool useFilter)
{
    auto sound = std::make_shared<SoundWork3D>();
    if (!sound) return false;

    if (sound->Load(filepath, loop, useFilter) == false) {
        DebugLog("読み込み失敗.\n");
        return false;
    }

    AddSoundList(sound);

    sound->Play3D(pos, delay);
    sound->SetVolume(volume);

    return true;
}

//-----------------------------------------------------------------------------
// ゲームサウンドの作成
//-----------------------------------------------------------------------------
std::shared_ptr<SoundWork> SoundDirector::CreateSoundWork(const std::string& filepath, bool loop, bool useFilter)
{
    auto ret = std::make_shared<SoundWork>();
    if (!ret) return nullptr;
    
    if (ret->Load(filepath, loop, useFilter) == false)
    {
        DebugLog("読み込み失敗.\n");
        return nullptr;
    }

    AddSoundList(ret);
    return ret;
}

//-----------------------------------------------------------------------------
// 3Dゲームサウンドの作成
//-----------------------------------------------------------------------------
std::shared_ptr<SoundWork3D> SoundDirector::CreateSoundWork3D(const std::string& filepath, bool loop, bool useFilter)
{
    auto ret = std::make_shared<SoundWork3D>();
    if (!ret) return nullptr;

    if (ret->Load(filepath, loop, useFilter) == false)
    {
        DebugLog("読み込み失敗.\n");
        return nullptr;
    }

    AddSoundList(ret);
    return ret;
}

//-----------------------------------------------------------------------------
// サウンドリストに登録
//-----------------------------------------------------------------------------
void SoundDirector::AddSoundList(std::shared_ptr<SoundWork> sound)
{
    if (!g_audioDevice->g_xAudio2) return;
    if (!g_audioDevice->g_pMasteringVoice) return;

    if (sound) m_spSoundList.push_back(sound);
}
