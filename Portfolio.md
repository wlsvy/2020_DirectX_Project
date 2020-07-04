
# DirectX11 개인 프로젝트

<!-- 
- Unity 엔진 등 상용엔진을 통해 진행해왔던 기존의 프로젝트들과는 다르게 게임이 구동하는 방식을 DirectX를 활용해 직접 구현하였습니다. 
-->

### 게임의 실시간 렌더링 기술과 DirectX 렌더링 파이프라인에 대해 공부했습니다.
- 렌더링 파이프라인은 3차원 모델을 2차원의 이미지로 표현하기 위한 단계적인 동작과정을 뜻합니다. DirectX 는 gpu에 접근해 프로그래머에게 그래픽스 처리 작업을 위해 필요한 api를 제공하며, 프로그래머는 쉐이더(Shader) hlsl 파일을 작성해 렌더링 파이프라인의 각 단계마다 동작과정을 명시합니다. 

- 그래픽스의 기초를 배우는 입장에서 렌더링 파이프라인이 데이터를 입력받는 방식과 결과물을 화면에 출력하는 동작, 각 단계가 수행하는 역할에 대한 이해가 필수적으로 뒷받침되어야 한다고 생각했습니다. 

- 실시간 렌더링 이론의 기초를 학습하기 위해 Tomas akenine-moller의 서적 '리얼타임 렌더링(2판)'을 참고했습니다. DirectX 프로그래밍에 대해서는 프랭크 D. 루나의 서적 'DirectX 11을 이용한 3D 게임 프로그래밍 입문'을 참고하여 기본 api를 다루는 법을 익힐 수 있었습니다.


### 렌더링 기술이 어떻게 응용되는지 알아보기 위해 실제 사용되는 렌더링 기법을 직접 구현했습니다.
- 렌더링 파이프라인의 각 단계는 그 자체로 많은 응용 가능성을 가지고 있습니다. 이 부분을 적극적으로 활용하는 것은 그래픽스 지식을 쌓는데 큰 도움이 될 것이라 생각했습니다. 

- 포워드 렌더링(forward rendering)과 지연 렌더링(deferred rendering) 중 저는 지연 렌더링을 기반으로 렌더링 시스템을 구축하기로 결정하였습니다. 포워드 렌더링은 한 단계의 pass를 거쳐 물체를 곧장 최종 결과물로 렌더링하는 반면, 지연 렌더링은 렌더링하는 과정을 나누어 물체를 두 단계 이상의 pass를 거치게 합니다. 첫 pass 에서는 물체의 정보(위치, 노말벡터 등)를 렌더타겟(G-buffer)에 저장하고, 통합된 물체들의 정보를 최종 pass를 통해 결과 화면을 렌더링한다는 점에서 지연시켜(deferred) 렌더링한다고 칭합니다. 지연 렌더링은 포워드 렌더링과 비교해 각종 장단점이 존재하지만, 렌더링하는 물체의 정보를 저장한다는 점은 추후 SSAO 같은 기법들을 구현할 때 분명 유리한 점이라 생각하였습니다. 
  - 저는 지연렌더링을 바탕으로 아래와 같은 렌더링 기법들을 구현하였습니다.
  
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

<!--
## DirectX 11 api 활용

<details>
  <summary>접기/펼치기</summary>

처음 DirectX 11 와 렌더링 이론에 대해 공부할 때는 

- ‘DirectX 11을 이용한 3D 게임 프로그래밍 입문’ / 프랭크 D. 루나 
- 'Real Time Rendering’ /   Tomas Akenine-Moller 외
- Jpres 의 [C++ DirectX 11 Tutorial](https://www.youtube.com/watch?v=gQIG77PfLgo&list=PLcacUGyBsOIBlGyQQWzp6D1Xn6ZENx9Y2) 유튜브 튜토리얼

을 참고하였습니다.

- 렌더링 파이프라인 기본 이론
- D3D11Device/SwapChain 을 포함한 각종 DirectX 자원을 초기화/다루는 방법
- Rasterizer/Sampler/Blend/DepthStencil State 등을 통해 렌더링 파이프라인을 활용하는 방법
- Vertex/Indice/Constant Buffer의 의미와 수행하는 역할
- Texture와 FBX 모델을 임포트하는 방법
- Vertex / Pixel shader 활용 방법
- Camera 클래스를 구현해 화면상에 3D model을 띄우는 방법
- 조명(Lightint)을 이용해 모델의 밝기를 표현하는 방법

등을 배울 수 있었습니다.

</details>

-->

## 조명

<details>
  <summary>접기/펼치기</summary>

#### [조명 연산 알고리즘Illumination models](https://en.wikipedia.org/wiki/Shading#Flat_shading) 과 조합해서 사용할 수 있는 밝기값 보간 방식

- `Flat shading` : 개별 삼각형에 대해 밝기값을 계산합니다.
  - 연산이 빠릅니다. 그러나 삼각형이 각진 부분 혹은 코너 부분에 있어서는 음양이 부드럽게 표현되지 않습니다.
  - specular light를 표현하는데 있어 좋지 못합니다. 직접적으로 반사되는 폴리곤은 면 전체가 똑같은 밝기로 빛나며 음영이 부드럽게 이어지지 않는 특징 때문에 자연스러운 연출이 되지 않습니다.
  - 또한 조명이 해당 폴리곤에 적당한 각도로 비추지 않는다면 specular lighting 요소는 아예 표현되지 않을 수도 있습니다.
- `Gouraud shading` : 삼각형의 각 정점에서 조명값을 정하고 계산된 색상을 삼각형 표면 전체로 보간합니다.
  - 정점의 조명값을 보간하는 특징 탓에 밝기가 부정확할 수 있습니다. (특히 specular lighting)
  - T 형태로 인접한 폴리곤의 경우 시각적으로 부자연스럽게 보일 수 있습니다.
  - 마하 밴드mach band 현상이 나타날 수 있습니다.
    - 두 개의 면이 만나는 경계선 부근에서 어두운 면은 더 어두워지고, 밝은 면은 더 밝게 보이는 일종의 착시 현상. 인간의 시각인식 체계 특징상 윤곽선을 추적하려는 경향에 의해 나타납니다.
- `Phong shading` : 각 정점에 저장된 법선을 이용하여 삼각형에 해당되는 각 픽셀의 법선 벡터를 보간합니다. 그 후 픽셀에 대해서 보간된 법선 벡터값을 활용해 밝기를 연산합니다.
  - 픽셀별 조명처리는 고러드gouraud 셰이딩 방식과 플랫flat 셰이딩 방식과 비교해서 연산이 복잡하고 비용이 많이 발생합니다.
  - 정반사광specular lighting을 가장 정확하게 표현할 수 있습니다.

#### 난반사Diffuse 관련
- 거친 표면의 성질 표현, 물리적 실제감 표현, 빛과 물체 표면과의 상호관계와 연관되어 있습니다.
- 광자가 난반사가 일어나는 표면에 도착하면 순간적으로 그 표면에 흡수됩니다.(주로 거친rough 표면에서 발생합니다) 광원으로부터 나온 광자의 색상과 물질의 색상에 따라서 광자가 완전히 흡수될 수도 있고, 임의의 방향으로 반사될 수도 있습니다. 난반사 성분은 시야 독립적입니다.(view - independent)

- [Lambert 법칙](https://en.wikipedia.org/wiki/Lambertian_reflectance) : 난반사만 일어날 수 있는 (완전히 거친) 표면에 반사된 빛을 계산하는 방법입니다.

#### 정반사specular 관련
- 하이라이트 생성함으로써 표면이 반짝거리도록 보이게 하고 굴곡을 보여주며 광원의 방향과 위치를 알게 해줍니다.
- 물리적 의미 : 광택 있는 표면에서 입사되는 광자가 반사 방향으로 튕겨져 나가는 원리입니다.

- [phong reflection](https://en.wikipedia.org/wiki/Phong_reflection_model) : 조명의 반사 벡터와 시야 벡터를 내적한 성분을 활용해 정반사 성분을 구합니다.
- [blinn - phong reflection](https://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_reflection_model) : halfway vector(조명 방향 벡터와 시야 방향 벡터의 중간값 벡터)를 활용해 정반사 성분을 구합니다.

![](https://upload.wikimedia.org/wikipedia/commons/thumb/e/e9/Blinn_phong_comparison.png/600px-Blinn_phong_comparison.png)
- phong 방식이 정반사광을 원형으로 나타낸다면, blinn phong 방식은 보다 타원형으로 나타냅니다. 강이나 바다에 반사되는 햇빛이 완벽한 원형의 모습을 유지하기보다는 수직방향으로 좀더 늘어져 보이는 것을 떠올리시면 됩니다.
- 조명이 굉장히 멀리 있는 경우(ex : Directional Light) 이면서 정사영orthographic/isometric 카메라를 활용하고 있을 때, halfway vector는 고정된 값으로 연산할 수 있기 때문에 phong 방식 보다 blinn-phong 방식이 더 빠를 수 있습니다.

</details>

## 주변광 차폐(SSAO Screen Space Ambient Occlusion)

<details>
  <summary>접기/펼치기</summary>
  
  
- `주변광 차폐(ambient occlusion)` 방식은 물체 표면의 특정한 부분이 노출되어 있다면 밝게, 둘러싸여져 있다면 그 부분은 주변광을 덜 받는 것으로 가정하고 보다 어둡게 표현하는 기법입니다.
  - 주름 혹은 방 가장자리의 모서리 부분을 예시로 생각하시면 됩니다.
  - SSAO는 screen space내의 정보를 활용해 정확하지는 않지만 보다 가벼운 연산으로 주변광 차폐값의 근사치를 구하는 방법입니다.

![](http://farm5.static.flickr.com/4026/4639752338_7a574740e9.jpg)

- 지연 렌더링 deferred rendering 환경에서는 쉽게 구현할 수 있습니다. screen space에 픽셀에는 렌더링 된 물체들의 위치/노말 벡터 정보가 저장되어 있다고 가정하겠습니다.
- `이웃한 픽셀의 위치 벡터 - 현재 연산하는 픽셀의 위치 벡터` 와 `현재 연산하는 픽셀의 노말 벡터`를 내적한 성분을 활용하면 됩니다. 내적값을 통해 두 벡터 사이의 각도를 알 수 있으며 해당값은 인접한 픽셀에 렌더링된 물체가 현재 픽셀을 얼마나 둘러싸는지(깊이 depth 차이가 얼마인지) 나타냅니다.

![](http://farm5.static.flickr.com/4054/4639143389_42b13c5ef6.jpg)
![](http://farm5.static.flickr.com/4030/4639143415_444cde1085.jpg)
- 벡터를 이용한 연산은 꽤 그럴듯한 결과를 보여줍니다. 하지만 이외에도 depth 버퍼를 활용하여 깊이값을 비교하는 방식을 떠올릴 수 있습니다.
  - 구현이 간편하고 더 빠른 연산이 가능해보이지만 문제점이 있는데 스스로를 가리는 현상self occlusion 과 모서리 주변 부분이 밝아지는 헤일로halo 현상이 나타난다는 것입니다.
  
- SSAO 방식은 빠르고 화면에 동적으로 움직이는 객체가 많을 때 효과적일 수 있습니다. 지연렌더링을 기반으로 하고 있다면 구현하기도 쉽습니다. 하지만 SSAO가 가지고 있는 단점도 역시 잘 고려해야 할 것이며 적절한 상황에 맞게 기법을 적용해야 할 것입니다.
  - 숨겨진 기하geometry 물체들, 특히 절두체 바깥의 물체들은 연산 과정에서 제외합니다.
  - 샘플링sampling(여기서는 몇 개의 인접한 픽셀과 비교하는지) 에 따라 성능 차이가 발생합니다.
  - 결과가 지저분할 수(noisy) 있습니다. 가우시안 블러와 같은 추가적인 작업이 필요할 수 있습니다.

#### Reference
- [Game Dev](https://www.gamedev.net/tutorials/programming/graphics/a-simple-and-practical-approach-to-ssao-r2753/)

</details>

## PostProcess 효과

<details>
  <summary>접기/펼치기</summary>
  
- 3D 렌더링에서 `후처리 효과(Post Processing)` 은 렌더링 결과물을 곧바로 화면에 출력하기 전 일종의 필터 역할을 하는 효과들을 적용하는 방식일 일컫습니다.

### Bloom

- `블룸(bloom)` 효과는 화면의 밝은 영역의 경계에서 빛이 발산되는 것 같은 효과를 주는 기법입니다.
  - 블룸 이펙트는 기존 이미지를 블러blurr 처리하여 구현할 수 있습니다. 블룸bloom 이펙트를 구현하기 이전에는 블러 처리가 먼저 이루어져야 합니다.
  
![bilinear downsampling](https://catlikecoding.com/unity/tutorials/advanced-rendering/bloom/blurring/bilinear-downsampling.png)

- 블러 효과를 적용할 때 다수의 픽셀들의 평균 색상값을 구하려 할 경우, 연산하는 픽셀의 수가 많다면 연산 비용이 증가합니다. 이를 피하기 위해 우선 다운 샘플링downsampling 을 활용하는 방법이 있습니다.
  - 이때 박스 샘플링(box sampling) 방식을 이용할 수 있습니다.

![](https://catlikecoding.com/unity/tutorials/advanced-rendering/bloom/creating-bloom/additive-blurring.png)

- 여러 단계의 다운 샘플링을 거친 이미지를 기존의 화면에 적용시켜야 할 것입니다. 단순히 다수의 다운 샘플링을 거친 이미지 하나 만을 적용한다면 예상보다 균등하게 흐릿해져 제대로 된 빛 발산 효과가 나타나지 않을 것입니다. 다운 샘플링을 거친 중간 단계의 이미지들 까지 중첩시켜서 적용해야 합니다. 

- 블러 이미지를 결과물에 적용할 때는 `Additive blending`을 적용해야 합니다. 블렌딩 방식이 혹시나 기존의 색상을 대체하는 방식이라면 역시 결과물의 빛 발산 효과가 제대로 나타나지 않을 것입니다.

### 감마 보정Gamma Correction

- 감마(gamma)라는 개념 사이에는 엔지니어 사이에도 개념 차이가 존재하며 이에 따른 혼란도 적지 않다고 한다. 적당히 '입력값에 대한 비선형적 출력 특성의 결정 요인' 정도로 이해하면 좋을 것 같다.

![](https://dthumb-phinf.pstatic.net/?src=%22http%3A%2F%2Fwww.monitor4u.co.kr%2Flesson%2FContentImg%2FGamma_Tour_01_sRGB-550.png%22&type=m10000_10000)

- 실제로 감마보정은 영상 처리 분야에서 생각보다 세분화된 단계로 적용된다.

감마보정의 주요 목적은
- 인간의 시야가 색상을 인식할 때 비선형적인 방식으로 인식합니다. 이때 밝은 색조보다는 어두운 색조의 대해서 더 민감하게 인식하는 경향이 있다. 일반적인 선형 색상으로 화면을 표시하려 한다면, 어두운 부분의 밝기가 변할 때 부드럽게 느껴지지 않고 단절되어 보이는 현상(posterization)이 발생합니다. 따라서 주어진 정보표현량의 한계 안에서 최적의 화질을 보여주기 위해선 비선형적으로 부호화하여 어두운 부분을 더 자세히 표현할 필요가 있습니다.
- CRT 음극선상의 특징상 비선형적으로 색상을 표현하기 때문에 원본의 색상을 정상적으로 나타내기 위해 소프트웨어 적으로 보정을 적용하는 이유도 있다고 하지만 주된 이유는 아니라고 합니다. 입력 전류를 조정함으로써 이 부분은 소프트웨어 측 보정 없이 해결할 수 있다고 합니다.
  - 하기야 디스플레이 장치 종류에 따라 보정값 수치가 바뀔 수 있는데 그 부분을 소프트웨어 측에서 책임지는 것은 올바른 해법이 아닌 것 같습니다.

[출처](https://ko.wikipedia.org/wiki/%EA%B0%90%EB%A7%88_%EB%B3%B4%EC%A0%95), [출처2](https://en.wikipedia.org/wiki/Gamma_correction)

```c++
void main()
{
    // do super fancy lighting in linear space
    [...]
    // apply gamma correction
    float gamma = 2.2;
    FragColor.rgb = pow(fragColor.rgb, vec3(1.0/gamma));
}
```
[감마 보정 예시 OpenGL](https://learnopengl.com/Advanced-Lighting/Gamma-Correction)

### Tone Mapping

- HDR(high dynamic range) : 폭넓은 범위(색상으로 출력하는 0~1 범위를 넘어선)의 색상을 표현하는 방식입니다. 디지털 색상을 출력할 때, 색상의 최대 범위를 넘어선 값을 단순히 하얀색으로 표현하지 않으며(혹은 최소 범위 보다도 작은 값을 검은색으로 표현하는 등) 노출exposure 값을 통해  

- 톤 맵핑(Tone Mapping)은 HDR 색상을 제한된 범위의 색상(LDR low dynamic range)으로 변환시키는 방법입니다. HDR 이미지를 그대로 출력한다면 디스플레이 장치가 표현할 수 있는 색상 범위를 넘어선 값은 제대로 표현하지 못하는 문제를 해결합니다.
  - 톤 맵핑의 목적은 디스플레이 장치가 제한된 밝기만을 표현할 수 있을 때, 실제 장면이 자연스럽게 화면상에 표시될 수 있도록 하는 것입니다.
  - 카메라가 빛에 노출되는 정도exposure 값을 조정하여 hdr 색상 표현 범위를 ldr 범위에 맞게 조정할 수 있습니다.
    - 사람의 눈을 예시로 들면, 어두운 밤에는 exposure 를 높여 눈이 더 많은 빛을 받아들이게 합니다. 이를 통해 어두운 영역을 상대적으로 밝게 인식할 수 있습니다.
    - 반대로 밝은 낮에는 exposure를 높여 눈이 빛을 덜 받아들게 합니다. 결과적으로 밝은 영역을 상대적으로 어둡게 인식하게 됩니다.
  
![](https://learnopengl.com/img/advanced-lighting/hdr_exposure.png)
  
```c++
uniform float exposure;

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
  
    // exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    // gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));
  
    FragColor = vec4(mapped, 1.0);
}  
```
[코드 출처](https://learnopengl.com/Advanced-Lighting/HDR)

#### Reference
- [wiki](https://en.wikipedia.org/wiki/Video_post-processing)
- [bloom](https://catlikecoding.com/unity/tutorials/advanced-rendering/bloom/)
- [감마 보정](https://blog.naver.com/psy2993/90081371916)

</details>

## Cpu Side Frustum Culling

<details>
  <summary>접기/펼치기</summary>
  
</details>

## Shadow Mapping

<details>
  <summary>접기/펼치기</summary>
  
</details>

## Skeletal Animation

<details>
  <summary>접기/펼치기</summary>
  
</details>
