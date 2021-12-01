#pragma once

#include <exception>
#include <unordered_set>

#include <RE/A/Actor.h>

#include <toml++/toml_table.h>

#include "FormId.hpp"

namespace RE {
    class TESDataHandler;
    class TESBoundObject;
} // namespace RE

class WBAConfig {
    std::unordered_set<FormId> _weightlessArrows;
    std::unordered_set<RE::TESBoundObject*> _weightlessArrowForms;

    explicit WBAConfig() {}

    void _readIndividualConfigs();
    std::size_t _readAndCountConfigs(const toml::table& table);

    void _loadWeightlessArrowForms(RE::TESDataHandler* dataHandler);

public:
    WBAConfig(const WBAConfig&) = delete;
    WBAConfig(WBAConfig&&) = delete;
    WBAConfig& operator=(const WBAConfig&) = delete;
    WBAConfig& operator=(WBAConfig&&) = delete;

    static WBAConfig& getInstance()
    {
        static WBAConfig instance;

        return instance;
    }

    // These functions needs to be called manually.
    // loadGameForms() must be run only after readConfig finishes.
    void readConfigs();
    void loadGameForms(RE::TESDataHandler* dataHandler);

    bool isWeightlessArrow(RE::TESBoundObject* const obj) const {
        return _weightlessArrowForms.contains(obj);
    }
};

class WBAConfigLoadError : public std::runtime_error {
public:
    explicit WBAConfigLoadError(const std::string& message);
};
