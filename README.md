# 2020_DirectX_Project
2020년 4학년 대학교 졸업 작품 프로젝트. 

DirectX를 활용해서 기초적인 렌더링 시스템, ECS(Entity-Component System)을 갖춘 게임 엔진입니다.

'게임은 어떻게 동작하는 것인가' 에 대한 궁금증을 풀기 위해 진행하였습니다.

<img width="512" src="https://github.com/wlsvy/2020_DirectX_Project/blob/master/Image/AnimationBlending.gif"/>
<img width="512" src="https://github.com/wlsvy/2020_DirectX_Project/blob/master/Image/ezgif-7-5df44fa34356.gif"/>

## Feature
- DirectX 11 Backend
- Light System
- SSAO(Screen Space Ambient Occlusion)
- PostProcess(Tone Map, Bloom, Gamma Correction)
- CPU side Frustum Culling
- Shadow Mapping / PCF(Percentge Closer Filtering)
- Skeletal Animation
- Fur Shader
- Physics Based Rendering
- Component System

## Dependencies
- [Dear ImGui](https://github.com/ocornut/imgui)
- [Assimp](https://github.com/assimp/assimp)
- [DirectXTK](https://github.com/Microsoft/DirectXTK)

## Script
- Engine : 엔진의 기본 작동을 관리하는 클래스

> [haeder](https://github.com/wlsvy/2020_DirectX_Project/blob/master/DX_study/DX_study/Script/Internal/Engine/Engine.h) [cpp](https://github.com/wlsvy/2020_DirectX_Project/blob/master/DX_study/DX_study/Script/Internal/Engine/Engine.cpp)

- App : 프로그램 진입점(main) 및, 엔진 초기화 및 업데이트

> [cpp](https://github.com/wlsvy/2020_DirectX_Project/blob/master/DX_study/DX_study/Script/App.cpp)

- DX11Resources : DirectX 11의 자원을 관리

> [header](https://github.com/wlsvy/2020_DirectX_Project/blob/master/DX_study/DX_study/Script/Internal/Engine/DX11Resources.h) [cpp](https://github.com/wlsvy/2020_DirectX_Project/blob/master/DX_study/DX_study/Script/Internal/Engine/DX11Resources.cpp)

- Graphics : DirectX11 자원을 활용해 오브젝트를 렌더링하는 클래스

> [header](https://github.com/wlsvy/2020_DirectX_Project/blob/master/DX_study/DX_study/Script/Internal/Graphics/Graphics.h) [cpp](https://github.com/wlsvy/2020_DirectX_Project/blob/master/DX_study/DX_study/Script/Internal/Graphics/Graphics.cpp)

- Object : GameObject와 Component 등 게임 내 활용되는 객체들의 기본 클래스. ObjectPool<T> 을 통해 관리되며,  MANAGED_OBJECT 는 ObjectPool<T>에서  어떤 타입(T)으로 관리 될지 명시
  
> [header](https://github.com/wlsvy/2020_DirectX_Project/blob/master/DX_study/DX_study/Script/Internal/Core/Object.h) [cpp](https://github.com/wlsvy/2020_DirectX_Project/blob/master/DX_study/DX_study/Script/Internal/Core/Object.cpp)
  
- GameObject : 게임 내 씬(Scene) 상에 존재하는 기본 오브젝트. 게임 오브젝트는 다수의 컴포넌트(Component)와 Transform, RenderInfo 를 포함.

> [header](https://github.com/wlsvy/2020_DirectX_Project/blob/master/DX_study/DX_study/Script/Internal/Core/GameObject.h) [cpp](https://github.com/wlsvy/2020_DirectX_Project/blob/master/DX_study/DX_study/Script/Internal/Core/GameObject.cpp)

- Component : GameObject의 멤버로 포함되며 GameObject가 가지는 특성과 행동을 정의.

> [header](https://github.com/wlsvy/2020_DirectX_Project/blob/master/DX_study/DX_study/Script/Component/Component.h) [cpp](https://github.com/wlsvy/2020_DirectX_Project/blob/master/DX_study/DX_study/Script/Component/Component.cpp)

