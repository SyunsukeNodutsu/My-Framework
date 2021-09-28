#pragma once

class BasicInput
{
public:

	BasicInput() = delete;
	~BasicInput() = delete;

	static bool IsKeyDown(int keycode);
	static bool IsKeyUp(int keycode);
	static bool IsKeyPressed(int keycode);

private:


};
