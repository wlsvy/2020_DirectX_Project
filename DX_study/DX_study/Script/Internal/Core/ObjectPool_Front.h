#pragma once
#include "ObjectPool.h"
#include "GameObject.h"
#include "../Graphics/AnimationClip.h"
#include "../Graphics/Mesh.h"
#include "../Graphics/Model.h"
#include "../Graphics/Shaders.h"
#include "../Graphics/Material.h"
#include "../Graphics/Texture.h"
#include "../../Component/Behaviour.h"
#include "../../Component/Renderable.h"
#include "../../Component/Light.h"

namespace Core {

	class LightPool : public Singleton<LightPool> {
	public:
		//virtual void Initialize() { GetInstance(); }
		static std::shared_ptr<RenderInfo> ri;

		void PushToShadowMapRenderQueue(const std::shared_ptr<RenderInfo> & r) {
			ri = r;
			Pool<LightBase>::GetInstance().ForEach(func);
			/*for (auto& light : m_Objects) {
				if (!light->IsShadowEnable()) {
					continue;
				}

				for (auto& renderable : r->GetRenerables()) {
					auto& aabb = renderable.GetMesh()->GetLocalAABB();

					if (!light->CullRenderable(aabb)) {
						light->PushToRenderQueue(r);
					}
				}
			}*/
		}

		static void func(const std::shared_ptr<LightBase> & light) {
			if (!light->IsShadowEnable()) {
				return;
			}
			/*for (auto& renderable : ri->GetRenerables()) {
				auto& aabb = renderable.GetMesh()->GetLocalAABB();

				if (!light->CullRenderable(aabb)) {
					light->PushToRenderQueue(ri);
				}
				
			}*/
		}
		/*static void func2(const std::shared_ptr<LightBase> & light) {
			if (!light->IsShadowEnable()) {
				return;
			}

			for (auto& renderable : r->GetRenerables()) {
				auto& aabb = renderable.GetMesh()->GetLocalAABB();

				if (!light->CullRenderable(aabb)) {
					light->PushToRenderQueue(r);
				}
			}
		}

		void DrawShadowMap() {
			for (auto& light : m_Objects) {
				if (!light->IsShadowEnable()) {
					continue;
				}


			}
		}*/
	};

	
}