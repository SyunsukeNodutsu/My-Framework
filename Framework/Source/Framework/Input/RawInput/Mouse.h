//-----------------------------------------------------------------------------
// File: Mouse.h
//
// RawInputAPIによるマウス管理
//-----------------------------------------------------------------------------
#pragma once

enum ButtonState : char { UP, HELD, RELEASED, PRESSED };

// 判定するマウスボタン
typedef enum
{
    Left,
    Right,
    Middle,
    X1,
    X2,

    Count,

    Mask = 7,

    AxisX = 8,
    AxisY = 16,
    AxisXY = AxisX | AxisY
} MouseButton;

// RawInputによるマウス管理クラス
class Mouse
{
public:

    // @brief コンストラクタ
    Mouse();

    // @brief 初期化
    // @param hwnd アプリケーションのウィンドウハンドル
    void Initialize(HWND hwnd);

    // @brief 終了
    // @param hwnd アプリケーションのウィンドウハンドル
    void Finalize(HWND hwnd);

    // @brief 次の入力判定の準備
    void Refresh();

    // @brief マウス入力情報を解析する
    // @param mouse 解析するマウスデータ
    // @param hwnd アプリケーションのウィンドウハンドル
    void ParseMouseData(RAWMOUSE mouse, HWND hwnd);

    // @brief マウスホイールの変化量を設定する
    // @param delta 変化量
    void SetMouseWheelDelta(int delta) { m_mouseWheelDelta = delta; }

    // マウス座標を設定
    void SetAt(float2 position, HWND hwnd = nullptr, bool abs = false);

    //--------------------------------------------------
    // マウスボタン判定
    //--------------------------------------------------

    // @brief キーが押されている間を返す
    // @param mkey 判定したいボタン
    // @return 押されている...true 押されていない...false
    bool IsDown(MouseButton mkey);

    // @brief キーが押された瞬間を返す
    // @param mkey 判定したいボタン
    // @return 押した瞬間...true それ以外...false
    bool IsPressed(MouseButton mkey);

    // @brief キーが離された瞬間を返す
    // @param mkey 判定したいボタン
    // @return 離された瞬間...true それ以外...false
    bool IsReleased(MouseButton mkey);

    // @brief マウス座標を返す
    // @param haste 解析を待たず、即座に取得？
    // @return マウス座標(全体)
    float2 GetMousePos(bool haste = false);

    // @brief マウスホイールの変化量を返す
    // @return マウスホイールの変化量
    int GetMouseWheelDelta() { return m_mouseWheelDelta; }

private:

    struct State {
        union {
            struct { uint32_t val; };
            struct {
                uint32_t mLeftButton : 1;
                uint32_t mRightButton : 1;
                uint32_t mMiddleButton : 1;
                uint32_t mX1Button : 1;
                uint32_t mX2Button : 1;
            };
        };
    };

    struct {
        ButtonState mLeftButton;
        ButtonState mRightButton;
        ButtonState mMiddleButton;
        ButtonState mX1Button;
        ButtonState mX2Button;

        ButtonState KeyState(MouseButton key) const {
            switch (key) {
            case MouseButton::Left:     return mLeftButton;
            case MouseButton::Right:    return mRightButton;
            case MouseButton::Middle:   return mMiddleButton;
            case MouseButton::X1:       return mX1Button;
            case MouseButton::X2:       return mX2Button;
            }
            return ButtonState::UP;
        }
    } _mStates;

private:

    // マウスデバイス
    RAWINPUTDEVICE  m_deviceMouse;

    // マウス状態
    State m_state;      // 現在のマウス状態
    State m_lastState;  // 1フレーム前のマウス状態

    // マウス座標
    float2 m_mousePos;

    // マウスホイールの変化量
    int m_mouseWheelDelta = 0;

    HWND m_owner;

private:

    // @brief 状態の設定 (save bit)
    // @param wparam wparamメッセージ
    // @param isDown 押されているかどうか
    void SetState(WPARAM wparam, bool isDown);

    // @brief 状態の設定 (mouse button state)
    // @param flags RAWMOUSEのbuttonData
    void SetState(ULONG flags);

};
