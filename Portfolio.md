
# DirectX11 개인 프로젝트

## 기본 아키텍쳐 구성

<details>
  <summary>접기/펼치기</summary>
  
![EngineArchitecture](https://github.com/wlsvy/2020_DirectX_Project/blob/master/Image/EngineArchitecture.png)

- Engine : 프로그램은  가장 기본이 되는 클래스 입니다. 아래의 두 클래스 객체를 멤버로 가집니다.
  - GameObject 렌더링을 담당하는 Graphics 클래스 
  - GameObject 객체를 포함하는 Scene 클래스

![GameObject](https://github.com/wlsvy/2020_DirectX_Project/blob/master/Image/GameObject.png)

- GameObject : 게임 내 세계에서 생성되어 행동하는 기본 클래스입니다.
   - Component를 상속받은 클래스를 활용해서 게임 오브젝트의 특성이나 동작을 정의할 수 있도록 구현하였습니다.
   

---
**GameObject 의 행동과 특성들을 정의하는 방식**은 크게 두 가지 방법이 있습니다. 

1. GameObject를 상속받는 새로운 클래스 작성
    - 파생 클래스를 작성해 정적으로 그 특성을 결정할 수 있습니다.
    - Component 의 경우, 특정 gameObject가 해당하는 컴포넌트를 가지고 있는지 런타임에 확인해야 하는 과정이 있을 수 있습니다.
      - 반면에 정적으로 멤버와 메서드가 결정된다면 컴파일 시간에 그 정보를 알 수 있으며 디버그도 보다 편리합니다.
    
2. GameObject의 특성을 나타내는 Component 클래스를 만들어 GameObject 의 멤버로 포함
    - 컴포넌트를 활용하면 공통되는 특성에 대한 코드 재사용률을 높일 수 있습니다.
    - 동적으로 메모리를 할당해서 사용하는 경우, 이에 대한 메모리 관리가 어려워질 수 있습니다.(내부 단편화가 발생할 수 있습니다.)
    - 서로 다른 종류의 컴포넌트간 통신은 까다로우면서 디버그할 때 추적하기가 어려울 수 있습니다.
    - 특정 컴포넌트를 확인할 때, gameObject의 컴포넌트들을 순회하는 경우, 추가 비용이 발생합니다.


 * 프로젝트의 규모가 크지 않은 점, component 구조가 코드 재사용성을 높일 수 있는 점, 과거 unity 엔진을 사용해서 component 시스템에 익숙한 점을 고려해 component system를 활용하기로 결정하였습니다.

---

![Scene Diagram](https://github.com/wlsvy/2020_DirectX_Project/blob/master/Image/Scene.png)

---

* 3D 그래픽스 이론 상, 렌더링 모델의 좌표 변환(Coordinate Transform)을 연산할 때, **부모 좌표계를 기반으로 자식의 좌표를 연산**하는 것이 일반적입니다.

* gameObject 객체의 위치정보를 저장하는 transform 구조를 설계할 때, Scene 클래스 안에 root 에 해당하는 Transform 을 가지게 하고 root의 자식으로 gameObject의 transform을 붙여나가는 tree 방식으로 구성하였습니다.

---


#### Reference
- [wiki Entity Component System](https://en.wikipedia.org/wiki/Entity_component_system)
- [Game Programming Pattern](http://gameprogrammingpatterns.com/component.html)


</details>

## Frame Update 동작


<details>
  <summary>접기/펼치기</summary>

![UpdatePrev](https://github.com/wlsvy/2020_DirectX_Project/blob/master/Image/UpdatePrev.png)

* 초기 Engine의 업데이트 방식을 구상할 때, gameObject가 포함하는 Component들을 순환하는 방식으로는 위의 다이어그램처럼 Engine의 Update() 호출이 전파되면서 모든 Component를 Update시키는 방식을 활용했습니다. 위 방식은 몇 가지 문제점이 있었습니다.
  - 모든 Component 가 프레임마다 Update 되어야 할 필요는 없습니다. 더 나아가 Component 파생 타입에 따라 다른 특징을 가지기 때문에 활용방식에도 차이가 있어야 했습니다. 
  - Component의 Update 순서가 GameObject 의 Update 순서에 의존합니다. 오브젝트 렌더링, 애니메이션 연산, 그 외 특정 컴포넌트 Update 등 다양한 기능이 체계없이 동작할 때 의도치 않은 결과를 발생시킬 수 있었습니다.


![HeapMemory](https://github.com/wlsvy/2020_DirectX_Project/blob/master/Image/HeapMemory.png)
![UpdateAfter](https://github.com/wlsvy/2020_DirectX_Project/blob/master/Image/UpdateAfter.png)

* 이전의 문제를 해결하고자 Pool 클래스에 Component들을 종류별로 할당시켰습니다.
  - 타입별로 Component가 분류되기 때문에, Engine 에서는 Component를 종류에 따라 적합한 방식으로 다룰 수 있게 되었습니다.
  - 또한 Update 순서에 있어서도 타입별로 호출 순서를 체계화 시킬 수 있었습니다.


</details>


## DirectX 11 api 활용

## 조명

## 주변광 차폐(SSAO Screen Space Ambient Occlusion)

## PostProcess 효과

## Cpu Side Frustum Culling

## Shadow Mapping

## Skeletal Animation

## Component System

## 기타

## 학습 내용
