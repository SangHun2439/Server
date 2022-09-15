#pragma once
#include <stack>

extern thread_local uint32 LThreadid;
extern thread_local std::stack<int32> LLockStack;

