//-----------------------------------------------------------------------------
// File: KeyCode.h
//
// 入力判定の際に使用するキーコードを定義
// 必要になった時に順次追加していく
//-----------------------------------------------------------------------------
#pragma once

//キーコード ※仮想キーコードと同じ値
namespace KeyCode
{
    enum : unsigned char
    {
        None = 0,

        Back = 0x8,
        Tab = 0x9,
        Shift = 0x10,// TODO: 左右判定ができない
        Control = 0x11,
        Alt = 0x12,// Alt(Menu)

        Enter = 0xd,

        Escape = 0x1b,

        Space = 0x20,

        Left = 0x25,
        Up = 0x26,
        Right = 0x27,
        Down = 0x28,

        K_1 = 0x31,
        K_2 = 0x32,
        K_3 = 0x33,
        K_4 = 0x34,
        K_5 = 0x35,
        K_6 = 0x36,
        K_7 = 0x37,
        K_8 = 0x38,
        K_9 = 0x39,

        A = 0x41,
        B = 0x42,
        C = 0x43,
        D = 0x44,
        E = 0x45,
        F = 0x46,
        G = 0x47,
        H = 0x48,
        I = 0x49,
        J = 0x4a,
        K = 0x4b,
        L = 0x4c,
        M = 0x4d,
        N = 0x4e,
        O = 0x4f,
        P = 0x50,
        Q = 0x51,
        R = 0x52,
        S = 0x53,
        T = 0x54,
        U = 0x55,
        V = 0x56,
        W = 0x57,
        X = 0x58,
        Y = 0x59,
        Z = 0x5a,

        F1 = 0x70,
        F2 = 0x71,
        F3 = 0x72,
        F4 = 0x73,
        F5 = 0x74,
        F6 = 0x75,
        F7 = 0x76,
        F8 = 0x77,
        F9 = 0x78,
        F10 = 0x79,
        F11 = 0x7a,
        F12 = 0x7b,
    };
}

//マウスのコード これを基準にして各入力デバイスに設定します
namespace MouseKode
{
    enum : unsigned char
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
    };
}
