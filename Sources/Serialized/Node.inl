#pragma once

#include "Helpers/ConstExpr.hpp"
#include "Helpers/String.hpp"
#include "Resources/Resource.hpp"
#include "Node.hpp"

namespace acid
{
template<typename T>
T Node::Get() const
{
	T value;
	*this >> value;
	return value;
}

template<typename T>
T Node::Get(const T &fallback) const
{
	if (!IsValid())
	{
		return fallback;
	}

	return Get<T>();
}

template<typename T>
void Node::Get(T &dest) const
{
	if (IsValid())
	{
		*this >> dest;
	}
}

template<typename T, typename K>
void Node::Get(T &dest, const K &fallback) const
{
	if (IsValid())
	{
		*this >> dest;
		return;
	}

	dest = fallback;
}

template<typename T>
void Node::Set(const T &value)
{
	*this << value;
}

template<typename T>
Node &Node::Append(T value)
{
	AddProperty() << value;
	return *this;
}

template<typename ...Args>
Node &Node::Append(Args ...args)
{
	(Append(args), ...);
	return *this;
}

template<typename T>
Node &Node::operator=(const T &rhs)
{
	Set(rhs);
	return *this;
}

/*const Node &operator>>(const Node &node, std::nullptr_t &object)
{
	object = nullptr;
	return node;
}*/

inline Node &operator<<(Node &node, const std::nullptr_t &object)
{
	node.SetValue("null");
	node.SetType(Node::Type::Null);
	return node;
}

inline const Node &operator>>(const Node &node, bool &object)
{
	object = String::From<bool>(node.GetValue());
	return node;
}

inline Node &operator<<(Node &node, bool object)
{
	node.SetValue(String::To(object));
	node.SetType(Node::Type::Boolean);
	return node;
}

template<typename T>
std::enable_if_t<std::is_arithmetic_v<T> || std::is_enum_v<T>, const Node &> operator>>(const Node &node, T &object)
{
	object = String::From<T>(node.GetValue());
	return node;
}

template<typename T>
std::enable_if_t<std::is_arithmetic_v<T> || std::is_enum_v<T>, Node &> operator<<(Node &node, const T &object)
{
	node.SetValue(String::To(object));
	node.SetType(Node::Type::Number);
	return node;
}

/*template<typename T>
std::enable_if_t<std::is_class_v<T> || std::is_pointer_v<T>, const Node &> operator>>(const Node &node, T &object)
{
	node >> ConstExpr::AsRef(object);
	return node;
}*/

template<typename T>
std::enable_if_t<std::is_class_v<T> || std::is_pointer_v<T>, Node &> operator<<(Node &node, const T &object)
{
	if (!ConstExpr::AsPtr(object))
	{
		return node << nullptr;
	}

	node << ConstExpr::AsRef(object);
	return node;
}

template<typename T>
const Node &operator>>(const Node &node, std::unique_ptr<T> &object)
{
	object = std::make_unique<T>();
	node >> *object;
	return node;
}

template<typename T>
Node &operator<<(Node &node, const std::unique_ptr<T> &object)
{
	if (!object)
	{
		return node << nullptr;
	}

	node << *object;
	return node;
}

template<typename T>
const Node &operator>>(const Node &node, std::shared_ptr<T> &object)
{
	// TODO: Abstract Resource streams out from shared_ptr.
	if constexpr (std::is_base_of_v<Resource, T>)
	{
		object = T::Create(node);
		return node;
	}
	else
	{
		object = std::make_shared<T>();
		node >> *object;
		return node;
	}
}

template<typename T>
Node &operator<<(Node &node, const std::shared_ptr<T> &object)
{
	if (!object)
	{
		return node << nullptr;
	}

	node << *object;
	return node;
}

/*inline const Node &operator>>(const Node &node, char *&string)
{
	std::strcpy(string, node.GetValue().c_str());
	return node;
}

inline Node &operator<<(Node &node, const char *string)
{
	node.SetValue(string);
	node.SetType(Node::Type::String);
	return node;
}*/

inline const Node &operator>>(const Node &node, std::string &string)
{
	string = node.GetValue();
	return node;
}

inline Node &operator<<(Node &node, const std::string &string)
{
	node.SetValue(string);
	node.SetType(Node::Type::String);
	return node;
}

inline const Node &operator>>(const Node &node, std::filesystem::path &object)
{
	object = node.GetValue();
	return node;
}

inline Node &operator<<(Node &node, const std::filesystem::path &object)
{
	auto str = object.string();
	std::replace(str.begin(), str.end(), '\\', '/');
	node.SetValue(str);
	node.SetType(Node::Type::String);
	return node;
}

template<typename T, typename K>
const Node &operator>>(const Node &node, std::pair<T, K> &pair)
{
	pair.first = String::From<T>(node.GetName());
	node >> pair.second;
	return node;
}

template<typename T, typename K>
Node &operator<<(Node &node, const std::pair<T, K> &pair)
{
	node.SetName(String::To(pair.first));
	node << pair.second;
	return node;
}

template<typename T>
const Node &operator>>(const Node &node, std::optional<T> &optional)
{
	if (node.GetValue() != "null")
	{
		T x;
		node >> x;
		optional = std::move(x);
	}
	else
	{
		optional = std::nullopt;
	}

	return node;
}

template<typename T>
Node &operator<<(Node &node, const std::optional<T> &optional)
{
	if (optional)
	{
		return node << *optional;
	}

	return node << nullptr;
}

template<typename T>
const Node &operator>>(const Node &node, std::vector<T> &vector)
{
	vector = {};
	vector.reserve(node.GetProperties().size());

	for (const auto &[propertyName, property] : node.GetProperties())
	{
		T x;
		property >> x;
		vector.emplace_back(std::move(x));
	}

	return node;
}

template<typename T>
Node &operator<<(Node &node, const std::vector<T> &vector)
{
	for (const auto &x : vector)
	{
		node.AddProperty() << x;
	}

	node.SetType(Node::Type::Array);
	return node;
}

template<typename T, typename K>
const Node &operator>>(const Node &node, std::map<T, K> &map)
{
	map = {};

	for (const auto &[propertyName, property] : node.GetProperties())
	{
		std::pair<T, K> pair;
		pair.first = String::From<T>(propertyName);
		property >> pair.second;
		map.emplace(std::move(pair));
	}

	return node;
}

template<typename T, typename K>
Node &operator<<(Node &node, const std::map<T, K> &map)
{
	for (const auto &x : map)
	{
		node.AddProperty() << x;
	}

	node.SetType(Node::Type::Array);
	return node;
}
}