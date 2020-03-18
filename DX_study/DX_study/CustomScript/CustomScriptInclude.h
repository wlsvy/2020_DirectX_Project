#pragma once
//GetComponent, AddComponent 초기화할 때 사용하지 말것
//컴포넌트 변수는 무조건 포인터
//생성자/소멸자 쓰지 말것
//구현하느라 죽는 줄 알았네

//클래스 전부 선언, 그 다음 모든 헤더 파일 포함

//기본 컴포넌트 삽입
#include "../Component/BaseComponentInclude.h"
#define NEVER_USE_CONSTRUCTOR(classname) classname(const COMPONENT_INIT_DESC & desc) : ScriptBehaviour(desc) { std::strcpy(mComponentName, #classname);}

class cstest;
class cstest02;
class MemoryBOOM;
class GroundTest;


#include "cstest.h"
#include "cstest02.h"
#include "MemoryBOOM.h"
#include "GroundedTest.h"
