
# DirectX11 개인 프로젝트

#### 프로젝트 소개는 [여기](https://github.com/wlsvy/2020_DirectX_Project)에서 확인하실 수 있습니다.
#### 프로젝트를 진행하면서 공부한 내용은 [여기](https://github.com/wlsvy/TIL/edit/master/Document/Graphics/2020_DirectX_Project.md)에 정리하였습니다.

### 게임의 실시간 렌더링 기술과 DirectX 렌더링 파이프라인에 대해 공부했습니다.
- 렌더링 파이프라인은 3차원 모델을 2차원의 이미지로 표현하기 위한 단계적인 동작과정을 뜻합니다. DirectX 는 gpu에 접근해 프로그래머에게 그래픽스 처리 작업을 위해 필요한 api를 제공하며, 프로그래머는 쉐이더(Shader) hlsl 파일을 작성해 렌더링 파이프라인의 각 단계마다 동작과정을 명시합니다. 

- 그래픽스의 기초를 배우는 입장에서 렌더링 파이프라인이 데이터를 입력받는 방식과 결과물을 화면에 출력하는 동작, 각 단계가 수행하는 역할에 대한 이해가 필수적으로 뒷받침되어야 한다고 생각했습니다. 

- 실시간 렌더링 이론의 기초를 학습하기 위해 Tomas akenine-moller의 서적 '리얼타임 렌더링(2판)'을 참고했습니다. DirectX 프로그래밍에 대해서는 프랭크 D. 루나의 서적 'DirectX 11을 이용한 3D 게임 프로그래밍 입문'을 참고하여 기본 api를 다루는 법을 익힐 수 있었습니다.


### 렌더링 기술이 어떻게 응용되는지 알아보기 위해 실제 사용되는 렌더링 기법을 직접 구현했습니다.
- 렌더링 파이프라인의 각 단계는 그 자체로 많은 응용 가능성을 가지고 있습니다. 이 부분을 적극적으로 활용하는 것은 그래픽스 지식을 쌓는데 큰 도움이 될 것이라 생각했습니다. 

- 포워드 렌더링(forward rendering)과 지연 렌더링(deferred rendering) 중 저는 지연 렌더링을 기반으로 렌더링 시스템을 구축하기로 결정하였습니다. 포워드 렌더링은 한 단계의 pass를 거쳐 물체를 곧장 최종 결과물로 렌더링하는 반면, 지연 렌더링은 렌더링하는 과정을 나누어 물체를 두 단계 이상의 pass를 거치게 합니다. 첫 pass 에서는 물체의 정보(위치, 노말벡터 등)를 렌더타겟(G-buffer)에 저장하고, 통합된 물체들의 정보를 최종 pass를 통해 결과 화면을 렌더링한다는 점에서 지연시켜(deferred) 렌더링한다고 칭합니다. 지연 렌더링은 포워드 렌더링과 비교해 각종 장단점이 존재하지만, 렌더링하는 물체의 정보를 저장한다는 점은 추후 SSAO 같은 기법들을 구현할 때 분명 유리한 점이라 생각하였습니다. 
  - 지연렌더링을 바탕으로 SSAO, Shadow Mapping, Volumetric Lighting과 같은 렌더링 기법을 구현하였습니다.
  
- 지연 렌더링을 기반으로 한 다양한 기법들을 구현하면서 저는 게임의 화면을 표현할 때 어떤 과정들이 존재하고 그 원리에 대해 이해해가면서 그래픽스 프로그래밍 실력을 향상시켰습니다.

### Unity 엔진에서 사용되는 GameObject - Component 관계를 모티브로 게임동작 구조를 구현했습니다.
- GameObject 의 행동과 특성들을 정의하는 방식은 크게 두 가지 방법이 있습니다. GameObject를 상속받는 새로운 클래스 작성하여 정적으로 클래스의 기능을 명시하는 것과 GameObject의 특성을 나타내는 Component 클래스를 만들어 GameObject의 멤버로 포함시키는 방법이 그것입니다.

- 평소 Unity 엔진을 활용해왔던 제게는 컴포넌트를 활용하는 방식이 마치 당연한 것처럼 받아들여져 왔지만, 이번 기회를 통해 component를 활용한 방식이 항상 좋은 것은 아니라는 사실을 깨달았고 두 가지 방식이 가지는 장단점을 찾아보고 비교해볼 수 있었습니다.

- 정적인 상속 방식에 대해 component 방식이 가지는 장점은 이렇습니다. 동적으로 gameObject의 특성을 정의할 수 있다는 것이 대표적입니다. 파생클래스를 작성한다는 것은 컴파일 되는 시점에 그 클래스의 특성이 결정되어 바뀌지 않는다고 볼 수 있습니다. 하지만 component의 경우, 런타임 중에 component 객체를 만들어 gameObject에 포함시키거나 제외시킬 수 있습니다. 또한 공통되는 특성에 대해 하나의 component를 작성해 둔다면, 해당 component를 계속 재사용할 수 있는 이점이 있습니다.

- 반대에 입장에서는 이런 특징이 나타납니다. component는 동적으로 생성되어 gameObject에 할당될 수 있으므로 메모리를 동적으로 여러번 할당받아야 한다고 볼 수 있습니다. 이 경우, 특정 component의 할당 / 반환 작업이 여러번 이루어진다면 메모리 단편화 문제가 발생할 여지가 있습니다. 두번째는 정적인 클래스의 경우, 해당 클래스가 어떤 멤버/메서드을 포함하고 있는지는 코드를 작성하는 그 순간에도 확인할 수 있습니다. 그렇지만 component를 활용하는 경우, 특정 gameObject가 어떤 기능을 가지고 있는지 확인하고 싶을 때 해당 기능을 포함하는 component가 존재하는지 확인해야 합니다. 이 경우 gameObject의 component 배열을 한번 순회해야 하기 때문에 오버헤드 비용을 지불해야 할 수 있습니다. component 간의 통신을 할 때도 오류가 발생한다면 정적인 파생클래스의 경우보다 디버그 추적이 더 힘들 수 있습니다.

- Unity 엔진의 활용해본 경험이 있었던 만큼, component 구조를 먼저 활용해보고 부분적으로 파생클래스를 활용한 구조를 적용해보면서 두 경우가 가지는 장단점을 비교해보았습니다. Unity 의 경우 C# 언어가 가진 특징인 metadata의 활용과 garbage collection 의 메모리 압축 과정을 통해 보다 componet 구조를 효율적으로 활용할 수 있는 환경이었으며, unreal 엔진의 경우 두 가지 구조를 범용적으로 활용함으로써 각각의 장점을 활용한 구조라는 것을 알게 되었습니다. 

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


- Unity 엔진의 활용해본 경험이 있었던 만큼, component 구조를 먼저 활용해보고 부분적으로 파생클래스를 활용한 구조를 적용해보면서 두 경우가 가지는 장단점을 비교해보았습니다.

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
