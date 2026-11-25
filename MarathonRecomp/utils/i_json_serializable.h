#pragma once

#include <nlohmann/json.hpp>
#include <utils/i_serializable.h>

class IJsonSerializable : public ISerializable<nlohmann::json, nlohmann::json> {};
