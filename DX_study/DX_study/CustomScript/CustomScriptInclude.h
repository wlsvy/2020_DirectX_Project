#pragma once
//GetComponent, AddComponent �ʱ�ȭ�� �� ������� ����
//������Ʈ ������ ������ ������
//������/�Ҹ��� ���� ����
//�����ϴ��� �״� �� �˾ҳ�

//Ŭ���� ���� ����, �� ���� ��� ��� ���� ����

//�⺻ ������Ʈ ����
#include "../Component/BaseComponentInclude.h"
#define NEVER_USE_CONSTRUCTOR(classname) classname(GameObject_v2 & obj) : ScriptBehaviour(obj) { std::strcpy(Name, #classname);}

class cstest;
class cstest02;
class MemoryBOOM;
class GroundTest;


#include "cstest.h"
#include "cstest02.h"
#include "MemoryBOOM.h"
#include "GroundedTest.h"
