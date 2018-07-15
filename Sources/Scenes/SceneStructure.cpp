﻿#include "SceneStructure.hpp"

#include "Physics/IShape.hpp"

namespace fl
{
	SceneStructure::SceneStructure() :
		ISpatialStructure(),
		m_objects(std::vector<GameObject *>())
	{
	}

	SceneStructure::~SceneStructure()
	{
	}

	void SceneStructure::Add(GameObject *object)
	{
		m_objects.emplace_back(object);
	}

	void SceneStructure::Remove(GameObject *object)
	{
		auto found = std::find(m_objects.begin(), m_objects.end(), object);

		if (found != m_objects.end())
		{
			m_objects.erase(found);
		}
	}

	void SceneStructure::Clear()
	{
		m_objects.clear();
	}

	std::vector<GameObject *> SceneStructure::QueryAll()
	{
		auto result = std::vector<GameObject *>();

		for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
		{
			result.emplace_back(*it);
		}

		return result;
	}

	std::vector<GameObject *> SceneStructure::QueryFrustum(const Frustum &range)
	{
		auto result = std::vector<GameObject *>();

		for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
		{
			auto gameObject = static_cast<GameObject *>(*it);
			auto collider = gameObject->GetComponent<IShape>();

			if (collider == nullptr || collider->InFrustum(range))
			{
				result.emplace_back(*it);
			}
		}

		return result;
	}

	/*std::vector<GameObject *> SceneStructure::QueryBounding(IShape *range)
	{
		auto result = std::vector<GameObject *>();

		for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
		{
			auto gameObject = static_cast<GameObject *>(*it);
			auto collider = gameObject->GetComponent<IShape>();

			if (collider == nullptr || range->Intersects(*collider).IsIntersection() || range->Contains(*collider))
			{
				result.emplace_back(*it);
			}
		}

		return result;
	}*/

	bool SceneStructure::Contains(GameObject *object)
	{
		return std::find(m_objects.begin(), m_objects.end(), object) != m_objects.end();
	}
}