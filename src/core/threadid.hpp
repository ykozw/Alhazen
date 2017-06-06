#pragma once

namespace Job
{
/*
0がmain()のスレッド
別にスレッドが起動されたら1から連番を振る
無効なスレッドの場合は-1
*/
extern __declspec(thread) uint32_t threadId;
};
